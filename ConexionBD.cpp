#include "ConexionBD.h"
#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QUuid>

ConexionBD::ConexionBD() : nombre(QUuid::createUuid().toString()) {}
ConexionBD::~ConexionBD() {
    if (conexion.isValid()) conexion.close();
    conexion = QSqlDatabase();
    QSqlDatabase::removeDatabase(nombre);
}
bool ConexionBD::abrir(QString &error) {
    error.clear();
    QString ruta = qEnvironmentVariable("FINANCIONANTI_CONFIG");
    if (ruta.isEmpty()) ruta = QCoreApplication::applicationDirPath() + "/conexion.local.ini";
    if (qEnvironmentVariableIsEmpty("FINANCIONANTI_CONFIG") && !QFileInfo::exists(ruta)) ruta = QStringLiteral(CONFIG_LOCAL);
    if (!QFileInfo::exists(ruta)) {
        error = "Falta conexion.local.ini con la configuración local de MySQL.";
        return false;
    }
    if (!QSqlDatabase::isDriverAvailable("QMYSQL")) {
        error = "No se encuentra el controlador QMYSQL o alguna de sus bibliotecas.";
        return false;
    }
    QSettings ajustes(ruta, QSettings::IniFormat);
    ajustes.beginGroup("mysql");
    if (!conexion.isValid()) conexion = QSqlDatabase::addDatabase("QMYSQL", nombre);
    conexion.setHostName(ajustes.value("host", "127.0.0.1").toString());
    conexion.setPort(ajustes.value("puerto", 3306).toInt());
    conexion.setDatabaseName(ajustes.value("base", "financionantimex").toString());
    conexion.setUserName(ajustes.value("usuario").toString());
    conexion.setConnectOptions("MYSQL_OPT_CONNECT_TIMEOUT=5");
    if (!conexion.open(conexion.userName(), ajustes.value("clave").toString())) {
        error = "No se pudo abrir financionantimex. Revisa el servicio MySQL, la base y las credenciales locales. Código: " + conexion.lastError().nativeErrorCode();
        return false;
    }
    QSqlQuery consulta(conexion);
    if (!consulta.exec("SELECT COUNT(*) FROM usuario") || !consulta.next()) {
        error = "La conexión abrió, pero no se pudo consultar la tabla usuario. Revisa el esquema.";
        conexion.close();
        return false;
    }
    return true;
}

