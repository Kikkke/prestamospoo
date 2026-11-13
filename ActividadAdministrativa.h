#pragma once
#include "CampoNacimiento.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QSet>
#include <QDateTime>
#include <QGridLayout>
#include <QResizeEvent>
class ActividadAdministrativa : public QWidget {
    struct Evento {QDateTime fecha;QString responsable,tipo,objetivo,motivo,antes,despues;};
    QList<Evento> eventos;QList<int> visibles;int pagina=0;
    QLineEdit *buscar;QComboBox *admin,*tipo;QDateEdit *desde,*hasta;
    QTableWidget *tabla;QLabel *contador,*total,*participantes,*resueltas;QPushButton *anterior,*siguiente;
    QGridLayout *filtros;
    QBoxLayout *metricas,*pie;
    QList<QWidget*> camposFiltro;
    int columnasFiltro=0;
    void adaptar(int ancho) {
        int columnas=ancho>=1150?5:ancho>=600?2:1;
        if(columnas!=columnasFiltro){
            for(auto *campo:camposFiltro)filtros->removeWidget(campo);
            for(int c=0;c<5;++c)filtros->setColumnStretch(c,c<columnas?1:0);
            for(int i=0;i<camposFiltro.size();++i)filtros->addWidget(camposFiltro[i],i/columnas,i%columnas);
            columnasFiltro=columnas;
        }
        metricas->setDirection(ancho<600?QBoxLayout::TopToBottom:QBoxLayout::LeftToRight);
        pie->setDirection(ancho<600?QBoxLayout::TopToBottom:QBoxLayout::LeftToRight);
        tabla->setColumnHidden(4,ancho<950);
        tabla->setColumnHidden(3,ancho<650);
        tabla->resizeRowsToContents();
    }
protected:
    void resizeEvent(QResizeEvent *evento) override {
        QWidget::resizeEvent(evento);adaptar(evento->size().width());
    }
private:
    static QLabel *texto(QString s,const char *nombre=""){auto *x=new QLabel(s);x->setObjectName(nombre);x->setWordWrap(true);return x;}
    void actualizar(bool reiniciar=true) {
        if(reiniciar)pagina=0;visibles.clear();QSet<QString> personas;int apelaciones=0;
        for(int i=0;i<eventos.size();++i){const auto &e=eventos[i];
            if(e.fecha.date()<desde->date()||e.fecha.date()>hasta->date())continue;
            if(admin->currentIndex()&&admin->currentText()!=e.responsable)continue;
            if(tipo->currentIndex()&&tipo->currentText()!=e.tipo)continue;
            if(!(e.responsable+" "+e.tipo+" "+e.objetivo+" "+e.motivo).contains(buscar->text().trimmed(),Qt::CaseInsensitive))continue;
            visibles.append(i);personas.insert(e.responsable);if(e.tipo.startsWith("Apelación"))++apelaciones;
        }
        total->setText(QString::number(visibles.size()));participantes->setText(QString::number(personas.size()));resueltas->setText(QString::number(apelaciones));
        tabla->setRowCount(0);
        for(int n=pagina*8;n<qMin(int(visibles.size()),(pagina+1)*8);++n){
            const auto &e=eventos[visibles[n]];int r=tabla->rowCount();tabla->insertRow(r);
            QStringList campos={e.fecha.toString("dd/MM/yyyy · HH:mm"),e.responsable,e.tipo,e.objetivo,"Ver detalle →"};
            for(int c=0;c<campos.size();++c)tabla->setItem(r,c,new QTableWidgetItem(campos[c]));
            for(int c=0;c<campos.size();++c)tabla->item(r,c)->setToolTip(campos[c]);
        }
        contador->setText(visibles.isEmpty()?"No hay acciones que coincidan con los filtros.":QString("%1 resultados · Página %2 de %3").arg(visibles.size()).arg(pagina+1).arg((visibles.size()+7)/8));
        tabla->resizeRowsToContents();
        anterior->setEnabled(pagina>0);siguiente->setEnabled((pagina+1)*8<visibles.size());
    }
    void detalle(int fila) {
        int n=pagina*8+fila;if(n>=visibles.size())return;const auto &e=eventos[visibles[n]];
        QDialog d(this);d.setWindowTitle("Detalle de actividad");d.resize(580,480);auto *l=new QVBoxLayout(&d);l->setContentsMargins(24,24,24,24);l->setSpacing(16);
        l->addWidget(texto(e.tipo,"title"));l->addWidget(texto(e.responsable+" · "+e.fecha.toString("dd/MM/yyyy HH:mm"),"muted"));l->addWidget(texto(e.objetivo,"section"));
        auto *lectura=new QTextEdit;lectura->setReadOnly(true);lectura->setLineWrapMode(QTextEdit::WidgetWidth);lectura->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);lectura->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        lectura->setPlainText("MOTIVO / DESCRIPCIÓN\n"+e.motivo+"\n\nANTES\n"+e.antes+"\n\nDESPUÉS\n"+e.despues);l->addWidget(lectura);
        l->addWidget(texto("Registro ilustrativo de solo lectura. No contiene contraseñas ni folios.","caption"));
        auto *cerrar=new QPushButton("Cerrar detalle");l->addWidget(cerrar);connect(cerrar,&QPushButton::clicked,&d,&QDialog::accept);d.exec();
    }
