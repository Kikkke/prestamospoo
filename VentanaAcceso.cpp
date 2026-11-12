#include "VentanaAcceso.h"
#include "PantallaLogin.h"
#include "PantallaRegistro.h"
#include "PanelPrincipal.h"
#include "PanelAdministrador.h"
#include "FondoAcceso.h"
#include "MarcaFinancionanti.h"

#include <QLabel>
#include <QFrame>
#include <QStackedWidget>
#include <QVBoxLayout>

VentanaAcceso::VentanaAcceso(QWidget *parent) : QWidget(parent)
{
    setWindowTitle(tr("FinancionantiMex — Simulador de préstamos"));
    setWindowIcon(iconoFinancionanti());
    resize(1180, 820);
    setMinimumSize(920, 700);

    auto *pantallas = new QStackedWidget(this);
    auto *acceso = new FondoAcceso;
    acceso->setObjectName("access");
    auto *fila = new QHBoxLayout(acceso);
    fila->setContentsMargins(48, 32, 48, 32);
    fila->setSpacing(44);
    auto *marca = new QFrame;
    marca->setObjectName("illustration");
    auto *presentacion = new QVBoxLayout(marca);
    presentacion->setContentsMargins(12, 24, 12, 24);
    auto *identidad = new QHBoxLayout;
    identidad->setSpacing(12);
    identidad->addWidget(logoFinancionanti(68));
    auto *nombre = new QLabel("FinancionantiMex");
    nombre->setObjectName("brandAccess");
    identidad->addWidget(nombre, 1);
    presentacion->addLayout(identidad);
    presentacion->addWidget(new IlustracionAcceso, 1);
    auto *mensaje = new QLabel("Una forma impresionanti\nde obtener un préstamo");
    mensaje->setObjectName("brandSlogan");
    mensaje->setWordWrap(true);
    presentacion->addWidget(mensaje);
    auto *detalle = new QLabel("Explora tus pagos y compara opciones con claridad.");
    detalle->setWordWrap(true);
    detalle->setObjectName("muted");
    presentacion->addWidget(detalle);
    presentacion->addSpacing(24);
    presentacion->addWidget(new QLabel("SIMULADOR DE PRÉSTAMOS  /  MXN"));
    fila->addWidget(marca, 1);
    auto *formularios = new QStackedWidget;
    formularios->setObjectName("authStack");
    auto *login = new PantallaLogin;
    auto *registro = new PantallaRegistro;
    formularios->addWidget(login);
    formularios->addWidget(registro);
    auto *tarjeta = new QFrame;
    tarjeta->setObjectName("authCard");
    tarjeta->setMaximumWidth(470);
    auto *tarjetaLayout = new QVBoxLayout(tarjeta);
    tarjetaLayout->setContentsMargins(28, 24, 28, 24);
    tarjetaLayout->addWidget(formularios);
    fila->addWidget(tarjeta, 1);
    auto *panel = new PanelPrincipal;
    pantallas->addWidget(acceso);
    pantallas->addWidget(panel);
    auto *admin = new PanelAdministrador;
    pantallas->addWidget(admin);
    admin->salir = [pantallas, acceso]() { pantallas->setCurrentWidget(acceso); };
    connect(login, &PantallaLogin::administradorSolicitado, this, [admin, pantallas]() {
        if (admin->confirmar("Acceso administrativo de demostración")) pantallas->setCurrentWidget(admin);
    });
    connect(login, &PantallaLogin::clienteDeshabilitadoSolicitado, this, [admin, pantallas, panel]() {
        if (admin->clientes[0].habilitado) pantallas->setCurrentWidget(panel);
        else admin->accesoCliente();
    });

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(pantallas);

    connect(login, &PantallaLogin::crearCuentaSolicitada, this,
            [formularios, registro]() { formularios->setCurrentWidget(registro); });
    connect(registro, &PantallaRegistro::volverSolicitado, this,
            [formularios, login]() { formularios->setCurrentWidget(login); });
    connect(login, &PantallaLogin::inicioSolicitado, this,
            [pantallas, panel]() { pantallas->setCurrentWidget(panel); });
    connect(registro, &PantallaRegistro::inicioSolicitado, this,
            [pantallas, panel]() { pantallas->setCurrentWidget(panel); });
    connect(panel, &PanelPrincipal::salirSolicitado, this,
            [pantallas, acceso, formularios, login]() {
                formularios->setCurrentWidget(login);
                pantallas->setCurrentWidget(acceso);
            });
}
