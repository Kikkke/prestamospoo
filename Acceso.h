#pragma once
#include <QDate>
#include <QDateTime>
#include <QSqlDatabase>
#include <QString>
#include <QImage>

inline const QString VERSION_TERMINOS = QStringLiteral("1");

struct DatosRegistro {
    QString nombre, paterno, materno, correo, clave, confirmacion;
    QDate nacimiento;
    bool lecturaCompleta = false, aceptaTerminos = false;
};

enum class Rol { Ninguno, Cliente, Administrador };
struct Sesion {
    int usuarioId = 0;
    Rol rol = Rol::Ninguno;
    QString nombre, paterno, materno, correo, fotoRuta;
    QDate nacimiento;
    bool restringida = false;
    QString motivo;
    QDateTime deshabilitadaEn;
    bool valida() const { return usuarioId > 0 && rol != Rol::Ninguno; }
};
Q_DECLARE_METATYPE(Sesion)
struct ResultadoAcceso {
    Sesion sesion;
    QString error;
    bool requiereFolio = false;
    bool correcto() const { return error.isEmpty() && sesion.valida(); }
};

namespace Validaciones {
QString correo(const QString &valor);
QString perfil(const DatosRegistro &datos);
QString clave(const QString &valor);
QString registro(const DatosRegistro &datos);
}
namespace Claves {
QString proteger(const QString &clave);
bool verificar(const QString &clave, const QString &almacenado);
}

// No depende de widgets. Cada instancia se usa con la conexión de su propio hilo.
class ServicioAcceso {
public:
    explicit ServicioAcceso(QSqlDatabase base) : bd(base) {}
    ResultadoAcceso registrar(const DatosRegistro &datos);
    ResultadoAcceso ingresar(const QString &correo, const QString &clave,
                             const QString &folio = {});
    ResultadoAcceso actualizarPerfil(const Sesion &, const DatosRegistro &, const QString &actual);
    ResultadoAcceso guardarFoto(const Sesion &, const QImage &);
    bool autorizarFolio(const Sesion &sesion, const QString &folio);
private:
    QSqlDatabase bd;
};
