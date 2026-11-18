#include "PantallaRegistro.h"
#include "CampoContrasena.h"
#include "CampoNacimiento.h"
#include "CamposNombre.h"
#include "TerminosRegistro.h"
#include <QCheckBox>
#include <QMessageBox>
#include <QScrollArea>

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

// Intercepta tanto el clic como la barra espaciadora antes de cambiar el estado.
class CasillaTerminos : public QCheckBox {
public:
    using QCheckBox::QCheckBox;
    bool lecturaCompletada=false;
protected:
    void nextCheckState() override {
        if(!lecturaCompletada){
            QMessageBox::warning(this,"Lee los términos y condiciones",
                "Antes de aceptar, debes leer los términos y condiciones. Pulsa «Leer términos y condiciones», desplázate hasta el final del documento y vuelve al registro.");
            return;
        }
        QCheckBox::nextCheckState();
    }
};

PantallaRegistro::PantallaRegistro(QWidget *parent) : QWidget(parent)
{
    auto *raiz = new QVBoxLayout(this);
    raiz->setContentsMargins(0,0,0,0);
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet("QScrollArea { background:transparent; border:none; } QScrollArea > QWidget > QWidget { background:transparent; }");
    auto *contenido = new QWidget;
    auto *layout = new QVBoxLayout(contenido);
    layout->setContentsMargins(8,8,12,8);
    layout->setSizeConstraint(QLayout::SetMinimumSize);
    scroll->setWidget(contenido);
    raiz->addWidget(scroll);
    layout->setSpacing(10);
    layout->addStretch();
    auto *titulo = new QLabel(tr("Crea tu cuenta"), this);
    titulo->setObjectName("title");
    titulo->setWordWrap(true);
    layout->addWidget(titulo);
    auto *nombre = new QLineEdit(this);
    auto *paterno = new QLineEdit(this);
    auto *materno = new QLineEdit(this);
    auto *correo = new QLineEdit(this);
    auto *contrasena = new QLineEdit(this);
    auto *confirmacion = new QLineEdit(this);
    nombre->setPlaceholderText(tr("Tu nombre o nombres"));
    correo->setPlaceholderText("tu@correo.com");
    contrasena->setPlaceholderText(tr("Crea una contraseña"));
    confirmacion->setPlaceholderText(tr("Repite tu contraseña"));
    agregarVisibilidad(contrasena);
    agregarVisibilidad(confirmacion);

    auto *nacimiento = crearCampoNacimiento(this);
    auto *formulario = new QFormLayout;
    formulario->setRowWrapPolicy(QFormLayout::WrapAllRows);
    formulario->setSpacing(8);
    formulario->addRow(tr("&Nombre(s):"), nombre);
    agregarApellidos(formulario, paterno, materno);
    formulario->addRow(tr("&Correo electrónico:"), correo);
    formulario->addRow(tr("Fecha de &nacimiento:"), nacimiento);
    formulario->addRow(tr("C&ontraseña:"), contrasena);
    formulario->addRow(tr("Con&firmar contraseña:"), confirmacion);
    layout->addLayout(formulario);

    auto *coincidencia = new QLabel(this);
    coincidencia->setObjectName("passwordHint");
    coincidencia->setWordWrap(true);
    coincidencia->setMinimumHeight(18);
    layout->addWidget(coincidencia);
    auto revisar = [contrasena, confirmacion, coincidencia]() {
        coincidencia->setText(confirmacion->text().isEmpty() ? QString()
            : contrasena->text() == confirmacion->text() ? "Las contraseñas coinciden."
            : "Las contraseñas no coinciden. Revisa ambos campos.");
    };
    connect(contrasena, &QLineEdit::textChanged, this, revisar);
    connect(confirmacion, &QLineEdit::textChanged, this, revisar);

    auto *verTerminos=new QPushButton("Leer términos y condiciones",this);
    verTerminos->setObjectName("link");layout->addWidget(verTerminos);
    auto *aceptar=new CasillaTerminos("He leído y acepto los términos y condiciones",this);
    aceptar->setStyleSheet("QCheckBox { spacing:10px; background:transparent; } QCheckBox::indicator { width:20px; height:20px; border:2px solid #ed916f; border-radius:4px; background:#1b1b20; } QCheckBox::indicator:disabled { border-color:#aaa0a0; background:#242124; } QCheckBox::indicator:checked { image:url(:/controles/aceptado.xpm); background:#ed916f; } QCheckBox:disabled { color:#aaa0a0; } ");
    aceptar->setText("He leído y acepto los términos\ny condiciones");
    layout->addWidget(aceptar);
    aceptar->setToolTip("Abre los términos y desplázate hasta el final para habilitar esta casilla.");
    connect(verTerminos,&QPushButton::clicked,this,[=](){if(mostrarTerminos(this))aceptar->lecturaCompletada=true;});

    // El registro y la persistencia se implementarán después.
    auto *registrar = new QPushButton(tr("Crear cuenta"), this);
    registrar->setObjectName("primary");
    registrar->setEnabled(false);
    connect(aceptar,&QCheckBox::toggled,registrar,&QPushButton::setEnabled);
    connect(registrar, &QPushButton::clicked, this, [=]() {
        if (!aceptar->lecturaCompletada || !aceptar->isChecked()) return;
        if (contrasena->text() != confirmacion->text()) {
            coincidencia->setText("Las contraseñas no coinciden. Revisa ambos campos.");
            confirmacion->setFocus();
            return;
        }
        if (!nombre->text().trimmed().isEmpty())
            emit perfilDemoCreado(nombre->text().trimmed(), paterno->text().trimmed(),
                                 materno->text().trimmed(), correo->text().trimmed(), nacimiento->date());
        aceptar->setChecked(false);aceptar->lecturaCompletada=false;
        emit inicioSolicitado();
    });
    layout->addWidget(registrar);

    auto *volver = new QPushButton(tr("Ya tengo una cuenta"), this);
    volver->setObjectName("link");
    layout->addWidget(volver);
    connect(volver, &QPushButton::clicked,
            this, &PantallaRegistro::volverSolicitado);

    layout->addStretch();
}
