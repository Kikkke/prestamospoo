#include "VentanaAcceso.h"
#include "PantallaLogin.h"
#include "PantallaRegistro.h"
#include "PanelPrincipal.h"
#include "PanelAdministrador.h"
#include "FondoAcceso.h"
#include "MarcaFinancionanti.h"

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QSqlQuery>
#include <QVariant>
#include <QFrame>
#include <QStackedWidget>
#include <QVBoxLayout>

VentanaAcceso::VentanaAcceso(QSqlDatabase base, QWidget *parent) : QWidget(parent)
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
    pantallas->addWidget(acceso);
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(pantallas);

    auto salir=[=]() {
        sesion={};
        pantallas->setCurrentWidget(acceso);
        while(pantallas->count()>1) {
            auto *anterior=pantallas->widget(1);
            pantallas->removeWidget(anterior);
            anterior->deleteLater();
        }
        login->limpiar(); registro->limpiar();
        formularios->setCurrentWidget(login);
    };
    auto entrar=[=](const Sesion &autenticada) {
        if(!autenticada.valida()) return;
        sesion=autenticada;
        QWidget *destino=nullptr;
        if(sesion.restringida) {
            auto *fondo=new FondoAcceso;
            auto *l=new QVBoxLayout(fondo);
            l->setContentsMargins(40,40,40,40);
            auto etiqueta=[l](const QString &t,const char *tipo) {
                auto *label=new QLabel(t); label->setTextFormat(Qt::PlainText);
                label->setObjectName(tipo); label->setWordWrap(true); l->addWidget(label);
            };
            etiqueta("Cuenta deshabilitada","title");
            etiqueta(nombreCompleto(sesion.nombre,sesion.paterno,sesion.materno),"section");
            etiqueta("Motivo: "+sesion.motivo+"\nFecha: "+sesion.deshabilitadaEn.toString("dd/MM/yyyy HH:mm"),"muted");
            etiqueta("Tu acceso está restringido. Puedes consultar aquí las respuestas de tus solicitudes.","muted");
            auto *historial=new QTextEdit; historial->setReadOnly(true); l->addWidget(historial);
            QSqlQuery q(base);
            q.prepare("SELECT a.creada_en,a.estado,a.mensaje,a.respuesta FROM apelacion a JOIN deshabilitacion d ON d.id=a.deshabilitacion_id WHERE d.cliente_id=:id ORDER BY a.creada_en DESC,a.id DESC");
            q.bindValue(":id",sesion.usuarioId);
            QStringList solicitudes;
            if(!q.exec()) solicitudes << "No se pudieron consultar las apelaciones. Vuelve a iniciar sesión para reintentar.";
            else while(q.next()) solicitudes << q.value(0).toDateTime().toString("dd/MM/yyyy HH:mm")+" · "+q.value(1).toString()+"\n"+q.value(2).toString()+"\n"+q.value(3).toString();
            historial->setPlainText(solicitudes.isEmpty()?"No tienes solicitudes registradas.":solicitudes.join("\n\n"));
            etiqueta("El envío y la resolución de apelaciones se habilitarán en la etapa de administración.","caption");
            auto *cerrar=new QPushButton("Cerrar sesión"); l->addWidget(cerrar);
            connect(cerrar,&QPushButton::clicked,this,salir);
            destino=fondo;
        } else if(sesion.rol==Rol::Administrador) {
            auto *admin=new PanelAdministrador(base,sesion);
            admin->salir=salir;
            destino=admin;
        } else {
            auto *panel=new PanelPrincipal;
            panel->cargarPerfilSesion(sesion.nombre,sesion.paterno,sesion.materno,sesion.correo,sesion.nacimiento);
            connect(panel,&PanelPrincipal::salirSolicitado,this,salir);
            destino=panel;
        }
        pantallas->addWidget(destino);
        pantallas->setCurrentWidget(destino);
    };
    connect(login,&PantallaLogin::accesoCompletado,this,entrar);
    connect(registro,&PantallaRegistro::accesoCompletado,this,entrar);
    connect(login,&PantallaLogin::crearCuentaSolicitada,this,[=]() {
        login->limpiar(); registro->limpiar(); formularios->setCurrentWidget(registro);
    });
    connect(registro,&PantallaRegistro::volverSolicitado,this,[=]() {
        registro->limpiar(); login->limpiar(); formularios->setCurrentWidget(login);
    });
}
