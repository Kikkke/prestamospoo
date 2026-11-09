#pragma once
#include "ContenidoAyuda.h"
#include "FondoAcceso.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QResizeEvent>

// Iconos vectoriales: mantienen su nitidez con cualquier escala de pantalla.
class IconoTema : public QWidget
{
public:
    explicit IconoTema(int tipo, QWidget *parent = nullptr) : QWidget(parent), tipo(tipo)
    { setObjectName("topicIcon"); setFixedSize(40, 40); setAttribute(Qt::WA_TransparentForMouseEvents); }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(QColor("#ed916f"), 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        switch (tipo) {
        case 0: // Información.
            p.drawEllipse(QRectF(5,5,30,30)); p.drawLine(20,18,20,28); p.drawPoint(20,12); break;
        case 1: // Recorrido.
            p.drawRoundedRect(QRectF(4,6,32,25),3,3); p.drawLine(4,13,36,13);
            p.drawLine(13,19,27,19); p.drawLine(23,16,27,19); p.drawLine(23,22,27,19); break;
        case 2: // Porcentaje.
            p.drawEllipse(QRectF(6,6,9,9)); p.drawEllipse(QRectF(25,25,9,9)); p.drawLine(9,32,31,8); break;
        case 3: // Calculadora.
            p.drawRoundedRect(QRectF(8,3,24,34),3,3); p.drawRect(QRectF(12,8,16,7));
            for(int y=21;y<=31;y+=10) for(int x=13;x<=27;x+=7) p.drawPoint(x,y); break;
        case 4: // Variación.
            p.drawLine(5,34,35,34); p.drawLine(5,34,5,6);
            p.drawPolyline(QPolygonF{QPointF(10,27),QPointF(18,22),QPointF(24,25),QPointF(34,10)});
            p.drawLine(27,10,34,10); p.drawLine(34,10,34,17); break;
        case 5: // Tabla.
            p.drawRoundedRect(QRectF(4,6,32,28),3,3); p.drawLine(4,15,36,15);
            p.drawLine(4,24,36,24); p.drawLine(15,6,15,34); p.drawLine(26,6,26,34); break;
        case 6: // Libro.
            p.drawPolyline(QPolygonF{QPointF(20,9),QPointF(5,5),QPointF(5,31),QPointF(20,35),QPointF(35,31),QPointF(35,5),QPointF(20,9),QPointF(20,35)});
            p.drawLine(9,14,15,16); p.drawLine(25,16,31,14); break;
        default: // Enlaces.
            p.drawRoundedRect(QRectF(4,15,20,15),6,6); p.drawRoundedRect(QRectF(16,6,20,15),6,6);
            p.drawLine(14,24,26,14); break;
        }
    }
private:
    int tipo;
};

class RejillaTemas : public QWidget
{
public:
    explicit RejillaTemas(QWidget *parent = nullptr) : QWidget(parent)
    {
        setObjectName("helpContent");
        grid = new QGridLayout(this);
        grid->setContentsMargins(0,0,0,0);
        grid->setSpacing(16);
        grid->setSizeConstraint(QLayout::SetNoConstraint);
    }
    QSize minimumSizeHint() const override { return QSize(0,grid->minimumSize().height()); }
    void agregar(QPushButton *b) { botones.append(b); distribuir(); }
protected:
    void resizeEvent(QResizeEvent *event) override { QWidget::resizeEvent(event); distribuir(); }
private:
    void distribuir()
    {
        const int columnas = width() >= 870 ? 3 : 2;
        for (int i=0;i<botones.size();++i) grid->addWidget(botones[i],i/columnas,i%columnas);
        for (int i=0;i<3;++i) grid->setColumnStretch(i,i<columnas ? 1 : 0);
    }
    QGridLayout *grid;
    QList<QPushButton *> botones;
};

