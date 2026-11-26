#include "PantallaLogin.h"
#include "CampoContrasena.h"
#include "TareaAcceso.h"
#include <QInputDialog>

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

    correo->setObjectName("correoLogin");
    contrasena->setObjectName("claveLogin");
    correo->setMaxLength(254);
    auto *error = new QLabel;
    error->setObjectName("passwordHint");
    error->setWordWrap(true);
    error->setTextFormat(Qt::PlainText);
    layout->addWidget(error);
    auto *iniciarSesion = new QPushButton(tr("Iniciar sesión"), this);
    iniciarSesion->setObjectName("primary");
    auto enviar = [=]() {
        const QString email=correo->text(), clave=contrasena->text();
        if(email.trimmed().isEmpty() || clave.isEmpty()) { error->setText("Introduce tu correo y contraseña."); return; }
        error->setText("Verificando credenciales…");
        ejecutarAcceso(this,[email,clave](ServicioAcceso &servicio){return servicio.ingresar(email,clave);},
            [=](ResultadoAcceso r) {
                if(r.requiereFolio) {
                    bool ok=false;
                    const QString folio=QInputDialog::getText(this,"Folio administrativo",
                        "Introduce el folio de tu cuenta para completar el acceso.",QLineEdit::Password,{},&ok);
                    if(!ok || folio.trimmed().isEmpty()) { contrasena->clear(); error->clear(); return; }
                    error->setText("Validando acceso administrativo…");
                    ejecutarAcceso(this,[email,clave,folio](ServicioAcceso &servicio){return servicio.ingresar(email,clave,folio);},
                        [=](ResultadoAcceso resultado) {
                            contrasena->clear();
                            if(!resultado.correcto()) { error->setText(resultado.error); return; }
                            limpiar(); emit accesoCompletado(resultado.sesion);
                        });
                } else {
                    contrasena->clear();
                    if(!r.correcto()) { error->setText(r.error); return; }
                    limpiar(); emit accesoCompletado(r.sesion);
                }
            });
    };
    connect(iniciarSesion,&QPushButton::clicked,this,enviar);
    connect(contrasena,&QLineEdit::returnPressed,this,enviar);
    connect(correo,&QLineEdit::returnPressed,this,enviar);
    layout->addWidget(iniciarSesion);

    layout->addWidget(new QLabel(tr("¿No tienes una cuenta?"), this));
    auto *crearCuenta = new QPushButton(tr("Crear cuenta"), this);
    crearCuenta->setObjectName("link");
    layout->addWidget(crearCuenta);
    connect(crearCuenta, &QPushButton::clicked,
            this, &PantallaLogin::crearCuentaSolicitada);

    layout->addWidget(new QLabel(":)",this));
    restablecer=[=]() {
        correo->clear(); contrasena->clear(); error->clear();
        for(auto *accion:contrasena->actions()) if(accion->isCheckable()) accion->setChecked(false);
    };
    layout->addStretch();
}
