#pragma once
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>

// Emblema vectorial basado en el boceto: bolsa, moneda y dos cuernos.
inline QIcon iconoFinancionanti()
{
    QIcon icono;
    for(int lado : {32,64,128,256}) {
        QPixmap pix(lado,lado); pix.fill(Qt::transparent);
        QPainter p(&pix); p.setRenderHint(QPainter::Antialiasing);
        p.scale(lado/100.0,lado/100.0);
        p.setPen(QPen(QColor("#ed916f"),3,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        p.setBrush(QColor("#33241f"));
        QPainterPath cuernos;
        cuernos.moveTo(35,31); cuernos.cubicTo(17,29,8,19,6,7);
        cuernos.cubicTo(16,18,24,18,38,20); cuernos.closeSubpath();
        cuernos.moveTo(65,31); cuernos.cubicTo(83,29,92,19,94,7);
        cuernos.cubicTo(84,18,76,18,62,20); cuernos.closeSubpath();
        p.drawPath(cuernos);
        p.setBrush(QColor("#ed916f"));
        p.drawEllipse(QRectF(30,13,40,40));
        p.setPen(QPen(QColor("#33241f"),2.6,Qt::SolidLine,Qt::RoundCap));
        QPainterPath peso;
        peso.moveTo(58,25); peso.cubicTo(39,18,37,33,50,33);
        peso.cubicTo(65,33,59,47,42,40);
        p.drawPath(peso); p.drawLine(50,20,50,45);
        p.setPen(QPen(QColor("#ed916f"),3,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        p.setBrush(QColor("#33241f"));
        QPainterPath bolsa;
        bolsa.moveTo(35,57); bolsa.cubicTo(33,50,26,47,25,46);
        bolsa.quadTo(50,52,75,46); bolsa.quadTo(65,52,65,57);
        bolsa.cubicTo(83,69,87,87,72,92);
        bolsa.quadTo(50,99,28,92); bolsa.cubicTo(13,87,17,69,35,57);
        bolsa.closeSubpath(); p.drawPath(bolsa);
        p.drawLine(34,57,66,57);
        p.drawLine(44,65,40,77); p.drawLine(57,65,61,74);
        p.end(); icono.addPixmap(pix);
    }
    return icono;
}

inline QLabel *logoFinancionanti(int lado)
{
    auto *logo=new QLabel;
    logo->setFixedSize(lado,lado);
    logo->setPixmap(iconoFinancionanti().pixmap(lado*2,lado*2).scaled(lado,lado,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    logo->setAccessibleName("Logo FinancionantiMex: bolsa con moneda y cuernos");
    logo->setToolTip("FinancionantiMex");
    return logo;
}
