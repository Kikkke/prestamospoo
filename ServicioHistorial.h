#pragma once
#include "Acceso.h"
#include <QVector>

struct RegistroHistorial {
    int id=0;
    QString nombre;
    qint64 montoCentavos=0;
    int tasaDiezmilesimas=0, meses=0;
};
struct ResultadoHistorial {
    QVector<RegistroHistorial> registros;
    QString error;
    bool accesoDenegado=false;
    int eliminadoId=0;
    bool correcto() const {return error.isEmpty();}
};

// Cada instancia utiliza la conexión del hilo que ejecuta la operación.
class ServicioHistorial {
public:
    explicit ServicioHistorial(QSqlDatabase base):bd(base) {}
    ResultadoHistorial listar(const Sesion &sesion);
    ResultadoHistorial eliminar(const Sesion &sesion,int simulacionId);
private:
    ResultadoHistorial ejecutar(const Sesion &sesion,int eliminarId);
    QSqlDatabase bd;
};
