#pragma once
#include "SelectorDesplegable.h"
#include <QWidget>
#include <QGridLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QResizeEvent>
class FiltrosClientes : public QWidget {
    QGridLayout *rejilla;QList<QWidget*> grupos;int columnas=0;
    void adaptar(int ancho){
        int n=ancho>=950?4:ancho>=500?2:1;if(n==columnas)return;
        for(auto *g:grupos)rejilla->removeWidget(g);
        for(int c=0;c<4;++c)rejilla->setColumnStretch(c,c<n?1:0);
        for(int i=0;i<grupos.size();++i)rejilla->addWidget(grupos[i],i/n,i%n);
        columnas=n;
    }
protected:
    void resizeEvent(QResizeEvent *e) override {QWidget::resizeEvent(e);adaptar(e->size().width());}
public:
    QComboBox *estado,*orden,*simulaciones;QPushButton *limpiar;
    QSize minimumSizeHint() const override{return QSize(240,80);}
    FiltrosClientes(QWidget *parent=nullptr):QWidget(parent){
        setObjectName("page");rejilla=new QGridLayout(this);rejilla->setContentsMargins(0,0,0,0);rejilla->setSpacing(12);rejilla->setSizeConstraint(QLayout::SetNoConstraint);
        estado=new SelectorDesplegable;estado->setObjectName("filtroEstadoClientes");estado->addItems({"Todos","Activo","Inactivo","Deshabilitada"});
        orden=new SelectorDesplegable;orden->setObjectName("ordenClientes");orden->addItems({"Nombre A–Z","Nombre Z–A","Acceso más reciente","Acceso más antiguo","Más simulaciones","Menos simulaciones"});
        simulaciones=new SelectorDesplegable;simulaciones->setObjectName("filtroSimulacionesClientes");simulaciones->addItems({"Todos","Con simulaciones","Sin simulaciones"});
        limpiar=new QPushButton("Limpiar filtros");limpiar->setCursor(Qt::PointingHandCursor);
        for(auto *c:{estado,orden,simulaciones}){c->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);c->setMinimumContentsLength(10);c->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);}
        QList<QWidget*> controles={estado,orden,simulaciones,limpiar};QStringList titulos={"Estado","Ordenar por","Simulaciones",""};
        for(int i=0;i<4;++i){auto *g=new QWidget;g->setObjectName("activityFilterGroup");auto *col=new QVBoxLayout(g);col->setContentsMargins(0,0,0,0);col->setSpacing(6);
            auto *etiqueta=new QLabel(titulos[i]);etiqueta->setObjectName("caption");etiqueta->setFixedHeight(20);etiqueta->setBuddy(controles[i]);controles[i]->setFixedHeight(40);controles[i]->setAccessibleName(titulos[i].isEmpty()?"Limpiar filtros":titulos[i]);
            col->addWidget(etiqueta);col->addWidget(controles[i]);grupos.append(g);
        }adaptar(240);
    }
};
