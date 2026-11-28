#pragma once
#include "Simulador.h"

struct DetalleAmortizacion {
    int id=0, tasaDiezmilesimas=0, meses=0;
    QString nombre;
    QDateTime creada,actualizada;
    qint64 monto=0,intereses=0,total=0;
    QVector<CuotaPrestamo> cuotas;
};
struct ResultadoDetalle {
    DetalleAmortizacion detalle;
    QString error;
    bool correcto() const {return error.isEmpty() && detalle.id>0 && !detalle.cuotas.isEmpty();}
};
struct ResultadoPdf {
    ResultadoDetalle consulta;
    QString error;
    int paginas=0;
    bool cancelado=false;
    bool correcto() const {return !cancelado && error.isEmpty() && paginas>0;}
};
class ServicioAmortizacion {
public:
    explicit ServicioAmortizacion(QSqlDatabase base):bd(base) {}
    ResultadoDetalle consultar(const Sesion &sesion,int id);
    ResultadoPdf exportar(const Sesion &sesion,int id,const QString &ruta);
private:
    // Se llama con transacción abierta; todos los datos proceden de consultas bloqueantes.
    ResultadoDetalle leer(const Sesion &sesion,int id);
    QSqlDatabase bd;
};
