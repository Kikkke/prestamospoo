#pragma once
#include <QAction>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>

inline QIcon iconoVisibilidad(bool visible)
{
    QPixmap pix(48,48); pix.fill(Qt::transparent);
    QPainter p(&pix); p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#ed916f"),2.5,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    QPainterPath ojo;
    ojo.moveTo(5,24); ojo.quadTo(24,3,43,24); ojo.quadTo(24,45,5,24);
    p.drawPath(ojo); p.drawEllipse(QRectF(18,18,12,12));
    if(visible) p.drawLine(9,8,39,40);
    return QIcon(pix);
}

inline void agregarVisibilidad(QLineEdit *campo)
{
    campo->setEchoMode(QLineEdit::Password);
    auto *accion=campo->addAction(iconoVisibilidad(false),QLineEdit::TrailingPosition);
    accion->setText("Mostrar contraseña"); accion->setToolTip(accion->text());
    accion->setCheckable(true);
    QObject::connect(accion,&QAction::toggled,campo,[campo,accion](bool mostrar) {
        campo->setEchoMode(mostrar?QLineEdit::Normal:QLineEdit::Password);
        accion->setIcon(iconoVisibilidad(mostrar));
        accion->setText(mostrar?"Ocultar contraseña":"Mostrar contraseña");
        accion->setToolTip(accion->text());
    });
}
