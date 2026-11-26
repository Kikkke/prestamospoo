#include "ConexionBD.h"
#include <QMessageBox>
#include <cstdio>
#include <QThreadPool>
#include <QApplication>
#include "VentanaAcceso.h"
#include "Tema.h"
#include "SelectorDesplegable.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("FinancionantiMex");
    app.setStyle(new EstiloSelectores);
    app.setStyleSheet(temaOscuro());
    ConexionBD base;
    QString error;
    const bool conectado = base.abrir(error);
    if (app.arguments().contains("--comprobar-bd")) {
        fprintf(conectado ? stdout : stderr, "%s\n", conectado ? "Conexion MySQL y lectura de usuario correctas." : error.toUtf8().constData());
        return conectado ? 0 : 1;
    }
    if (!conectado) {
        QMessageBox::critical(nullptr, "Conexión a MySQL", error);
        return 1;
    }
    VentanaAcceso ventana(base.base());
    ventana.show();
    const int resultado=app.exec();
    QThreadPool::globalInstance()->waitForDone();
    return resultado;
}
