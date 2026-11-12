#pragma once
#include "FondoAcceso.h"
#include "MarcaFinancionanti.h"
#include "CampoContrasena.h"
#include <QDialog>
#include <QPushButton>
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QScrollArea>
#include <QGridLayout>
#include <QRandomGenerator>
#include <functional>
#include <QRegularExpression>

// Silueta vectorial circular basada en la referencia del usuario.
class AvatarCliente : public QWidget {
public:
    explicit AvatarCliente(QWidget *parent=nullptr):QWidget(parent) {
        setFixedSize(88,88); setObjectName("topicIcon");
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAccessibleName("Icono de cliente");
    }
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
        p.scale(width()/100.0,height()/100.0);
        const QColor cobre("#ed916f");
        p.setPen(QPen(cobre,2.8)); p.setBrush(Qt::NoBrush);
        p.drawEllipse(QRectF(4,4,92,92));
        p.setPen(Qt::NoPen); p.setBrush(cobre);
        p.drawEllipse(QRectF(33,23,34,34));
        QPainterPath cuerpo;
        cuerpo.moveTo(22,79); cuerpo.cubicTo(29,51,71,51,78,79);
        cuerpo.quadTo(50,102,22,79); cuerpo.closeSubpath();
        p.drawPath(cuerpo);
    }
};

