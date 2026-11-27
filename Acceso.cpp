#include "Acceso.h"
#include <QPasswordDigestor>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QSqlQuery>
#include <QVariant>

namespace {
constexpr int iteraciones = 600000;
const QString errorSql = QStringLiteral("No se pudo completar la operación en MySQL. Revisa la conexión y vuelve a intentarlo.");
// Revierte todos los retornos de error; el commit se solicita expresamente.
class Transaccion {
public:
    explicit Transaccion(QSqlDatabase db) : bd(db), abierta(bd.transaction()) {}
    ~Transaccion() { if (abierta) bd.rollback(); }
    bool disponible() const { return abierta; }
    bool confirmar() { if (!abierta || !bd.commit()) return false; abierta = false; return true; }
private:
    QSqlDatabase bd;
    bool abierta;
};
QString correoNormal(const QString &correo) { return correo.trimmed().toLower(); }
QString fechaAhora() { return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"); }
ResultadoAcceso fallo(const QString &error) { ResultadoAcceso r; r.error = error; return r; }
}

QString Validaciones::correo(const QString &valor) {
    const QString correo = correoNormal(valor);
    const QRegularExpression formatoCorreo("^[A-Za-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[A-Za-z0-9!#$%&'*+/=?^_`{|}~-]+)*@[A-Za-z0-9](?:[A-Za-z0-9-]*[A-Za-z0-9])?(?:\\.[A-Za-z0-9](?:[A-Za-z0-9-]*[A-Za-z0-9])?)+$");
    if (correo.isEmpty()) return "Introduce tu correo electrónico.";
    if (correo.size() > 254 || correo.section('@',0,0).size() > 64 || !formatoCorreo.match(correo).hasMatch())
        return "Introduce un correo válido, por ejemplo: nombre@dominio.com (máximo 254 caracteres).";
    for (const auto &parte : correo.section('@',1).split('.'))
        if (parte.size()>63) return "Cada parte del dominio del correo admite hasta 63 caracteres.";
    return {};
}

QString Validaciones::perfil(const DatosRegistro &d) {
    const QRegularExpression letras(QStringLiteral("^[\\p{L}\\p{M} ]+$"));
    const QStringList valores{d.nombre, d.paterno, d.materno};
    const QStringList etiquetas{"Nombre(s)", "Apellido paterno", "Apellido materno"};
    for (int i = 0; i < valores.size(); ++i) {
        const QString valor = valores[i].normalized(QString::NormalizationForm_C).trimmed();
        if (valor.isEmpty()) return etiquetas[i] + ": este campo es obligatorio.";
        if (valores[i].size() > 40 || !letras.match(valor).hasMatch()
            || !QRegularExpression("\\p{L}").match(valor).hasMatch())
            return etiquetas[i] + ": utiliza únicamente letras y espacios, sin exceder " + QString::number(40) + " caracteres.";
    }
    const QString errorCorreo = Validaciones::correo(d.correo);
    if (!errorCorreo.isEmpty()) return errorCorreo;
    if (!d.nacimiento.isValid() || d.nacimiento > QDate::currentDate()) return "Introduce una fecha de nacimiento válida.";
    if (d.nacimiento.addYears(18) > QDate::currentDate()) return "Debes tener al menos 18 años cumplidos.";
    return {};
}
QString Validaciones::clave(const QString &valor) {
    const auto caracteres = valor.toUcs4();
    bool mayuscula=false, minuscula=false, numero=false, especial=false;
    for (auto c : caracteres) {
        if (QChar::isSpace(c) || !QChar::isPrint(c)) return "La contraseña no debe contener espacios ni caracteres de control.";
        mayuscula |= QChar::isUpper(c); minuscula |= QChar::isLower(c);
        numero |= QChar::isDigit(c); especial |= !QChar::isLetterOrNumber(c) && !QChar::isMark(c);
    }
    if (caracteres.size()<8 || caracteres.size()>15 || !mayuscula || !minuscula || !numero || !especial)
        return "La contraseña debe tener entre 8 y 15 caracteres, con mayúscula, minúscula, número y carácter especial.";
    return {};
}
QString Validaciones::registro(const DatosRegistro &d) {
    const QStringList nombres{d.nombre, d.paterno, d.materno};
    const QStringList etiquetas{"Nombre(s)", "Apellido paterno", "Apellido materno"};
    for (int i=0; i<nombres.size(); ++i)
        if (nombres[i].size()>40)
            return etiquetas[i] + ": máximo 40 caracteres.";
    QString error = perfil(d);
    if (!error.isEmpty()) return error;
    error = clave(d.clave);
    if (!error.isEmpty()) return error;
    if (d.clave != d.confirmacion) return "Las contraseñas no coinciden.";
    if (!d.lecturaCompleta || !d.aceptaTerminos) return "Lee los términos hasta el final y marca la casilla de aceptación.";
    return {};
}
QString Claves::proteger(const QString &clave) {
    QByteArray sal(16, '\0');
    for (int i=0; i<sal.size(); ++i) sal[i] = char(QRandomGenerator::system()->generate() & 255);
    const auto hash = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256, clave.toUtf8(), sal, iteraciones, 32);
    if (hash.size()!=32) return {};
    return "pbkdf2-sha256$" + QString::number(iteraciones) + "$" + QString::fromLatin1(sal.toBase64()) + "$" + QString::fromLatin1(hash.toBase64());
}
bool Claves::verificar(const QString &clave, const QString &almacenado) {
    const auto partes = almacenado.split('$');
    if (clave.toUcs4().size()>15 || partes.size()!=4 || partes[0]!="pbkdf2-sha256" || partes[1]!="600000") return false;
    const auto sal = QByteArray::fromBase64(partes[2].toLatin1(), QByteArray::AbortOnBase64DecodingErrors);
    const auto esperado = QByteArray::fromBase64(partes[3].toLatin1(), QByteArray::AbortOnBase64DecodingErrors);
    if (sal.size()!=16 || esperado.size()!=32 || QString::fromLatin1(sal.toBase64())!=partes[2]
        || QString::fromLatin1(esperado.toBase64())!=partes[3]) return false;
    const auto obtenido = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256, clave.toUtf8(), sal, iteraciones, 32);
    if (obtenido.size()!=esperado.size()) return false;
    // Recorre todos los bytes, sin terminar en el primer byte diferente.
    volatile unsigned diferencia = 0;
    for (int i=0; i<esperado.size(); ++i) diferencia = diferencia | unsigned(uchar(esperado[i]) ^ uchar(obtenido[i]));
    return diferencia == 0;
}