class PantallaAyuda : public QWidget
{
public:
    explicit PantallaAyuda(QWidget *parent = nullptr) : QWidget(parent)
    {
        auto *raiz = new QVBoxLayout(this);
        raiz->setContentsMargins(0,0,0,0);
        auto *vistas = new QStackedWidget;
        raiz->addWidget(vistas);
        auto nuevaPagina = [vistas]() {
            auto *fondo = new FondoAcceso;
            fondo->setObjectName("page");
            auto *layout = new QVBoxLayout(fondo);
            layout->setContentsMargins(24,24,24,24);
            layout->setSpacing(16);
            auto *scroll = new QScrollArea;
            scroll->setWidgetResizable(true);
            scroll->setWidget(fondo);
            vistas->addWidget(scroll);
            return layout;
        };
        const QStringList titulos = {"Sobre el simulador", "Cómo usar el software", "Tasa de interés", "Ejemplo de préstamo",
                                    "Monto, tasa y plazo", "Tabla de amortización", "Glosario financiero", "Fuentes y consultas"};
        const QStringList resumenes = {
            "Conoce el objetivo, el alcance y las funciones disponibles en este prototipo.",
            "Recorre el inicio, el historial y el comparador paso a paso.",
            "Entiende cómo un porcentaje se convierte en intereses en pesos.",
            "Sigue un préstamo de $12,000 durante tres meses, pago por pago.",
            "Descubre qué cambia en tus pagos al modificar estas condiciones.",
            "Aprende a leer el saldo, los intereses y el abono a capital.",
            "Consulta 32 conceptos explicados con lenguaje sencillo y ejemplos.",
            "Amplía tus conocimientos con materiales de Banxico y CONDUSEF."};
        auto *indice = nuevaPagina();
        indice->addWidget(label("ACERCA DE", "eyebrow"));
        indice->addWidget(label("Aprende sobre tu préstamo", "title"));
        indice->addWidget(label("Elige un tema para consultar su explicación. Puedes pulsar cualquier parte de la tarjeta.", "muted"));
        auto *rejilla = new RejillaTemas;
        indice->addWidget(rejilla);
        indice->addStretch();
        const auto guia = guiaPrestamos();
        for (int i=0;i<titulos.size();++i) {
            auto *b = new QPushButton;
            b->setObjectName("topicCard");
            b->setAccessibleName(titulos[i]);
            b->setAccessibleDescription(resumenes[i]);
            b->setCursor(Qt::PointingHandCursor);
            b->setMinimumWidth(245);
            b->setFixedHeight(232);
            b->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
            auto *box = new QVBoxLayout(b);
            box->setContentsMargins(20,20,20,18);
            box->setSpacing(12);
            auto *cabecera = new QHBoxLayout;
            auto *titulo = label(titulos[i],"section");
            titulo->setAttribute(Qt::WA_TransparentForMouseEvents);
            cabecera->addWidget(titulo,1);
            cabecera->addWidget(new IconoTema(i),0,Qt::AlignTop);
            box->addLayout(cabecera);
            auto *resumen = label(resumenes[i],"muted");
            resumen->setAttribute(Qt::WA_TransparentForMouseEvents);
            box->addWidget(resumen);
            box->addStretch();
            auto *mas = label("Conocer más  →", "topicLink");
            mas->setAttribute(Qt::WA_TransparentForMouseEvents);
            box->addWidget(mas);
            rejilla->agregar(b);

            auto *detalle = nuevaPagina();
            auto *volver = new QPushButton("← Volver a los temas");
            volver->setCursor(Qt::PointingHandCursor);
            detalle->addWidget(volver,0,Qt::AlignLeft);
            detalle->addWidget(label(titulos[i],"title"));
            auto *card = new QFrame;
            card->setObjectName("card");
            auto *textoLayout = new QVBoxLayout(card);
            textoLayout->setContentsMargins(24,24,24,24);
            textoLayout->setSpacing(16);
            detalle->addWidget(card);
            if(i<guia.size()) textoLayout->addWidget(parrafo(guia[i].second));
            else if(i==6) {
                for (const auto &termino : glosarioPrestamos()) {
                    textoLayout->addWidget(label(termino.first,"section"));
                    textoLayout->addWidget(parrafo(termino.second));
                }
            } else {
                textoLayout->addWidget(parrafo("Conceptos contrastados con materiales de educación financiera de Banco de México y CONDUSEF. Los ejemplos numéricos fueron calculados para esta guía; no son ofertas de una institución."));
                const QStringList nombres={"Banco de México · Glosario financiero","CONDUSEF · Aprende sobre el crédito","Banco de México · Explicaciones del CAT"};
                const QStringList urls={"https://contigo.banxico.org.mx/glosario.html","https://webappsos.condusef.gob.mx/EducaTuCartera/credito.html","https://www.banxico.org.mx/CAT/Explicaciones.html"};
                for(int j=0;j<urls.size();++j) {
                    auto *link=label(QString("<a href='%1' style='color:#ed916f'>%2</a>").arg(urls[j],nombres[j]));
                    link->setOpenExternalLinks(true);
                    link->setTextInteractionFlags(Qt::TextBrowserInteraction);
                    textoLayout->addWidget(link);
                }
            }
            detalle->addStretch();
            connect(b,&QPushButton::clicked,this,[vistas,i]() {
                vistas->setCurrentIndex(i+1);
                auto *scroll=qobject_cast<QScrollArea *>(vistas->currentWidget());
                scroll->verticalScrollBar()->setValue(0);
            });
            connect(volver,&QPushButton::clicked,this,[vistas,b]() { vistas->setCurrentIndex(0); b->setFocus(); });
        }
    }
private:
    static QLabel *label(const QString &t,const char *tipo="") {
        auto *l=new QLabel(t); l->setWordWrap(true); l->setObjectName(tipo); return l;
    }
    static QLabel *parrafo(const QString &t) {
        auto *l=label(t); l->setTextFormat(Qt::PlainText);
        l->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
        return l;
    }
};
