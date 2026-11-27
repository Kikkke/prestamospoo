#include "Simulador.h"
#include <QRegularExpression>
#include <QLocale>
#include <QSqlQuery>
#include <QVariant>
#include <cmath>

namespace {
bool leerDecimal(const QString &texto,qint64 &centavos) {
    // Rechaza comas, exponentes, signos, decimales extra y desbordamientos.
    const QString valor=texto.trimmed();
    static const QRegularExpression formato("\\A[0-9]+(?:\\.[0-9]{1,2})?\\z");
    if(valor.size()>16 || !formato.match(valor).hasMatch())return false;
    bool ok=false;
    const qint64 entero=valor.section('.',0,0).toLongLong(&ok);
    if(!ok || entero>10000000)return false;
    const QString fraccion=valor.contains('.')?valor.section('.',1).leftJustified(2,'0'):"00";
    centavos=entero*100+fraccion.toInt();return true;
}
qint64 interesMensual(qint64 saldo,int tasa) {
    // Redondeo a centavos: mitad hacia arriba, exacto para cantidades no negativas.
    return (saldo*qint64(tasa)+60000)/120000;
}
CalculoPrestamo fallo(const QString &error) {CalculoPrestamo r;r.error=error;return r;}
}
QString MotorPrestamo::decimal(qint64 centavos) {
    return QString::number(centavos/100)+"."+QString::number(centavos%100).rightJustified(2,'0');
}
QString MotorPrestamo::dinero(qint64 centavos) {
    return QLocale(QLocale::Spanish,QLocale::Mexico).toString(centavos/100)+"."+QString::number(centavos%100).rightJustified(2,'0');
}
QString MotorPrestamo::validar(const CondicionesPrestamo &d) {
    static const QRegularExpression nombre("\\A[\\p{L}\\p{M}\\p{N} _]+\\z");
    if(d.nombre.trimmed().isEmpty() || d.nombre.size()>50 || !nombre.match(d.nombre).hasMatch()
       || !QRegularExpression("[\\p{L}\\p{N}_]").match(d.nombre).hasMatch())
        return "Nombre: de 1 a 50 caracteres; utiliza letras, números, espacios o guion bajo (_).";
    if(d.monto<100 || d.monto>1000000000)return "Monto: de $1.00 a $10,000,000.00 MXN.";
    if(d.tasa<0 || d.tasa>20000)return "Tasa anual: de 0.00 % a 200.00 %.";
    if(d.meses<1 || d.meses>480)return "Plazo: un número entero de 1 a 480 meses.";
    return {};
}
CalculoPrestamo MotorPrestamo::calcular(const EntradaPrestamo &e) {
    CondicionesPrestamo d;
    d.nombre=e.nombre.normalized(QString::NormalizationForm_C).trimmed();
    if(e.nombre.size()>50)return fallo("Nombre: máximo 50 caracteres.");
    qint64 tasa=0;
    if(!leerDecimal(e.monto,d.monto))return fallo("Monto inválido: usa números, punto decimal y hasta dos decimales, sin separadores de miles.");
    if(!leerDecimal(e.tasa,tasa) || tasa>20000)return fallo("Tasa anual: de 0.00 % a 200.00 %, con hasta dos decimales y punto decimal.");
    d.tasa=int(tasa);
    const QString plazo=e.meses.trimmed();bool ok=false;
    if(plazo.size()>3 || !QRegularExpression("\\A[0-9]+\\z").match(plazo).hasMatch())return fallo("Plazo: un número entero de 1 a 480 meses, sin decimales.");
    d.meses=plazo.toInt(&ok);if(!ok)return fallo("Plazo inválido.");
    return calcular(d);
}
CalculoPrestamo MotorPrestamo::calcular(const CondicionesPrestamo &d) {
    const auto error=validar(d);if(!error.isEmpty())return fallo(error);
    CalculoPrestamo r;r.condiciones=d;
    r.condiciones.nombre=d.nombre.normalized(QString::NormalizationForm_C).trimmed();
    const long double tasa=d.tasa/120000.0L;
    const long double teorica=d.tasa==0?static_cast<long double>(d.monto)/d.meses:
        d.monto*tasa/(-std::expm1(-d.meses*std::log1p(tasa)));
    if(!std::isfinite(teorica) || teorica<0)return fallo("No se pudo calcular una cuota válida.");
    r.regular=std::llround(teorica);
    qint64 saldo=d.monto,capitalTotal=0;
    bool ajusteInteres=false,ceros=false;
    for(int mes=1;mes<=d.meses;++mes) {
        const qint64 interes=interesMensual(saldo,d.tasa);
        // El interés siempre se cubre; nunca se crea capital negativo por redondeo.
        const qint64 capital=mes==d.meses?saldo:qBound<qint64>(0,r.regular-interes,saldo);
        const qint64 pago=capital+interes;
        ajusteInteres|=(mes<d.meses && interes>r.regular);
        ceros|=(mes<d.meses && pago==0);
        r.cuotas.append({mes,saldo,interes,capital,pago,saldo-capital});
        saldo-=capital;capitalTotal+=capital;r.intereses+=interes;r.total+=pago;
    }
    if(saldo!=0 || capitalTotal!=d.monto || r.total!=d.monto+r.intereses)return fallo("No se pudo cuadrar la amortización.");
    r.aviso="Último pago: $"+dinero(r.cuotas.last().pago)+" MXN. El último pago liquida el saldo restante.";
    if(ceros)r.aviso+=" El redondeo produce pagos de $0.00 en algunos periodos; consulta la tabla completa.";
    if(ajusteInteres)r.aviso+=" Algunos pagos se ajustan para cubrir el interés del mes sin aumentar la deuda.";
    return r;
}

