#pragma once
#include <QDateEdit>
#include <QCalendarWidget>
#include <QAbstractItemView>
#include <QMouseEvent>
#include <QPainter>
#include <QLocale>
#include <QTextCharFormat>

// Resaltado compartido para los calendarios de clientes y administradores.
class CalendarioNacimiento : public QCalendarWidget {
    QPoint puntero{-1,-1};
public:
    explicit CalendarioNacimiento(QWidget *parent=nullptr):QCalendarWidget(parent) {
        setMinimumSize(360,300);
        setFirstDayOfWeek(Qt::Monday);
        setVerticalHeaderFormat(NoVerticalHeader);
        setHorizontalHeaderFormat(ShortDayNames);
        QTextCharFormat finSemana;finSemana.setForeground(QColor("#ed916f"));
        setWeekdayTextFormat(Qt::Saturday,finSemana);setWeekdayTextFormat(Qt::Sunday,finSemana);
        if(auto *vista=findChild<QAbstractItemView*>()) {
            vista->setMouseTracking(true);vista->viewport()->setMouseTracking(true);
            vista->viewport()->installEventFilter(this);
        }
    }
protected:
    bool eventFilter(QObject *obj,QEvent *evento) override {
        if(evento->type()==QEvent::MouseMove) {
            puntero=static_cast<QMouseEvent*>(evento)->pos();updateCells();
        } else if(evento->type()==QEvent::Leave) {puntero=QPoint(-1,-1);updateCells();}
        return QCalendarWidget::eventFilter(obj,evento);
    }
    void paintCell(QPainter *p,const QRect &r,QDate fecha) const override {
        QCalendarWidget::paintCell(p,r,fecha);
        if(r.contains(puntero)&&fecha>=minimumDate()&&fecha<=maximumDate()) {
            p->save();p->setRenderHint(QPainter::Antialiasing);
            p->setPen(QPen(QColor("#ed916f"),2));
            p->setBrush(QColor(237,145,111,55));
            p->drawRoundedRect(r.adjusted(2,2,-2,-2),5,5);p->restore();
        }
    }
};
inline QDateEdit *crearCampoNacimiento(QWidget *parent=nullptr) {
    auto *fecha=new QDateEdit(QDate(2000,1,1),parent);
    fecha->setCalendarPopup(true);
    fecha->setCalendarWidget(new CalendarioNacimiento);
    fecha->setLocale(QLocale(QLocale::Spanish,QLocale::Mexico));
    fecha->setDisplayFormat("dd/MM/yyyy");
    fecha->setMaximumDate(QDate::currentDate());
    fecha->setAccessibleName("Fecha de nacimiento");
    return fecha;
}
