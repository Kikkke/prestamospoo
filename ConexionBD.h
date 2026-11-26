#pragma once
#include <QSqlDatabase>
#include <QString>

// Una conexión por instancia, utilizada únicamente desde el hilo principal.
class ConexionBD {
public:
    ConexionBD();
    ~ConexionBD();
    ConexionBD(const ConexionBD &) = delete;
    ConexionBD &operator=(const ConexionBD &) = delete;
    bool abrir(QString &error);
    QSqlDatabase base() const { return conexion; }
private:
    QString nombre;
    QSqlDatabase conexion;
};
