#pragma once
#include "CampoNacimiento.h"
#include "Acceso.h"
#include "CamposNombre.h"
#include "CampoContrasena.h"
#include <QDialog>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <functional>
inline bool editarPerfil(QWidget *padre,QString &nombre,QString &paterno,QString &materno,QString &correo,QDate &nacimiento,
                         const std::function<bool(const QString &,const QString &)> &autorizar) {
    QDialog d(padre);d.setWindowTitle("Editar datos");d.setMinimumWidth(460);
    auto *l=new QVBoxLayout(&d);l->setContentsMargins(24,24,24,24);l->setSpacing(14);
    auto *titulo=new QLabel("Datos de la cuenta");titulo->setObjectName("title");l->addWidget(titulo);
    auto *f=new QFormLayout;f->setRowWrapPolicy(QFormLayout::WrapAllRows);f->setSpacing(10);l->addLayout(f);
    auto *n=new QLineEdit(nombre);auto *ap=new QLineEdit(paterno);auto *am=new QLineEdit(materno);auto *c=new QLineEdit(correo);auto *fecha=crearCampoNacimiento();fecha->setDate(nacimiento);
    auto *clave=new QLineEdit;auto *repetir=new QLineEdit;agregarVisibilidad(clave);agregarVisibilidad(repetir);
    clave->setPlaceholderText("Deja vacío si desea conservarla");
    repetir->setPlaceholderText("Deja vacío si desea conservarla");
    f->addRow("Nombre(s)",n);agregarApellidos(f,ap,am);f->addRow("Correo",c);f->addRow("Fecha de nacimiento",fecha);
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
        nombre=n->text().trimmed();paterno=ap->text().trimmed();materno=am->text().trimmed();correo=c->text().trimmed();nacimiento=fecha->date();d.accept();
    });
    return d.exec()==QDialog::Accepted;
}
