#pragma once
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
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