GuardadoSimulacion ServicioSimulaciones::guardar(const Sesion &s,const CondicionesPrestamo &datos) {
    auto fallo=[](const QString &mensaje){GuardadoSimulacion r;r.error=mensaje;return r;};
    if(!s.valida() || s.rol!=Rol::Cliente || s.restringida)return fallo("Solo un cliente habilitado puede guardar simulaciones.");
    // Recalcula desde las condiciones: el llamador no puede alterar cuotas ni totales.
    const auto calculo=MotorPrestamo::calcular(datos);
    if(!calculo.valido())return fallo(calculo.error);
    if(!bd.transaction())return fallo("No se pudo iniciar el guardado. Revisa la conexión a MySQL.");
    struct Transaccion {QSqlDatabase bd;bool activa=true;~Transaccion(){if(activa)bd.rollback();}} tx{bd};
    QSqlQuery q(bd);
    q.prepare("SELECT u.id FROM usuario u JOIN cliente c ON c.usuario_id=u.id WHERE u.id=:id AND NOT EXISTS(SELECT 1 FROM administrador WHERE usuario_id=u.id) FOR UPDATE");
    q.bindValue(":id",s.usuarioId);
    if(!q.exec() || !q.next())return fallo("No se pudo verificar tu cuenta. Vuelve a iniciar sesión.");
    q.finish();
    q.prepare("SELECT id FROM deshabilitacion WHERE cliente_id=:id AND reactivada_en IS NULL FOR UPDATE");
    q.bindValue(":id",s.usuarioId);
    if(!q.exec())return fallo("No se pudo comprobar el estado de tu cuenta.");
    if(q.next())return fallo("Tu cuenta está deshabilitada. No se guardó la simulación.");
    q.finish();
    q.prepare("INSERT INTO simulacion(cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES(:id,:nombre,:monto,:tasa,:meses,'MXN',NOW(),NOW())");
    q.bindValue(":id",s.usuarioId);q.bindValue(":nombre",calculo.condiciones.nombre);
    q.bindValue(":monto",MotorPrestamo::decimal(datos.monto));q.bindValue(":tasa",MotorPrestamo::decimal(datos.tasa));q.bindValue(":meses",datos.meses);
    if(!q.exec())return fallo("No se pudo guardar la simulación. No se conservarán datos parciales.");
    const int id=q.lastInsertId().toInt();
    q.prepare("INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES(:id,:numero,:interes,:capital)");
    for(const auto &cuota:calculo.cuotas) {
        q.bindValue(":id",id);q.bindValue(":numero",cuota.numero);
        q.bindValue(":interes",MotorPrestamo::decimal(cuota.interes));q.bindValue(":capital",MotorPrestamo::decimal(cuota.capital));
        if(!q.exec())return fallo("Falló el guardado de las cuotas. Se revierte la simulación completa.");
    }
    if(!bd.commit())return fallo("MySQL no confirmó el guardado. Comprueba la conexión y el registro antes de reintentar.");
    tx.activa=false;GuardadoSimulacion resultado;resultado.id=id;return resultado;
}