ResultadoAcceso ServicioAcceso::registrar(const DatosRegistro &d) {
    const auto error = Validaciones::registro(d);
    if (!error.isEmpty()) return fallo(error);
    const QString hash = Claves::proteger(d.clave);
    if (hash.isEmpty()) return fallo("No se pudo proteger la contraseña.");
    Transaccion tx(bd);
    if (!tx.disponible()) return fallo(errorSql);
    QSqlQuery q(bd);
    // Serializa altas incluso entre dos procesos de la aplicación sin añadir UNIQUE al esquema.
    q.prepare("SELECT GET_LOCK(CONCAT(DATABASE(), ':registro'), 5)");
    if (!q.exec() || !q.next() || q.value(0).toInt()!=1) return fallo("Hay otro registro en curso. Inténtalo de nuevo.");
    struct Liberar { QSqlDatabase bd; ~Liberar() { QSqlQuery q(bd); q.exec("DO RELEASE_LOCK(CONCAT(DATABASE(), ':registro'))"); } } bloqueo{bd};
    q.prepare("SELECT id FROM usuario WHERE LOWER(TRIM(correo))=:correo FOR UPDATE");
    q.bindValue(":correo", correoNormal(d.correo));
    if (!q.exec()) return fallo(errorSql);
    if (q.next()) return fallo("Ese correo ya está registrado. Utiliza otro o inicia sesión.");
    q.finish();
    q.prepare("INSERT INTO usuario(nombre,apellido_paterno,apellido_materno,correo,nacimiento,password_hash,creado_en,ultimo_acceso,terminos_version,terminos_aceptados_en) VALUES(:nombre,:paterno,:materno,:correo,:nacimiento,:hash,:ahora,:acceso,:version,:aceptado)");
    Sesion s;
    s.nombre=d.nombre.normalized(QString::NormalizationForm_C).simplified();
    s.paterno=d.paterno.normalized(QString::NormalizationForm_C).simplified();
    s.materno=d.materno.normalized(QString::NormalizationForm_C).simplified();
    s.correo=correoNormal(d.correo); s.nacimiento=d.nacimiento; s.rol=Rol::Cliente;
    q.bindValue(":nombre",s.nombre); q.bindValue(":paterno",s.paterno); q.bindValue(":materno",s.materno);
    q.bindValue(":correo",s.correo); q.bindValue(":nacimiento",s.nacimiento.toString(Qt::ISODate)); q.bindValue(":hash",hash);
    const auto ahora=fechaAhora();
    q.bindValue(":ahora",ahora); q.bindValue(":acceso",ahora); q.bindValue(":version",VERSION_TERMINOS); q.bindValue(":aceptado",ahora);
    if (!q.exec()) return fallo(errorSql);
    s.usuarioId=q.lastInsertId().toInt();
    q.prepare("INSERT INTO cliente(usuario_id) VALUES(:id)"); q.bindValue(":id",s.usuarioId);
    if (!q.exec() || !tx.confirmar()) return fallo(errorSql);
    ResultadoAcceso resultado; resultado.sesion=s; return resultado;
}

