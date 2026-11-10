#include <QApplication>
#include "VentanaAcceso.h"
#include "Tema.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("FinancionantiMex");
    app.setStyle("Fusion");
    app.setStyleSheet(temaOscuro());
    VentanaAcceso ventana;
    ventana.show();
    return app.exec();
}
