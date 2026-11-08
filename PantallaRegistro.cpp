#include "PantallaRegistro.h"

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

PantallaRegistro::PantallaRegistro(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->addStretch();
    auto *titulo = new QLabel(tr("Crea tu cuenta."), this);
    titulo->setObjectName("title");
    titulo->setWordWrap(true);
    layout->addWidget(titulo);
    auto *subtitulo = new QLabel(tr("Tus prestamos estan en las manos correctas"), this);
    subtitulo->setObjectName("muted");
    subtitulo->setWordWrap(true);
    layout->addWidget(subtitulo);

    auto *nombre = new QLineEdit(this);
    auto *correo = new QLineEdit(this);
    auto *contrasena = new QLineEdit(this);
    auto *confirmacion = new QLineEdit(this);
    nombre->setPlaceholderText(tr("Tu nombre"));
    correo->setPlaceholderText("tu@correo.com");
    contrasena->setPlaceholderText(tr("Crea una contraseña"));
    confirmacion->setPlaceholderText(tr("Repite tu contraseña"));
    contrasena->setEchoMode(QLineEdit::Password);
    confirmacion->setEchoMode(QLineEdit::Password);

    auto *formulario = new QFormLayout;
    formulario->setRowWrapPolicy(QFormLayout::WrapAllRows);
    formulario->setSpacing(8);
    formulario->addRow(tr("&Nombre:"), nombre);
    formulario->addRow(tr("&Correo electrónico:"), correo);
    formulario->addRow(tr("C&ontraseña:"), contrasena);
    formulario->addRow(tr("Con&firmar contraseña:"), confirmacion);
    layout->addLayout(formulario);

    // El registro y la persistencia se implementarán después.
    auto *registrar = new QPushButton(tr("Crear cuenta"), this);
    registrar->setObjectName("primary");
    connect(registrar, &QPushButton::clicked, this, &PantallaRegistro::inicioSolicitado);
    layout->addWidget(registrar);

    auto *volver = new QPushButton(tr("Volver al inicio de sesión"), this);
    volver->setObjectName("link");
    layout->addWidget(volver);
    connect(volver, &QPushButton::clicked,
            this, &PantallaRegistro::volverSolicitado);

    auto *aviso = new QLabel(tr("Modo prototipo: Crear cuenta abre el inicio sin validar ni guardar los datos."), this);
    aviso->setWordWrap(true);
    aviso->setObjectName("muted");
    layout->addWidget(aviso);
    layout->addStretch();
}