// Datos exclusivamente ilustrativos; viven durante esta ejecución.
class PanelAdministrador : public QWidget {
public:
    std::function<void()> salir;
    struct Sim { QString nombre; double monto; double tasa; int meses; };
    struct Cliente { QString nombre,correo; QDate ultimo; bool habilitado=true; QString motivo; QList<Sim> sims; };
    struct Apelacion { int cliente; QString texto,estado,respuesta; QDate fecha; };
    struct Admin { QString nombre,correo,folio; QDate nacimiento; };
    QList<Cliente> clientes;
    QList<Apelacion> apelaciones;
    QList<Admin> admins;
    PanelAdministrador(QWidget *parent=nullptr):QWidget(parent) {
        setObjectName("dashboard");
        admins={{"Enrique","enrique@demo.mx","ENR310307GB34",QDate(2007,3,31)},{"Andrea","andrea@demo.mx","AND150398JK72",QDate(1998,3,15)}};
        QStringList nombres={"Ana López","Luis Pérez","María Torres","Carlos Ruiz","Sofía García","Diego Hernández"};
        for(int i=0;i<nombres.size();++i) clientes.append({nombres[i],QString("cliente%1@demo.mx").arg(i+1),QDate::currentDate().addMonths(-i*2),true,{},{{"Computadora",12000,0,3},{"Estudios",24000,12,12}}});
        clientes[0].habilitado=false; clientes[0].motivo="Inactividad";
        apelaciones.append({0,"Deseo retomar mis simulaciones y solicito reactivar mi cuenta.","Pendiente",{},QDate::currentDate()});
        auto *raiz=new QHBoxLayout(this); raiz->setContentsMargins(0,0,0,0); raiz->setSpacing(0);
        auto *lateral=new QFrame; lateral->setObjectName("sidebar"); lateral->setFixedWidth(206);
        auto *menu=new QVBoxLayout(lateral); menu->setContentsMargins(16,24,16,20); menu->setSpacing(12);
        menu->addWidget(logoFinancionanti(64)); menu->addWidget(label("FinancionantiMex","brandSidebar"));
        menu->addWidget(label("Bienvenido, Enrique","section")); menu->addWidget(label("ADMINISTRACIÓN","eyebrow"));
        auto *navegacion=new QButtonGroup(this);
        navegacion->setExclusive(true);
        for(const auto &s:QStringList{"Clientes","Administradores","Apelaciones"}) {
            auto *b=button(s,menu); b->setCheckable(true);
            navegacion->addButton(b); b->setChecked(s==pagina);
            connect(b,&QPushButton::clicked,this,[this,s](){ if(pagina!=s) { pagina=s; render(); } });
        }
        menu->addStretch(); menu->addWidget(label("PROTOTIPO\nCambios temporales","caption"));
        connect(button("Cerrar sesión",menu),&QPushButton::clicked,this,[this](){if(salir)salir();});
        raiz->addWidget(lateral);
        scroll=new QScrollArea; scroll->setWidgetResizable(true); raiz->addWidget(scroll,1); render();
    }
    bool confirmar(const QString &accion) {
        QDialog d(this); d.setWindowTitle("Confirmación administrativa"); d.setMinimumWidth(430);
        auto *l=new QVBoxLayout(&d); l->addWidget(label(accion,"section"));
        l->addWidget(label("Para continuar, ingresa tu folio de administrador.","muted"));
        l->addWidget(label("Demostración: ENR310307GB34","caption"));
        auto *campo=new QLineEdit; agregarVisibilidad(campo); campo->setPlaceholderText("Folio de administrador"); l->addWidget(campo);
        auto *error=label("","passwordHint"); l->addWidget(error);
        auto *botones=new QDialogButtonBox; auto *ok=botones->addButton("Confirmar",QDialogButtonBox::AcceptRole); botones->addButton("Cancelar",QDialogButtonBox::RejectRole); l->addWidget(botones);
        connect(botones,&QDialogButtonBox::rejected,&d,&QDialog::reject);
        connect(ok,&QPushButton::clicked,&d,[&](){if(campo->text().trimmed()==admins.first().folio)d.accept();else error->setText("Folio incorrecto. Revisa el código de demostración.");});
        return d.exec()==QDialog::Accepted;
    }
    void accesoCliente() {
        if(clientes[0].habilitado) { QMessageBox::information(this,"Cuenta reactivada","Tu apelación fue aprobada. Tu cuenta vuelve a estar habilitada."); return; }
        QDialog d(this); d.setWindowTitle("Cuenta deshabilitada"); d.setMinimumWidth(500);
        auto *l=new QVBoxLayout(&d); l->addWidget(label("Cuenta deshabilitada","title"));
        l->addWidget(label("Por razones de inactividad o uso inapropiado, tu cuenta ha sido deshabilitada. Pulsa el botón para realizar una apelación.","muted"));
        l->addWidget(label("Cliente de muestra: Ana López\nMotivo: "+clientes[0].motivo));
        bool pendiente=false;
        for(const auto &a:apelaciones) if(a.cliente==0) {
            l->addWidget(label(a.fecha.toString("dd/MM/yyyy")+" · "+a.estado+"\n"+a.respuesta,"caption"));
            if(a.estado=="Pendiente")pendiente=true;
        }
        auto *texto=new QTextEdit; texto->setPlaceholderText("Explica por qué solicitas la reactivación"); texto->setMaximumHeight(140); l->addWidget(texto);
        auto *enviar=button(pendiente?"Ya tienes una apelación pendiente":"Enviar apelación",l); enviar->setEnabled(!pendiente);
        connect(enviar,&QPushButton::clicked,&d,[&](){if(texto->toPlainText().trimmed().isEmpty()){texto->setPlaceholderText("Escribe una explicación antes de enviar.");return;} apelaciones.append({0,texto->toPlainText().trimmed(),"Pendiente",{},QDate::currentDate()}); d.accept(); render();});
        connect(button("Volver al acceso",l),&QPushButton::clicked,&d,&QDialog::reject); d.exec();
    }
private:
    QScrollArea *scroll; QString pagina="Clientes";
    static QLabel *label(const QString &s,const char *tipo="") {auto *l=new QLabel(s);l->setWordWrap(true);l->setObjectName(tipo);return l;}
    static QPushButton *button(const QString &s,QBoxLayout *l) {auto *b=new QPushButton(s);b->setCursor(Qt::PointingHandCursor);l->addWidget(b);return b;}
    static QVBoxLayout *card(QBoxLayout *l) {auto *f=new QFrame;f->setObjectName("card");auto *c=new QVBoxLayout(f);c->setContentsMargins(20,16,20,16);c->setSpacing(12);l->addWidget(f);return c;}
    QString estado(int i) const {if(!clientes[i].habilitado)return "Deshabilitada";return clientes[i].ultimo>=QDate::currentDate().addMonths(-6)?"Activo":"Inactivo";}
    void render() {
        auto *viejo=scroll->takeWidget(); if(viejo)viejo->deleteLater();
        auto *f=new FondoAcceso; auto *l=new QVBoxLayout(f);l->setContentsMargins(24,24,24,24);l->setSpacing(16);scroll->setWidget(f);
        l->addWidget(label("ADMINISTRACIÓN / "+pagina,"eyebrow"));l->addWidget(label(pagina,"title"));
        l->addWidget(label("Datos de demostración · No se guardan al cerrar la aplicación","caption"));
        if(pagina=="Clientes") {
            auto *barra=new QHBoxLayout;auto *buscar=new QLineEdit;buscar->setPlaceholderText("Buscar por nombre o correo…");auto *filtro=new QComboBox;filtro->addItems({"Todos","Activo","Inactivo","Deshabilitada"});barra->addWidget(buscar,1);barra->addWidget(filtro);l->addLayout(barra);
            auto *rejilla=new QGridLayout;l->addLayout(rejilla); auto tarjetas=QList<QPushButton*>();
            for(int i=0;i<clientes.size();++i){ const auto &c=clientes[i];auto *b=new QPushButton; b->setObjectName("topicCard");b->setMinimumHeight(300);b->setCursor(Qt::PointingHandCursor);auto *contenido=new QVBoxLayout(b);contenido->setContentsMargins(20,16,20,16); contenido->addWidget(new AvatarCliente);
                for(auto *t:{label(c.nombre,"section"),label(c.correo,"muted"),label("Último acceso: "+c.ultimo.toString("dd/MM/yyyy"),"caption"),label(estado(i)+QString(" · %1 simulaciones").arg(c.sims.size()),"badge"),label("Ver información →","topicLink")}){t->setAttribute(Qt::WA_TransparentForMouseEvents);contenido->addWidget(t);}
                rejilla->addWidget(b,i/2,i%2);tarjetas.append(b);connect(b,&QPushButton::clicked,this,[this,i](){ficha(i);});}
            auto filtrar=[this,buscar,filtro,tarjetas,rejilla](){int pos=0;for(int i=0;i<tarjetas.size();++i){bool visible=(clientes[i].nombre+" "+clientes[i].correo).contains(buscar->text().trimmed(),Qt::CaseInsensitive)&&(filtro->currentIndex()==0||estado(i)==filtro->currentText());tarjetas[i]->setVisible(visible);if(visible)rejilla->addWidget(tarjetas[i],pos/2,pos%2),++pos;}};
            connect(buscar,&QLineEdit::textChanged,this,filtrar);connect(filtro,&QComboBox::currentTextChanged,this,filtrar);
        } else if(pagina=="Administradores") {
            l->addWidget(label("Equipo de administración · Mismos permisos para todas las cuentas", "muted"));
            auto *barra=new QHBoxLayout;
            auto *buscar=new QLineEdit; buscar->setPlaceholderText("Buscar administrador por nombre o correo…");
            barra->addWidget(buscar,1);
            auto *agregar=button("+ Agregar administrador",barra); agregar->setObjectName("primary");
            connect(agregar,&QPushButton::clicked,this,[this](){agregarAdmin();}); l->addLayout(barra);
            auto *contador=label(QString("%1 administradores · Consulta de datos protegida por folio").arg(admins.size()),"caption"); l->addWidget(contador);
            auto *grid=new QGridLayout; grid->setSpacing(16); l->addLayout(grid);
            QList<QFrame*> tarjetas;
            for(int i=0;i<admins.size();++i) {
                auto *frame=new QFrame; frame->setObjectName("card");
                auto *c=new QVBoxLayout(frame); c->setContentsMargins(20,20,20,20); c->setSpacing(12);
                auto *cabecera=new QHBoxLayout; auto *avatar=new AvatarCliente; avatar->setFixedSize(56,56);
                cabecera->addWidget(avatar); cabecera->addStretch();
                cabecera->addWidget(label(i==0?"TU SESIÓN":"ADMINISTRADOR","badge")); c->addLayout(cabecera);
                c->addWidget(label(admins[i].nombre,"section")); c->addWidget(label(admins[i].correo,"muted"));
                c->addWidget(label("Consulta su ficha con el folio de tu sesión activa.","caption"));
                auto *acciones=new QHBoxLayout; c->addLayout(acciones);
                connect(button("Ver información →",acciones),&QPushButton::clicked,this,[this,i](){verAdmin(i);});
                auto *eliminar=button("Eliminar",acciones); eliminar->setEnabled(i!=0&&admins.size()>1);
                eliminar->setToolTip(i==0?"No puedes eliminar tu propia sesión":"Eliminar esta cuenta administrativa");
                connect(eliminar,&QPushButton::clicked,this,[this,i](){if(confirmar("Eliminar a "+admins[i].nombre+". Perderá el acceso administrativo.")){admins.removeAt(i);render();}});
                grid->addWidget(frame,i/2,i%2); tarjetas.append(frame);
            }
            auto *vacio=label("No hay administradores que coincidan con la búsqueda.","muted"); vacio->hide(); l->addWidget(vacio);
            connect(buscar,&QLineEdit::textChanged,f,[this,buscar,grid,tarjetas,contador,vacio](){
                int visibles=0;
                for(int i=0;i<tarjetas.size();++i){bool coincide=(admins[i].nombre+" "+admins[i].correo).contains(buscar->text().trimmed(),Qt::CaseInsensitive); tarjetas[i]->setVisible(coincide);if(coincide){grid->addWidget(tarjetas[i],visibles/2,visibles%2);++visibles;}}
                contador->setText(QString("Mostrando %1 de %2 administradores").arg(visibles).arg(admins.size()));vacio->setVisible(visibles==0);
            });
        } else {
            l->addWidget(label("Revisa las solicitudes y comunica una resolución al cliente.","muted"));
            auto *resumen=new QHBoxLayout; l->addLayout(resumen);
            for(const QString &estado:QStringList{"Pendiente","Aprobada","Rechazada"}) {
                int cantidad=0;for(const auto &a:apelaciones)if(a.estado==estado)++cantidad;
                auto *c=card(resumen);c->addWidget(label(estado=="Pendiente"?"Pendientes":estado=="Aprobada"?"Aprobadas":"Rechazadas","muted"));c->addWidget(label(QString::number(cantidad),estado=="Pendiente"?"metricAccent":"metric"));
            }
            auto *barra=new QHBoxLayout; auto *buscar=new QLineEdit;buscar->setPlaceholderText("Buscar apelación por cliente o correo…");
            auto *filtro=new QComboBox;filtro->addItems({"Todas","Pendiente","Aprobada","Rechazada"});barra->addWidget(buscar,1);barra->addWidget(filtro);l->addLayout(barra);
            QList<QWidget*> tarjetas;
            for(int i=0;i<apelaciones.size();++i){const auto &a=apelaciones[i];auto *c=card(l);tarjetas.append(c->parentWidget());
                auto *cabecera=new QHBoxLayout;auto *avatar=new AvatarCliente;avatar->setFixedSize(44,44);cabecera->addWidget(avatar);
                auto *identidad=new QVBoxLayout;identidad->addWidget(label(clientes[a.cliente].nombre,"section"));identidad->addWidget(label(clientes[a.cliente].correo,"caption"));cabecera->addLayout(identidad,1);cabecera->addWidget(label(a.estado,"badge"));c->addLayout(cabecera);
                c->addWidget(label("Solicitud del "+a.fecha.toString("dd/MM/yyyy")+" · Motivo: "+clientes[a.cliente].motivo,"caption"));
                c->addWidget(label("MENSAJE DEL CLIENTE","eyebrow"));c->addWidget(label(a.texto));
                if(!a.respuesta.isEmpty()){c->addWidget(label("RESOLUCIÓN","eyebrow"));c->addWidget(label(a.respuesta,"muted"));}
                if(a.estado=="Pendiente") {
                    auto *acciones=new QHBoxLayout;c->addLayout(acciones);acciones->addStretch();
                    for(bool aprobar:{false,true}) {
                        auto *accion=button(aprobar?"Aprobar y reactivar":"Rechazar",acciones);if(aprobar)accion->setObjectName("primary");
                        connect(accion,&QPushButton::clicked,this,[this,i,aprobar](){bool ok=true;QString motivo=aprobar?"Tu cuenta ha sido reactivada.":QInputDialog::getMultiLineText(this,"Resolver apelación","Motivo del rechazo:",{},&ok);if(!ok||motivo.trimmed().isEmpty())return;if(confirmar(aprobar?"Aprobar apelación y reactivar cuenta":"Rechazar apelación: "+motivo)){apelaciones[i].estado=aprobar?"Aprobada":"Rechazada";apelaciones[i].respuesta=motivo;if(aprobar)clientes[apelaciones[i].cliente].habilitado=true;render();}});
                    }
                }
            }
            auto *vacio=label("No hay apelaciones que coincidan con estos filtros.","muted");l->addWidget(vacio);
            auto filtrar=[this,buscar,filtro,tarjetas,vacio](){int visibles=0;for(int i=0;i<tarjetas.size();++i){const auto &a=apelaciones[i];const auto &cliente=clientes[a.cliente];bool coincide=(cliente.nombre+" "+cliente.correo).contains(buscar->text().trimmed(),Qt::CaseInsensitive)&&(filtro->currentIndex()==0||a.estado==filtro->currentText());tarjetas[i]->setVisible(coincide);if(coincide)++visibles;}vacio->setVisible(visibles==0);};
            connect(buscar,&QLineEdit::textChanged,f,filtrar);connect(filtro,&QComboBox::currentTextChanged,f,filtrar);filtrar();
        } l->addStretch();
    }
    void ficha(int i) {
        QDialog d(this);d.setWindowTitle("Ficha del cliente");d.resize(650,600);auto *l=new QVBoxLayout(&d);l->addWidget(label(clientes[i].nombre,"title"));l->addWidget(label(clientes[i].correo+"\nEstado: "+estado(i),"muted"));
        auto *des=button("Deshabilitar cuenta",l);des->setEnabled(clientes[i].habilitado);
        connect(des,&QPushButton::clicked,&d,[&](){bool ok;auto motivo=QInputDialog::getItem(&d,"Deshabilitar cuenta","Selecciona el motivo",{"Inactividad","Uso inapropiado"},0,false,&ok);if(ok&&confirmar("Deshabilitar a "+clientes[i].nombre+"\nMotivo: "+motivo)){clientes[i].habilitado=false;clientes[i].motivo=motivo;d.accept();render();}});
        auto *sc=new QScrollArea;sc->setWidgetResizable(true);auto *contenido=new QWidget;auto *lista=new QVBoxLayout(contenido);sc->setWidget(contenido);l->addWidget(sc);
        for(int j=0;j<clientes[i].sims.size();++j){auto s=clientes[i].sims[j];auto *c=card(lista);c->addWidget(label(s.nombre,"section"));c->addWidget(label(QString("$%1 MXN · %2 % anual · %3 meses").arg(s.monto,0,'f',2).arg(s.tasa).arg(s.meses),"muted"));
            connect(button("Editar",c),&QPushButton::clicked,&d,[&,j](){if(editar(i,j)){d.accept();render();}});
            connect(button("Eliminar simulación",c),&QPushButton::clicked,&d,[&,j](){if(confirmar("Eliminar la simulación «"+clientes[i].sims[j].nombre+"» de "+clientes[i].nombre)){clientes[i].sims.removeAt(j);d.accept();render();}});
        }if(clientes[i].sims.isEmpty())lista->addWidget(label("Este cliente no tiene simulaciones guardadas.","muted"));
        connect(button("Cerrar ficha",l),&QPushButton::clicked,&d,&QDialog::reject);d.exec();
    }
    bool editar(int i,int j) {
        const auto anterior=clientes[i].sims[j];QDialog d(this);d.setWindowTitle("Editar simulación");auto *l=new QVBoxLayout(&d);auto *f=new QFormLayout;l->addLayout(f);
        auto *nombre=new QLineEdit(anterior.nombre);auto *monto=new QDoubleSpinBox;monto->setRange(0.01,999999999);monto->setValue(anterior.monto);auto *tasa=new QDoubleSpinBox;tasa->setRange(0,1000);tasa->setValue(anterior.tasa);auto *meses=new QSpinBox;meses->setRange(1,600);meses->setValue(anterior.meses);
        f->addRow("Nombre",nombre);f->addRow("Monto MXN",monto);f->addRow("Tasa anual %",tasa);f->addRow("Plazo en meses",meses);l->addWidget(label("Prototipo: se editan las condiciones de muestra. El recálculo se implementará después.","caption"));
        connect(button("Guardar cambios",l),&QPushButton::clicked,&d,[&](){if(nombre->text().trimmed().isEmpty())return;QString resumen=QString("%1 → %2\nMonto: %3 → %4\nTasa anual: %5 → %6\nMeses: %7 → %8").arg(anterior.nombre,nombre->text()).arg(anterior.monto).arg(monto->value()).arg(anterior.tasa).arg(tasa->value()).arg(anterior.meses).arg(meses->value());if(confirmar(resumen))d.accept();});connect(button("Cancelar",l),&QPushButton::clicked,&d,&QDialog::reject);
        if(d.exec()!=QDialog::Accepted)return false;clientes[i].sims[j]={nombre->text().trimmed(),monto->value(),tasa->value(),meses->value()};return true;
    }
    void verAdmin(int i) {
        if(!confirmar("Consultar los datos de "+admins[i].nombre))return;
        const auto &a=admins[i];
        QDialog d(this);d.setWindowTitle("Información del administrador");d.setMinimumWidth(480);
        auto *l=new QVBoxLayout(&d);l->setContentsMargins(24,24,24,24);l->setSpacing(16);
        auto *cabecera=new QHBoxLayout;auto *avatar=new AvatarCliente;avatar->setFixedSize(64,64);cabecera->addWidget(avatar);cabecera->addWidget(label(a.nombre,"title"),1);l->addLayout(cabecera);
        l->addWidget(label(i==0?"Sesión actual · Administrador":"Administrador · Mismos permisos","badge"));
        auto *datos=new QFormLayout;datos->setSpacing(16);datos->addRow("Nombre",label(a.nombre));datos->addRow("Correo",label(a.correo));datos->addRow("Fecha de nacimiento",label(a.nacimiento.toString("dd/MM/yyyy")));datos->addRow("Contraseña",label("Protegida · No se muestra"));datos->addRow("Folio",label("Reservado · Se entrega al crear la cuenta"));l->addLayout(datos);
        l->addWidget(label("Consulta autorizada con el folio de Enrique, administrador de la sesión de demostración.","caption"));
        connect(button("Cerrar ficha",l),&QPushButton::clicked,&d,&QDialog::accept);d.exec();
    }
    void agregarAdmin() {
        QDialog d(this);d.setWindowTitle("Agregar administrador");d.setMinimumWidth(480);
        auto *l=new QVBoxLayout(&d);l->setContentsMargins(24,24,24,24);l->setSpacing(14);
        l->addWidget(label("Nuevo administrador","title"));l->addWidget(label("Tendrá los mismos permisos que el resto del equipo. Su folio se generará al crear la cuenta.","muted"));
        auto *f=new QFormLayout;f->setRowWrapPolicy(QFormLayout::WrapAllRows);f->setSpacing(10);l->addLayout(f);
        auto *nombre=new QLineEdit;nombre->setPlaceholderText("Nombre completo");auto *correo=new QLineEdit;correo->setPlaceholderText("nombre@correo.com");auto *clave=new QLineEdit;agregarVisibilidad(clave);
        auto *fecha=new QDateEdit(QDate(2000,1,1));fecha->setCalendarPopup(true);fecha->setDisplayFormat("dd/MM/yyyy");fecha->setMaximumDate(QDate::currentDate());
        f->addRow("Nombre",nombre);f->addRow("Correo",correo);f->addRow("Contraseña",clave);f->addRow("Fecha de nacimiento",fecha);
        auto *error=label("","passwordHint");l->addWidget(error);l->addWidget(label("Demostración: utiliza datos ficticios. La contraseña no se conserva ni autentica nuevas sesiones.","caption"));
        auto *acciones=new QHBoxLayout;l->addLayout(acciones);connect(button("Cancelar",acciones),&QPushButton::clicked,&d,&QDialog::reject);auto *crear=button("Crear administrador",acciones);crear->setObjectName("primary");
        connect(crear,&QPushButton::clicked,&d,[&](){
            QString prefijo;for(auto c:nombre->text().normalized(QString::NormalizationForm_D).toUpper())if(c>='A'&&c<='Z'&&prefijo.size()<3)prefijo+=c;
            if(prefijo.size()<3||!QRegularExpression("^[^\\s@]+@[^\\s@]+\\.[^\\s@]+$").match(correo->text().trimmed()).hasMatch()||clave->text().isEmpty()){error->setText("Escribe un nombre con al menos tres letras, un correo válido y una contraseña.");return;}
            if(fecha->date().addYears(18)>QDate::currentDate()){error->setText("El administrador debe tener al menos 18 años.");return;}
            for(const auto &a:admins)if(a.correo.compare(correo->text().trimmed(),Qt::CaseInsensitive)==0){error->setText("Ese correo ya está registrado.");return;}
            if(!confirmar("Crear administrador: "+nombre->text().trimmed()+" · "+correo->text().trimmed()))return;
            QString folio;bool repetido;
            do { folio=prefijo+fecha->date().toString("ddMMyy")+"-";const QString alfabeto="ABCDEFGHJKLMNPQRSTUVWXYZ23456789";for(int k=0;k<8;++k)folio+=alfabeto[QRandomGenerator::system()->bounded(int(alfabeto.size()))];repetido=false;for(const auto &a:admins)if(a.folio==folio)repetido=true; }while(repetido);
            admins.append({nombre->text().trimmed(),correo->text().trimmed(),folio,fecha->date()});
            QDialog entrega(&d);entrega.setWindowTitle("Administrador creado");entrega.setMinimumWidth(480);auto *e=new QVBoxLayout(&entrega);e->setContentsMargins(24,24,24,24);e->setSpacing(16);
            e->addWidget(label("Cuenta de muestra creada","title"));e->addWidget(label(nombre->text().trimmed()+" · "+correo->text().trimmed(),"muted"));e->addWidget(label("FOLIO ASIGNADO","eyebrow"));
            auto *valor=new QLineEdit(folio);valor->setReadOnly(true);e->addWidget(valor);e->addWidget(label("Guarda este folio. Puedes seleccionarlo y copiarlo; no aparecerá en el directorio de administradores.","caption"));
            connect(button("Entendido",e),&QPushButton::clicked,&entrega,&QDialog::accept);entrega.exec();d.accept();render();
        });d.exec();
    }
};
