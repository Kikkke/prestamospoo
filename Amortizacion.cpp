#include "Amortizacion.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QSaveFile>
#include <QPdfWriter>
#include <QPainter>
#include <QPageSize>
#include <QImage>
#include <QLocale>

namespace {
// Lectura consistente. El rollback libera bloqueos; aquí no se escribe en MySQL.
struct Lectura {
    QSqlDatabase bd;bool abierta;
    explicit Lectura(QSqlDatabase b):bd(b),abierta(bd.transaction()){}
    ~Lectura(){if(abierta)bd.rollback();}
};
ResultadoDetalle fallo(const QString &error){ResultadoDetalle r;r.error=error;return r;}
const QString errorBD="No se pudo consultar la amortización. Revisa la conexión y pulsa Actualizar.";
const QString inconsistente="Las cuotas guardadas están incompletas o son inconsistentes. No se mostrarán ni exportarán resultados incorrectos.";

bool dibujarPdf(const DetalleAmortizacion &d,QSaveFile &archivo,int &paginas) {
    bool terminado=false;
    {
        QPdfWriter pdf(&archivo);pdf.setResolution(96);pdf.setPageSize(QPageSize(QPageSize::A4));
        pdf.setPageMargins(QMarginsF(15,15,15,15));pdf.setTitle(d.nombre+" - Amortización");pdf.setCreator("FinancionantiMex");
        QPainter p;if(!p.begin(&pdf))return false;
        const int ancho=pdf.width(),alto=pdf.height(),porPagina=22;
        paginas=(d.cuotas.size()+porPagina-1)/porPagina;
        const auto emitido=QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss");
        const QImage logo(QStringLiteral(ASSETS_DIR)+"/FinancionantiMex-logo.png");
        const QLocale mx(QLocale::Spanish,QLocale::Mexico);
        auto dinero=[](qint64 valor){return MotorPrestamo::dinero(valor);};
        for(int pagina=0;pagina<paginas;++pagina) {
            if(pagina && !pdf.newPage()){p.end();return false;}
            p.fillRect(QRect(0,0,ancho,alto),Qt::white);p.setPen(QColor("#222222"));
            if(!logo.isNull())p.drawImage(QRect(0,0,46,46),logo);
            p.setFont(QFont("Segoe UI",16,QFont::Bold));p.drawText(60,28,"FinancionantiMex");
            p.setFont(QFont("Segoe UI",11,QFont::Bold));
            p.drawText(QRect(0,53,ancho,67),Qt::TextWrapAnywhere|Qt::AlignVCenter,d.nombre+" - Tabla de amortización");
            p.setFont(QFont("Segoe UI",9));
            p.drawText(0,134,"Guardada: "+d.creada.toString("dd/MM/yyyy HH:mm:ss")+"   |   Actualizada: "+d.actualizada.toString("dd/MM/yyyy HH:mm:ss"));
            p.drawText(0,153,"Emisión: "+emitido+"   |   Simulación "+QString::number(d.id));
            p.drawText(0,174,QString("Monto: $%1 MXN | Tasa anual: %2 % | %3 pagos mensuales").arg(dinero(d.monto),mx.toString(d.tasaDiezmilesimas/10000.0,'f',d.tasaDiezmilesimas%100?4:2)).arg(d.meses));
            p.drawText(0,195,QString("Primer pago: $%1 | Último pago: $%2").arg(dinero(d.cuotas.first().pago),dinero(d.cuotas.last().pago)));
            const QStringList encabezados={"Pago","Saldo inicial","Mensualidad","Intereses","Abono a capital","Saldo final"};
            const int indice=42,cw=(ancho-indice)/5;
            auto celda=[=](int col,int y,int altoFila){return QRect(col==0?0:indice+(col-1)*cw,y,(col==0?indice:cw)-6,altoFila);};
            int y=216;
            p.fillRect(0,y,ancho,30,QColor("#ed916f"));p.setFont(QFont("Segoe UI",8,QFont::Bold));
            for(int c=0;c<6;++c)p.drawText(celda(c,y,30),Qt::AlignRight|Qt::AlignVCenter,encabezados[c]);
            y+=30;p.setFont(QFont("Segoe UI",8));
            for(int n=pagina*porPagina;n<qMin(int(d.cuotas.size()),(pagina+1)*porPagina);++n) {
                const auto &r=d.cuotas[n];if(n%2==0)p.fillRect(0,y,ancho,24,QColor("#f3f1f0"));
                const QStringList valores={QString::number(r.numero),dinero(r.inicial),dinero(r.pago),dinero(r.interes),dinero(r.capital),dinero(r.final)};
                for(int c=0;c<6;++c)p.drawText(celda(c,y,24),Qt::AlignRight|Qt::AlignVCenter,valores[c]);
                y+=24;
            }
            if(pagina==paginas-1) {
                p.setFont(QFont("Segoe UI",9,QFont::Bold));p.drawText(0,y+23,"Totales de la simulación");
                const QStringList titulos={"Capital","Intereses","Total de pagos"};
                const QList<qint64> valores={d.monto,d.intereses,d.total};
                for(int c=0;c<3;++c) {
                    p.setFont(QFont("Segoe UI",9));p.drawText(c*ancho/3,y+44,titulos[c]);
                    p.setFont(QFont("Segoe UI",10,QFont::Bold));p.drawText(c*ancho/3,y+65,"$"+dinero(valores[c])+" MXN");
                }
            }
            p.setFont(QFont("Segoe UI",8));
            p.drawText(QRect(0,alto-62,ancho,38),Qt::TextWordWrap,"Importes en MXN, según las cuotas guardadas. Sin comisiones, seguros ni impuestos. Documento informativo: no acredita una deuda ni la aprobación de un préstamo.");
            p.drawText(QRect(0,alto-22,ancho,22),Qt::AlignRight,QString("Página %1 de %2").arg(pagina+1).arg(paginas));
        }
        terminado=p.end();
    } // QPdfWriter termina el documento antes de revisar y confirmar el archivo.
    return terminado && archivo.error()==QFileDevice::NoError;
}
}

