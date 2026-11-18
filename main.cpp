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
    VentanaAcceso ventana;
    ventana.show();
    return app.exec();
}
