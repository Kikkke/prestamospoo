#include "PantallaLogin.h"
#include "CampoContrasena.h"

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

PantallaLogin::PantallaLogin(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(14);
    layout->addStretch();
    auto *titulo = new QLabel(tr("Bienvenido de nuevo"), this);
    titulo->setObjectName("title");
    titulo->setWordWrap(true);
    layout->addWidget(titulo);
    auto *subtitulo = new QLabel(tr("Explora y compara tus opciones de préstamo."), this);
    subtitulo->setObjectName("muted");
    subtitulo->setWordWrap(true);
    layout->addWidget(subtitulo);

    auto *correo = new QLineEdit(this);
    auto *contrasena = new QLineEdit(this);
    correo->setPlaceholderText("tu@correo.com");
    contrasena->setPlaceholderText(tr("Tu contraseña"));
    agregarVisibilidad(contrasena);

    auto *formulario = new QFormLayout;
    formulario->setRowWrapPolicy(QFormLayout::WrapAllRows);
    formulario->setSpacing(12);
    formulario->addRow(tr("&Correo electrónico:"), correo);
    formulario->addRow(tr("C&ontraseña:"), contrasena);
    layout->addLayout(formulario);

    // Navegación del prototipo; todavía no autentica usuarios.
    auto *iniciarSesion = new QPushButton(tr("Iniciar sesión"), this);
    iniciarSesion->setObjectName("primary");
    connect(iniciarSesion, &QPushButton::clicked, this, &PantallaLogin::inicioSolicitado);
    layout->addWidget(iniciarSesion);

    layout->addWidget(new QLabel(tr("¿No tienes una cuenta?"), this));
    auto *crearCuenta = new QPushButton(tr("Crear cuenta"), this);
    crearCuenta->setObjectName("link");
    layout->addWidget(crearCuenta);
    connect(crearCuenta, &QPushButton::clicked,
            this, &PantallaLogin::crearCuentaSolicitada);

    auto *aviso = new QLabel(tr("Acceso de demostración. No necesitas introducir datos reales.\n:)"), this);
    aviso->setWordWrap(true);
    aviso->setObjectName("muted");
    layout->addWidget(aviso);
    auto *adminDemo = new QPushButton("Demo · Administrador", this);
    adminDemo->setObjectName("link");
    layout->addWidget(adminDemo);
    connect(adminDemo, &QPushButton::clicked, this, &PantallaLogin::administradorSolicitado);
    auto *clienteDemo = new QPushButton("Demo · Cuenta deshabilitada", this);
    clienteDemo->setObjectName("link");
    layout->addWidget(clienteDemo);
    connect(clienteDemo, &QPushButton::clicked, this, &PantallaLogin::clienteDeshabilitadoSolicitado);
    layout->addStretch();
}
