#pragma once
#include "AvatarUsuario.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>
#include <QSlider>
#include <QFrame>
#include <QScrollArea>
#include <QScreen>
#include <functional>
inline QPushButton *botonFoto(QWidget *parent=nullptr){
    auto *b=new QPushButton(parent);b->setFixedSize(34,34);b->setObjectName("primary");b->setToolTip("Cambiar foto de perfil");b->setAccessibleName(b->toolTip());b->setCursor(Qt::PointingHandCursor);
    QPixmap icono(24,24);icono.fill(Qt::transparent);QPainter p(&icono);p.setRenderHint(QPainter::Antialiasing);p.setPen(QPen(QColor("#17100e"),1.8));p.setBrush(Qt::NoBrush);p.drawRoundedRect(QRectF(3,7,18,13),2,2);p.drawEllipse(QRectF(8,10,8,8));p.drawRect(QRectF(8,4,8,3));p.end();b->setIcon(QIcon(icono));b->setIconSize(QSize(22,22));return b;
}
inline QWidget *avatarEditable(AvatarCliente *avatar,QPushButton *boton){
    auto *grupo=new QWidget;grupo->setObjectName("topicIcon");grupo->setFixedSize(avatar->width()+10,avatar->height()+10);
    avatar->setParent(grupo);avatar->move(0,0);boton->setParent(grupo);boton->move(grupo->width()-34,grupo->height()-34);return grupo;
}
inline bool editarFoto(QWidget *padre,QPixmap &foto,const std::function<bool()> &autorizar){
    QDialog d(padre);d.setWindowTitle("Foto de perfil");d.setMinimumWidth(460);
    d.setStyleSheet(R"(QSlider { background: transparent; min-height: 28px; } QSlider::groove:horizontal { height: 6px; background: #494047; border-radius: 3px; } QSlider::sub-page:horizontal { background: #ed916f; border-radius: 3px; } QSlider::handle:horizontal { background: #fff2ec; border: 2px solid #ed916f; width: 18px; margin: -8px 0; border-radius: 10px; } QSlider::handle:horizontal:hover { background: #ed916f; } QSlider::handle:horizontal:disabled { background: #777077; border-color: #777077; } QSlider::sub-page:horizontal:disabled { background: #494047; })");
    auto *raiz=new QVBoxLayout(&d);raiz->setContentsMargins(0,0,0,0);
    auto *scroll=new QScrollArea;scroll->setWidgetResizable(true);scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    auto *contenido=new QWidget;contenido->setObjectName("page");scroll->setWidget(contenido);raiz->addWidget(scroll);
    d.resize(500,qMin(740,d.screen()->availableGeometry().height()-80));
    auto *l=new QVBoxLayout(contenido);l->setContentsMargins(24,24,24,24);l->setSpacing(16);
    auto *titulo=new QLabel("Tu foto de perfil");titulo->setObjectName("title");l->addWidget(titulo);
    auto *ayuda=new QLabel("Selecciona una imagen y ajusta el encuadre. Los cambios son temporales.");ayuda->setWordWrap(true);ayuda->setObjectName("muted");l->addWidget(ayuda);
    auto *preview=new AvatarCliente;preview->setFixedSize(180,180);l->addWidget(preview,0,Qt::AlignCenter);
    QPixmap original=foto,resultado=foto;preview->setFoto(resultado);
    auto *zoom=new QSlider(Qt::Horizontal);zoom->setRange(100,300);zoom->setValue(100);
    auto *horizontal=new QSlider(Qt::Horizontal);horizontal->setRange(0,100);horizontal->setValue(50);
    auto *vertical=new QSlider(Qt::Horizontal);vertical->setRange(0,100);vertical->setValue(50);
    auto *panel=new QFrame;panel->setObjectName("card");auto *ajustes=new QVBoxLayout(panel);ajustes->setContentsMargins(18,14,18,14);ajustes->setSpacing(6);l->addWidget(panel);
    QList<QLabel*> valores;
    for(auto par:QList<QPair<QString,QSlider*>>{{"Acercamiento",zoom},{"Izquierda / derecha",horizontal},{"Arriba / abajo",vertical}}){
        auto *fila=new QHBoxLayout;fila->addWidget(new QLabel(par.first),1);auto *valor=new QLabel;valor->setObjectName("badge");valores.append(valor);fila->addWidget(valor);ajustes->addLayout(fila);ajustes->addWidget(par.second);
        par.second->setAccessibleName(par.first);par.second->setTracking(true);par.second->setSingleStep(1);par.second->setPageStep(10);par.second->setCursor(Qt::PointingHandCursor);
    }
    auto *aviso=new QLabel;aviso->setObjectName("caption");aviso->setWordWrap(true);ajustes->addWidget(aviso);

    auto actualizar=[&](){
        for(auto *s:{zoom,horizontal,vertical})s->setEnabled(!original.isNull());
        valores[0]->setText(QString::number(zoom->value())+" %");valores[1]->setText(QString::number(horizontal->value())+" %");valores[2]->setText(QString::number(vertical->value())+" %");
        if(original.isNull()){resultado=QPixmap();preview->setFoto(resultado);aviso->setText("Selecciona una foto para activar el encuadre.");return;}
        int lado=qMax(1,qMin(original.width(),original.height())*100/zoom->value());
        horizontal->setEnabled(original.width()>lado);vertical->setEnabled(original.height()>lado);
        aviso->setText((!horizontal->isEnabled()||!vertical->isEnabled())?"Aumenta el acercamiento para poder desplazar la foto en ambos sentidos.":"Arrastra los controles para ajustar el recorte. La vista previa cambia al instante.");
        int x=(original.width()-lado)*horizontal->value()/100,y=(original.height()-lado)*vertical->value()/100;
        resultado=original.copy(x,y,lado,lado).scaled(512,512,Qt::KeepAspectRatio,Qt::SmoothTransformation);preview->setFoto(resultado);
    };
    for(auto *s:{zoom,horizontal,vertical})QObject::connect(s,&QSlider::valueChanged,&d,[&](int){actualizar();});
    auto *seleccionar=new QPushButton("Seleccionar imagen");l->addWidget(seleccionar);
    QObject::connect(seleccionar,&QPushButton::clicked,&d,[&](){
        QString ruta=QFileDialog::getOpenFileName(&d,"Seleccionar foto",{},"Imágenes (*.png *.jpg *.jpeg *.webp *.bmp)");
        if(ruta.isEmpty())return;QImageReader lector(ruta);lector.setAutoTransform(true);
        if(lector.size().width()>12000||lector.size().height()>12000){QMessageBox::warning(&d,"Imagen demasiado grande","Selecciona una imagen de hasta 12000 píxeles por lado.");return;}
        QImage imagen=lector.read();if(imagen.isNull()){QMessageBox::warning(&d,"Imagen no válida","No se pudo abrir esta imagen.");return;}
        original=QPixmap::fromImage(imagen.scaled(2048,2048,Qt::KeepAspectRatio,Qt::SmoothTransformation));zoom->setValue(120);horizontal->setValue(50);vertical->setValue(50);actualizar();
    });
    auto *restaurar=new QPushButton("Restablecer icono predeterminado");l->addWidget(restaurar);QObject::connect(restaurar,&QPushButton::clicked,&d,[&](){original=QPixmap();actualizar();});
    auto *acciones=new QHBoxLayout;l->addLayout(acciones);auto *cancelar=new QPushButton("Cancelar");auto *guardar=new QPushButton("Guardar foto");guardar->setObjectName("primary");guardar->setDefault(true);cancelar->setAutoDefault(false);acciones->addWidget(cancelar);acciones->addWidget(guardar);
    QObject::connect(cancelar,&QPushButton::clicked,&d,&QDialog::reject);
    QObject::connect(guardar,&QPushButton::clicked,&d,[&](){if(autorizar()){foto=resultado;d.accept();}});
    for(auto *b:{seleccionar,restaurar})b->setAutoDefault(false);
    actualizar();return d.exec()==QDialog::Accepted;
}