public:
    QSize minimumSizeHint() const override { return QSize(300,400); }
    ActividadAdministrativa(QWidget *parent=nullptr):QWidget(parent) {
        setObjectName("page");
        QStringList nombres={"Enrique","Pedro","Uziel","Alexander","Jared"};
        QStringList tipos={"Administrador creado","Administrador editado","Administrador eliminado","Cliente deshabilitado","Cliente reactivado","Simulación editada","Simulación eliminada","Apelación aprobada","Apelación rechazada","Contraseña cambiada","Folio propio consultado"};
        for(int i=0;i<24;++i){int t=i%tipos.size();eventos.append({QDateTime(QDate::currentDate().addDays(-i/3),QTime(18-i%3,35)),nombres[i%5],tipos[t],t<3?"Cuenta administrativa de muestra":t==10?"Cuenta propia":"Cliente de muestra · Simulación de estudios",
            t==8?"La solicitud requiere información adicional para justificar la reactivación. Se comunica el motivo al cliente.":t==10?"Consulta propia autorizada por otro administrador. El valor del folio no se registra.":"Acción ilustrativa para mostrar la organización del historial administrativo.",
            t==5?"Monto: $12,000.00 · Plazo: 6 meses":t==7?"Apelación pendiente":t==8?"Apelación pendiente":"No aplica",
            t==5?"Monto: $18,000.00 · Plazo: 12 meses":t==7?"Aprobada · Cuenta reactivada":t==8?"Rechazada · Cuenta deshabilitada":"Acción completada (ejemplo)"});}
        auto *l=new QVBoxLayout(this);l->setSizeConstraint(QLayout::SetNoConstraint);l->setContentsMargins(0,0,0,0);l->setSpacing(16);
        l->addWidget(texto("Historial compartido · Disponible para todos los administradores","muted"));
        metricas=new QHBoxLayout;l->addLayout(metricas);
        QList<QLabel*> valores;
        for(auto titulo:QStringList{"Acciones del filtro","Administradores participantes","Apelaciones resueltas"}){auto *f=new QFrame;f->setObjectName("card");auto *c=new QVBoxLayout(f);c->setContentsMargins(20,16,20,16);c->addWidget(texto(titulo,"caption"));auto *v=texto("0","metricAccent");c->addWidget(v);valores.append(v);metricas->addWidget(f);}
        total=valores[0];participantes=valores[1];resueltas=valores[2];
        buscar=new QLineEdit;buscar->setPlaceholderText("Buscar administrador, cliente, simulación o descripción…");l->addWidget(buscar);
        filtros=new QGridLayout;l->addLayout(filtros);
        admin=new QComboBox;admin->addItem("Todos los administradores");admin->addItems(nombres);
        tipo=new QComboBox;tipo->addItem("Todas las acciones");tipo->addItems(tipos);
        desde=crearCampoNacimiento();hasta=crearCampoNacimiento();desde->setDate(QDate::currentDate().addDays(-30));hasta->setDate(QDate::currentDate());
        auto *limpiar=new QPushButton("Limpiar filtros");
        for(auto *combo:{admin,tipo}){combo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);combo->setMinimumContentsLength(10);combo->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);}
        QList<QWidget*> controles={admin,tipo,desde,hasta,limpiar};
        QStringList etiquetas={"Administrador","Acción","Desde","Hasta",""};
        for(int i=0;i<controles.size();++i){
            auto *grupo=new QWidget;grupo->setObjectName("activityFilterGroup");auto *col=new QVBoxLayout(grupo);col->setContentsMargins(0,0,0,0);col->setSpacing(6);
            auto *etiqueta=texto(etiquetas[i],"caption");etiqueta->setFixedHeight(20);
            controles[i]->setFixedHeight(40);
            col->addWidget(etiqueta);col->addWidget(controles[i]);
            camposFiltro.append(grupo);filtros->addWidget(grupo,i,0);
        }
        tabla=new QTableWidget(0,5);tabla->setHorizontalHeaderLabels({"Fecha y hora","Responsable","Acción","Elemento afectado","Detalle"});
        tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);tabla->horizontalHeader()->setMinimumSectionSize(70);tabla->setMinimumWidth(0);tabla->setWordWrap(true);tabla->verticalHeader()->setMinimumSectionSize(58);tabla->verticalHeader()->hide();tabla->verticalHeader()->setDefaultSectionSize(58);tabla->setMinimumHeight(300);tabla->setShowGrid(false);tabla->setAlternatingRowColors(true);tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);tabla->setSelectionBehavior(QAbstractItemView::SelectRows);tabla->setCursor(Qt::PointingHandCursor);l->addWidget(tabla,1);
        pie=new QHBoxLayout;l->addLayout(pie);contador=texto("","caption");pie->addWidget(contador,1);anterior=new QPushButton("← Anterior");siguiente=new QPushButton("Siguiente →");pie->addWidget(anterior);pie->addWidget(siguiente);
        connect(buscar,&QLineEdit::textChanged,this,[this](){actualizar();});for(auto *c:{admin,tipo})connect(c,&QComboBox::currentTextChanged,this,[this](){actualizar();});
        for(auto *c:{desde,hasta})connect(c,&QDateEdit::dateChanged,this,[this](){actualizar();});
        connect(limpiar,&QPushButton::clicked,this,[this](){buscar->clear();admin->setCurrentIndex(0);tipo->setCurrentIndex(0);desde->setDate(QDate::currentDate().addDays(-30));hasta->setDate(QDate::currentDate());actualizar();});
        connect(anterior,&QPushButton::clicked,this,[this](){--pagina;actualizar(false);});connect(siguiente,&QPushButton::clicked,this,[this](){++pagina;actualizar(false);});
        connect(tabla,&QTableWidget::cellClicked,this,[this](int r,int){detalle(r);});actualizar();adaptar(width());
    }
};
