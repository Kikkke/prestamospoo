#include "ServicioHistorial.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>

namespace {
ResultadoHistorial fallo(const QString &texto,bool denegado=false) {
    ResultadoHistorial r;r.error=texto;r.accesoDenegado=denegado;return r;
}
const QString errorConexion="No se pudo completar la operación del historial. Revisa la conexión a MySQL y pulsa Actualizar.";
}
ResultadoHistorial ServicioHistorial::listar(const Sesion &s) {return ejecutar(s,0);}
ResultadoHistorial ServicioHistorial::eliminar(const Sesion &s,int id) {
    if(id<=0)return fallo("La simulación no está disponible para eliminar.");
    return ejecutar(s,id);
}
ResultadoHistorial ServicioHistorial::ejecutar(const Sesion &s,int eliminarId) {
    if(!s.valida() || s.rol!=Rol::Cliente || s.restringida)
        return fallo("Solo un cliente habilitado puede consultar o modificar su historial.",true);
    if(!bd.transaction())return fallo(errorConexion);
    struct Transaccion {QSqlDatabase bd;bool activa=true;~Transaccion(){if(activa)bd.rollback();}} tx{bd};
    QSqlQuery q(bd);
    q.prepare("SELECT u.id FROM usuario u JOIN cliente c ON c.usuario_id=u.id WHERE u.id=:id AND NOT EXISTS(SELECT 1 FROM administrador WHERE usuario_id=u.id) FOR UPDATE");
    q.bindValue(":id",s.usuarioId);
    if(!q.exec())return fallo(errorConexion);
    if(!q.next())return fallo("Tu cuenta no tiene acceso al historial. Vuelve a iniciar sesión.",true);
    q.finish();
    q.prepare("SELECT id FROM deshabilitacion WHERE cliente_id=:id AND reactivada_en IS NULL FOR UPDATE");
    q.bindValue(":id",s.usuarioId);
    if(!q.exec())return fallo(errorConexion);
    if(q.next())return fallo("Tu cuenta está deshabilitada. No puedes consultar ni modificar el historial.",true);
    q.finish();
    if(eliminarId>0) {
        // El ID solicitado nunca basta: propietario y estado se verifican en el UPDATE.
        q.prepare("UPDATE simulacion SET eliminada_en=NOW(),actualizada_en=NOW() WHERE id=:simulacion AND cliente_id=:cliente AND eliminada_en IS NULL");
        q.bindValue(":simulacion",eliminarId);q.bindValue(":cliente",s.usuarioId);
        if(!q.exec())return fallo(errorConexion);
        if(q.numRowsAffected()!=1)return fallo("La simulación ya no está disponible en tu historial. Pulsa Actualizar.");
        q.finish();
    }
    q.prepare("SELECT id,nombre,CAST(monto*100 AS SIGNED),CAST(tasa_anual*10000 AS SIGNED),meses FROM simulacion WHERE cliente_id=:cliente AND eliminada_en IS NULL ORDER BY creada_en DESC,id DESC");
    q.bindValue(":cliente",s.usuarioId);
    if(!q.exec())return fallo(errorConexion);
    ResultadoHistorial r;
    while(q.next())r.registros.append({q.value(0).toInt(),q.value(1).toString(),q.value(2).toLongLong(),q.value(3).toInt(),q.value(4).toInt()});
    if(q.lastError().isValid())return fallo(errorConexion);
    q.finish();
    if(!bd.commit())return fallo("MySQL no confirmó la operación. Pulsa Actualizar para comprobar el historial antes de reintentar.");
    tx.activa=false;r.eliminadoId=eliminarId;return r;
}
