#include "Acceso.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDir>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>
#include <QUuid>

namespace {
ResultadoAcceso errorPerfil(const QString &mensaje) { ResultadoAcceso r; r.error=mensaje; return r; }
const QString errorBD="No se pudieron guardar los cambios. Revisa la conexión a MySQL.";
struct OperacionPerfil {
    QSqlDatabase bd;
    bool abierta=false, bloqueo=false;
    explicit OperacionPerfil(QSqlDatabase b):bd(b) {
        QSqlQuery q(bd);
        bloqueo=q.exec("SELECT GET_LOCK(CONCAT(DATABASE(), ':registro'),5)") && q.next() && q.value(0).toInt()==1;
        if(bloqueo) abierta=bd.transaction();
    }
    ~OperacionPerfil() {
        if(abierta) bd.rollback();
        if(bloqueo) {QSqlQuery q(bd);q.exec("DO RELEASE_LOCK(CONCAT(DATABASE(), ':registro'))");}
    }
    bool commit() {if(!abierta || !bd.commit())return false;abierta=false;return true;}
};
// Revalida el rol y el estado; nunca toma un ID de un campo editable.
bool leerPropio(QSqlDatabase bd,const Sesion &s,QSqlQuery &q) {
    if(!s.valida() || s.restringida)return false;
    q.prepare("SELECT u.nombre,u.apellido_paterno,u.apellido_materno,u.correo,u.nacimiento,u.password_hash,u.foto_ruta FROM usuario u WHERE u.id=:id AND ((:rol=1 AND EXISTS(SELECT 1 FROM cliente WHERE usuario_id=u.id) AND NOT EXISTS(SELECT 1 FROM administrador WHERE usuario_id=u.id) AND NOT EXISTS(SELECT 1 FROM deshabilitacion WHERE cliente_id=u.id AND reactivada_en IS NULL)) OR (:admin=2 AND EXISTS(SELECT 1 FROM administrador WHERE usuario_id=u.id AND eliminado_en IS NULL) AND NOT EXISTS(SELECT 1 FROM cliente WHERE usuario_id=u.id))) FOR UPDATE");
    q.bindValue(":id",s.usuarioId);q.bindValue(":rol",int(s.rol));q.bindValue(":admin",int(s.rol));
    return q.exec() && q.next();
}
bool auditar(QSqlDatabase bd,const Sesion &s,const QString &accion) {
    if(s.rol!=Rol::Administrador)return true;
    QSqlQuery q(bd);
    q.prepare("INSERT INTO actividad_administrativa(administrador_id,accion,entidad,entidad_id,descripcion,creada_en) VALUES(:id,:accion,'usuario',:usuario,'Cambio del perfil propio',NOW())");
    q.bindValue(":id",s.usuarioId);q.bindValue(":usuario",s.usuarioId);q.bindValue(":accion",accion);return q.exec();
}
QString carpetaFotos() {return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/fotos";}
void retirarFoto(const QString &ruta) {
    const QFileInfo f(ruta);
    if(!ruta.isEmpty() && !f.isSymLink() && f.canonicalPath()==QFileInfo(carpetaFotos()).canonicalFilePath()
        && f.fileName().startsWith("perfil-") && f.suffix()=="png") QFile::remove(ruta);
}
}

ResultadoAcceso ServicioAcceso::actualizarPerfil(const Sesion &s,const DatosRegistro &d,const QString &actual) {
    QString error=Validaciones::perfil(d);
    const bool cambiaClave=!d.clave.isEmpty() || !d.confirmacion.isEmpty();
    if(error.isEmpty() && cambiaClave) {
        error=Validaciones::clave(d.clave);
        if(error.isEmpty() && d.clave!=d.confirmacion)error="Las contraseñas no coinciden.";
    }
    if(!error.isEmpty())return errorPerfil(error);
    OperacionPerfil tx(bd);if(!tx.abierta)return errorPerfil(errorBD);
    QSqlQuery q(bd);if(!leerPropio(bd,s,q))return errorPerfil("No se pudo autorizar tu sesión. Vuelve a iniciar sesión.");
    Sesion nueva=s;
    nueva.nombre=d.nombre.normalized(QString::NormalizationForm_C).simplified();
    nueva.paterno=d.paterno.normalized(QString::NormalizationForm_C).simplified();
    nueva.materno=d.materno.normalized(QString::NormalizationForm_C).simplified();
    nueva.correo=d.correo.trimmed().toLower();nueva.nacimiento=d.nacimiento;nueva.fotoRuta=q.value(6).toString();
    QString hash=q.value(5).toString();
    const bool cambiaDatos=nueva.nombre!=q.value(0).toString() || nueva.paterno!=q.value(1).toString() || nueva.materno!=q.value(2).toString() || nueva.correo!=q.value(3).toString().trimmed().toLower() || nueva.nacimiento!=q.value(4).toDate();
    q.finish();
    if(cambiaClave) {
        if(!Claves::verificar(actual,hash))return errorPerfil("La contraseña actual no es correcta.");
        if(Claves::verificar(d.clave,hash))return errorPerfil("La nueva contraseña debe ser diferente de la actual.");
        hash=Claves::proteger(d.clave);if(hash.isEmpty())return errorPerfil(errorBD);
    }
    if(!cambiaDatos && !cambiaClave)return errorPerfil("No hay cambios para guardar.");
    q.prepare("SELECT id FROM usuario WHERE LOWER(TRIM(correo))=:correo AND id<>:id FOR UPDATE");
    q.bindValue(":correo",nueva.correo);q.bindValue(":id",s.usuarioId);
    if(!q.exec())return errorPerfil(errorBD);
    if(q.next())return errorPerfil("Ese correo ya está registrado en otra cuenta.");q.finish();
    q.prepare("UPDATE usuario SET nombre=:n,apellido_paterno=:p,apellido_materno=:m,correo=:c,nacimiento=:f,password_hash=:h WHERE id=:id");
    q.bindValue(":n",nueva.nombre);q.bindValue(":p",nueva.paterno);q.bindValue(":m",nueva.materno);q.bindValue(":c",nueva.correo);q.bindValue(":f",nueva.nacimiento);q.bindValue(":h",hash);q.bindValue(":id",s.usuarioId);
    if(!q.exec() || !auditar(bd,s,cambiaClave?"PERFIL_Y_CONTRASENA":"PERFIL") || !tx.commit())return errorPerfil(errorBD);
    ResultadoAcceso r;r.sesion=nueva;return r;
}

ResultadoAcceso ServicioAcceso::guardarFoto(const Sesion &s,const QImage &foto) {
    if(!foto.isNull() && foto.size()!=QSize(512,512))return errorPerfil("El recorte debe medir 512 × 512 píxeles.");
    OperacionPerfil tx(bd);if(!tx.abierta)return errorPerfil(errorBD);
    QSqlQuery q(bd);if(!leerPropio(bd,s,q))return errorPerfil("No se pudo autorizar tu sesión.");
    const QString anterior=q.value(6).toString();q.finish();
    if((foto.isNull() && anterior.isEmpty()) || (!foto.isNull() && !anterior.isEmpty() && foto.convertToFormat(QImage::Format_ARGB32)==QImage(anterior).convertToFormat(QImage::Format_ARGB32)))return errorPerfil("No hay cambios en la foto para guardar.");
    QString ruta;
    if(!foto.isNull()) {
        if(!QDir().mkpath(carpetaFotos()))return errorPerfil("No se pudo crear la carpeta de fotos.");
        ruta=carpetaFotos()+"/perfil-"+QUuid::createUuid().toString(QUuid::WithoutBraces)+".png";
        QSaveFile archivo(ruta);
        if(!archivo.open(QIODevice::WriteOnly) || !foto.save(&archivo,"PNG") || !archivo.commit())return errorPerfil("No se pudo guardar la imagen.");
    }
    q.prepare("UPDATE usuario SET foto_ruta=:ruta WHERE id=:id");
    q.bindValue(":ruta",ruta.isEmpty()?QVariant():QVariant(ruta));q.bindValue(":id",s.usuarioId);
    if(!q.exec() || !auditar(bd,s,"FOTO_PERFIL") || !tx.commit()) {retirarFoto(ruta);return errorPerfil(errorBD);}
    retirarFoto(anterior);ResultadoAcceso r;r.sesion=s;r.sesion.fotoRuta=ruta;return r;
}
