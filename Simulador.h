#pragma once
#include "Acceso.h"
#include <QVector>

struct EntradaPrestamo { QString nombre, monto, tasa, meses; };
// Dinero en centavos y tasa anual en centésimas de punto porcentual (12 % = 1200).
struct CondicionesPrestamo {
    QString nombre;
    qint64 monto=0;
    int tasa=0, meses=0;
    bool operator==(const CondicionesPrestamo &otra) const {
        return nombre==otra.nombre && monto==otra.monto && tasa==otra.tasa && meses==otra.meses;
    }
};
struct CuotaPrestamo { int numero; qint64 inicial, interes, capital, pago, final; };
struct CalculoPrestamo {
    CondicionesPrestamo condiciones;
    QVector<CuotaPrestamo> cuotas;
    qint64 regular=0, intereses=0, total=0;
    QString error, aviso;
    bool valido() const { return error.isEmpty() && !cuotas.isEmpty(); }
};
namespace MotorPrestamo {
QString validar(const CondicionesPrestamo &datos);
CalculoPrestamo calcular(const EntradaPrestamo &entrada);
CalculoPrestamo calcular(const CondicionesPrestamo &datos);
QString decimal(qint64 centavos); // Formato SQL exacto, sin separadores de miles.
QString dinero(qint64 centavos); // Formato de presentación es-MX.
}
struct GuardadoSimulacion {
    int id=0;
    QString error;
    bool correcto() const {return id>0 && error.isEmpty();}
};
class ServicioSimulaciones {
public:
    explicit ServicioSimulaciones(QSqlDatabase base):bd(base) {}
    GuardadoSimulacion guardar(const Sesion &sesion,const CondicionesPrestamo &datos);
private:
    QSqlDatabase bd;
};