ResultadoDetalle ServicioAmortizacion::leer(const Sesion &s,int id) {
    if(!s.valida() || s.rol!=Rol::Cliente || s.restringida || id<=0)return fallo("No tienes acceso a esta simulación.");
    QSqlQuery q(bd);
    q.prepare("SELECT u.id FROM usuario u JOIN cliente c ON c.usuario_id=u.id WHERE u.id=:id AND NOT EXISTS(SELECT 1 FROM administrador WHERE usuario_id=u.id) FOR UPDATE");
    q.bindValue(":id",s.usuarioId);
    if(!q.exec())return fallo(errorBD);
    if(!q.next())return fallo("Tu cuenta no tiene acceso a esta simulación.");q.finish();
    q.prepare("SELECT id FROM deshabilitacion WHERE cliente_id=:id AND reactivada_en IS NULL FOR UPDATE");q.bindValue(":id",s.usuarioId);
    if(!q.exec())return fallo(errorBD);
    if(q.next())return fallo("Tu cuenta está deshabilitada. No puedes consultar ni exportar simulaciones.");q.finish();
    q.prepare("SELECT id,nombre,CAST(monto*100 AS SIGNED),CAST(tasa_anual*10000 AS SIGNED),meses,creada_en,actualizada_en,moneda FROM simulacion WHERE id=:sim AND cliente_id=:cliente AND eliminada_en IS NULL FOR UPDATE");
    q.bindValue(":sim",id);q.bindValue(":cliente",s.usuarioId);
    if(!q.exec())return fallo(errorBD);
    if(!q.next())return fallo("La simulación ya no está disponible en tu cuenta. Puede haber sido eliminada.");
    for(int c=0;c<8;++c)if(q.value(c).isNull())return fallo(inconsistente);
    ResultadoDetalle r;auto &d=r.detalle;
    d.id=q.value(0).toInt();d.nombre=q.value(1).toString();d.monto=q.value(2).toLongLong();d.tasaDiezmilesimas=q.value(3).toInt();d.meses=q.value(4).toInt();d.creada=q.value(5).toDateTime();d.actualizada=q.value(6).toDateTime();
    if(d.nombre.trimmed().isEmpty() || d.nombre.size()>160 || d.monto<100 || d.monto>1000000000 || d.tasaDiezmilesimas<0 || d.tasaDiezmilesimas>2000000 || d.meses<1 || d.meses>480 || !d.creada.isValid() || !d.actualizada.isValid() || q.value(7).toString()!="MXN")return fallo(inconsistente);
    q.finish();
    q.prepare("SELECT numero,CAST(interes*100 AS SIGNED),CAST(abono_capital*100 AS SIGNED) FROM cuota WHERE simulacion_id=:id ORDER BY numero FOR UPDATE");q.bindValue(":id",id);
    if(!q.exec())return fallo(errorBD);
    qint64 saldo=d.monto,capitalTotal=0;int numero=1;
    while(q.next()) {
        if(q.value(0).isNull() || q.value(1).isNull() || q.value(2).isNull())return fallo(inconsistente);
        const qint64 interes=q.value(1).toLongLong(),capital=q.value(2).toLongLong();
        if(numero>d.meses || q.value(0).toInt()!=numero || interes<0 || interes>d.monto || capital<0 || capital>saldo)return fallo(inconsistente);
        d.cuotas.append({numero++,saldo,interes,capital,interes+capital,saldo-capital});
        saldo-=capital;capitalTotal+=capital;d.intereses+=interes;d.total+=interes+capital;
    }
    if(q.lastError().isValid())return fallo(errorBD);
    if(d.cuotas.size()!=d.meses || saldo!=0 || capitalTotal!=d.monto || d.total!=d.monto+d.intereses)return fallo(inconsistente);
    return r;
}
ResultadoDetalle ServicioAmortizacion::consultar(const Sesion &s,int id) {
    Lectura tx(bd);if(!tx.abierta)return fallo(errorBD);return leer(s,id);
}
ResultadoPdf ServicioAmortizacion::exportar(const Sesion &s,int id,const QString &ruta) {
    ResultadoPdf r;if(ruta.isEmpty()){r.cancelado=true;return r;}
    Lectura tx(bd);if(!tx.abierta){r.error=errorBD;return r;}
    r.consulta=leer(s,id);if(!r.consulta.correcto()){r.error=r.consulta.error;return r;}
    const QString destino=ruta.endsWith(".pdf",Qt::CaseInsensitive)?ruta:ruta+".pdf";
    QSaveFile archivo(destino);archivo.setDirectWriteFallback(false);
    if(!archivo.open(QIODevice::WriteOnly)){r.error="No se pudo crear el PDF. Revisa la carpeta y los permisos de escritura.";return r;}
    if(!dibujarPdf(r.consulta.detalle,archivo,r.paginas)){archivo.cancelWriting();r.error="No se pudo escribir el PDF completo. No se reemplazó el archivo anterior.";return r;}
    // Los bloqueos de lectura siguen activos hasta confirmar el archivo completo.
    if(!archivo.commit()){r.error="No se pudo confirmar el PDF. Revisa permisos, espacio disponible o si el archivo está abierto.";return r;}
    return r;
}