ResultadoAcceso ServicioAcceso::ingresar(const QString &correo, const QString &clave, const QString &folio) {
    if (correo.trimmed().isEmpty() || clave.isEmpty()) return fallo("Introduce tu correo y contraseña.");
    Transaccion tx(bd);
    if (!tx.disponible()) return fallo(errorSql);
    QSqlQuery q(bd);
    q.prepare("SELECT u.id,u.nombre,u.apellido_paterno,u.apellido_materno,u.correo,u.nacimiento,u.password_hash,c.usuario_id,a.usuario_id,a.eliminado_en,a.folio,u.foto_ruta FROM usuario u LEFT JOIN cliente c ON c.usuario_id=u.id LEFT JOIN administrador a ON a.usuario_id=u.id WHERE LOWER(TRIM(u.correo))=:correo FOR UPDATE");
    q.bindValue(":correo",correoNormal(correo));
    if (!q.exec()) return fallo(errorSql);
    if (!q.next()) return fallo("Correo o contraseña incorrectos.");
    Sesion s;
    s.usuarioId=q.value(0).toInt(); s.nombre=q.value(1).toString(); s.paterno=q.value(2).toString(); s.materno=q.value(3).toString();
    s.correo=q.value(4).toString(); s.nacimiento=q.value(5).toDate();
    s.fotoRuta=q.value(11).toString();
    const QString hash=q.value(6).toString(), folioEsperado=q.value(10).toString();
    const bool cliente=!q.value(7).isNull(), admin=!q.value(8).isNull(), eliminado=!q.value(9).isNull();
    if (q.next()) return fallo("La cuenta tiene datos duplicados. Revisa la base de datos.");
    q.finish();
    if (!Claves::verificar(clave,hash)) return fallo("Correo o contraseña incorrectos.");
    if (cliente==admin || eliminado) return fallo("Esta cuenta no tiene un rol disponible para iniciar sesión.");
    s.rol=admin?Rol::Administrador:Rol::Cliente;
    if (admin) {
        if (folio.isEmpty()) { ResultadoAcceso r; r.requiereFolio=true; return r; }
        if (folio.trimmed().toUpper()!=folioEsperado) return fallo("El folio no corresponde a esta cuenta administrativa.");
    } else {
        q.prepare("SELECT motivo,creada_en FROM deshabilitacion WHERE cliente_id=:id AND reactivada_en IS NULL FOR UPDATE");
        q.bindValue(":id",s.usuarioId);
        if (!q.exec()) return fallo(errorSql);
        if (q.next()) { s.restringida=true; s.motivo=q.value(0).toString(); s.deshabilitadaEn=q.value(1).toDateTime(); }
        q.finish();
    }
    if (!s.restringida) {
        q.prepare("UPDATE usuario SET ultimo_acceso=:ahora WHERE id=:id");
        q.bindValue(":ahora",fechaAhora()); q.bindValue(":id",s.usuarioId);
        if (!q.exec()) return fallo(errorSql);
    }
    if (!tx.confirmar()) return fallo(errorSql);
    ResultadoAcceso r; r.sesion=s; return r;
}
bool ServicioAcceso::autorizarFolio(const Sesion &s, const QString &folio) {
    if (!s.valida() || s.rol!=Rol::Administrador || s.restringida) return false;
    QSqlQuery q(bd);
    q.prepare("SELECT folio FROM administrador WHERE usuario_id=:id AND eliminado_en IS NULL AND NOT EXISTS (SELECT 1 FROM cliente WHERE usuario_id=:cliente)");
    q.bindValue(":id",s.usuarioId); q.bindValue(":cliente",s.usuarioId);
    return q.exec() && q.next() && q.value(0).toString()==folio.trimmed().toUpper();
}
