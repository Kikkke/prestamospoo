#pragma once
#include "CampoNacimiento.h"
#include "Acceso.h"
#include "TareaAcceso.h"
#include <QRegularExpressionValidator>
#include "CamposNombre.h"
#include "CampoContrasena.h"
#include <QDialog>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <functional>
#include <QScrollArea>
#include <QScreen>
inline bool editarPerfil(QWidget *padre,QString &nombre,QString &paterno,QString &materno,QString &correo,QDate &nacimiento,
                         const std::function<bool(const QString &,const QString &)> &autorizar, Sesion *sesion=nullptr) {
    DialogoGuardado d(padre);d.setWindowTitle("Editar datos");d.setMinimumWidth(460);
    auto *raiz=new QVBoxLayout(&d);raiz->setContentsMargins(0,0,0,0);
    auto *scroll=new QScrollArea;scroll->setWidgetResizable(true);scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    auto *contenido=new QWidget;scroll->setWidget(contenido);raiz->addWidget(scroll);
    d.resize(500,qMin(780,d.screen()->availableGeometry().height()-80));
    auto *l=new QVBoxLayout(contenido);l->setContentsMargins(24,24,24,24);l->setSpacing(14);
    auto *titulo=new QLabel("Datos de la cuenta");titulo->setObjectName("title");l->addWidget(titulo);
    auto *f=new QFormLayout;f->setRowWrapPolicy(QFormLayout::WrapAllRows);f->setSpacing(10);l->addLayout(f);
    auto *n=new QLineEdit(nombre);auto *ap=new QLineEdit(paterno);auto *am=new QLineEdit(materno);auto *c=new QLineEdit(correo);auto *fecha=crearCampoNacimiento();fecha->setDate(nacimiento);
    n->setObjectName("nombrePerfil");ap->setObjectName("paternoPerfil");am->setObjectName("maternoPerfil");c->setObjectName("correoPerfil");
    for(auto *campo:{n,ap,am}) {
        campo->setMaxLength(40);
        campo->setValidator(new QRegularExpressionValidator(QRegularExpression("[\\p{L}\\p{M} ]*"),campo));
    }
    c->setMaxLength(254);
    auto *actual=new QLineEdit;agregarVisibilidad(actual);
    actual->setPlaceholderText("Necesaria solo para cambiar la contraseña");
    auto *clave=new QLineEdit;auto *repetir=new QLineEdit;agregarVisibilidad(clave);agregarVisibilidad(repetir);
    clave->setPlaceholderText("Deja vacío si deseas conservarla");
    repetir->setPlaceholderText("Deja vacío si deseas conservarla");
    f->addRow("Nombre(s)",n);agregarApellidos(f,ap,am);f->addRow("Correo",c);f->addRow("Fecha de nacimiento",fecha);
    if(sesion) f->addRow("Contraseña actual",actual);
    else actual->setParent(&d),actual->hide();
    f->addRow("Nueva contraseña",clave);f->addRow("Confirmar nueva contraseña",repetir);
    auto *error=new QLabel;error->setWordWrap(true);error->setObjectName("passwordHint");l->addWidget(error);
    auto *acciones=new QHBoxLayout;l->addLayout(acciones);auto *cancelar=new QPushButton("Cancelar");auto *guardar=new QPushButton("Guardar cambios");guardar->setObjectName("primary");guardar->setDefault(true);cancelar->setAutoDefault(false);acciones->addWidget(cancelar);acciones->addWidget(guardar);
    QObject::connect(cancelar,&QPushButton::clicked,&d,&QDialog::reject);
    QObject::connect(guardar,&QPushButton::clicked,&d,[&](){
        DatosRegistro datos{n->text(),ap->text(),am->text(),c->text(),clave->text(),repetir->text(),fecha->date()};
        QString fallo=Validaciones::perfil(datos);
        if(fallo.isEmpty() && (!datos.clave.isEmpty() || !datos.confirmacion.isEmpty())) {
            fallo=Validaciones::clave(datos.clave);
            if(fallo.isEmpty() && datos.clave!=datos.confirmacion) fallo="Las contraseñas no coinciden.";
        }
        if(!fallo.isEmpty()) {error->setText(fallo); return;}
        if(!autorizar(c->text().trimmed(),clave->text()))return;
        if(sesion) {
            const Sesion identidad=*sesion;
            const QString anterior=actual->text();
            error->setText("Guardando cambios…");
            ejecutarAcceso(&d,[identidad,datos,anterior](ServicioAcceso &servicio){return servicio.actualizarPerfil(identidad,datos,anterior);},
                [&](ResultadoAcceso r){
                    if(!r.correcto()){error->setText(r.error);return;}
                    *sesion=r.sesion;
                    nombre=sesion->nombre;paterno=sesion->paterno;materno=sesion->materno;
                    correo=sesion->correo;nacimiento=sesion->nacimiento;
                    d.accept();
                });
            return;
        }
        nombre=n->text().trimmed();paterno=ap->text().trimmed();materno=am->text().trimmed();correo=c->text().trimmed();nacimiento=fecha->date();d.accept();
    });
    return d.exec()==QDialog::Accepted;
}
