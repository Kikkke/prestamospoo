#pragma once
#include <QComboBox>
#include <QCompleter>
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QPainter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <array>

// Catálogo independiente: resultados ilustrativos precargados, sin motor de cálculo.
class ComparadorSimulaciones : public QWidget
{
public:
    explicit ComparadorSimulaciones(QWidget *parent=nullptr) : QWidget(parent)
    {
        setObjectName("comparisonContent");
        auto *raiz=new QVBoxLayout(this);
        raiz->setContentsMargins(0,0,0,0); raiz->setSpacing(16);
        auto *opciones=new QHBoxLayout; opciones->setSpacing(16);
        raiz->addLayout(opciones);
        for(int i=0;i<2;++i) {
            auto *c=tarjeta(opciones);
            c->addWidget(etiqueta(i==0?"OPCIÓN A":"OPCIÓN B","eyebrow"));
            auto *titulo=etiqueta("Buscar y elegir simulación","caption");
            c->addWidget(titulo);
            selectores[i]=new QComboBox;
            auto *selector=selectores[i];
            selector->setEditable(true); selector->setInsertPolicy(QComboBox::NoInsert);
            selector->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Fixed);
            selector->setAccessibleName(i==0?"Simulación de la opción A":"Simulación de la opción B");
            titulo->setBuddy(selector);
            for(const auto &e:ejemplos) selector->addItem(QString::fromUtf8(e.nombre));
            selector->setCurrentIndex(i);
            selector->lineEdit()->addAction(icono(0),QLineEdit::LeadingPosition);
            selector->lineEdit()->setPlaceholderText("Escribe un nombre…");
            selector->completer()->setCompletionMode(QCompleter::PopupCompletion);
            selector->completer()->setFilterMode(Qt::MatchContains);
            selector->completer()->setCaseSensitivity(Qt::CaseInsensitive);
            c->addWidget(selector);
            c->addSpacing(8);
            c->addWidget(etiqueta("Mensualidad de muestra","muted"));
            cuotas[i]=etiqueta("","metricAccent"); c->addWidget(cuotas[i]);
            detalles[i]=etiqueta("","caption"); c->addWidget(detalles[i]);
            connect(selector,&QComboBox::currentTextChanged,this,[this]() { actualizar(); });
        }
        aviso=etiqueta("","comparisonNotice"); raiz->addWidget(aviso);
        auto *resumen=new QHBoxLayout; resumen->setSpacing(16); raiz->addLayout(resumen);
        const QStringList titulos={"Menor mensualidad","Menor total","Menor plazo"};
        for(int i=0;i<3;++i) {
            auto *c=tarjeta(resumen);
            auto *cabecera=new QHBoxLayout;
            auto *imagen=new QLabel; imagen->setPixmap(icono(i+1).pixmap(22,22));
            imagen->setFixedSize(24,24);
            cabecera->addWidget(imagen); cabecera->addWidget(etiqueta(titulos[i],"muted"),1);
            c->addLayout(cabecera);
            ganadores[i]=etiqueta("","section"); c->addWidget(ganadores[i]);
            diferencias[i]=etiqueta("","caption"); c->addWidget(diferencias[i]);
        }
        raiz->addWidget(etiqueta("Comparación al detalle","section"));
        tabla=new QTableWidget(6,3);
        tabla->setAccessibleName("Detalle de las dos simulaciones seleccionadas");
        tabla->setHorizontalHeaderLabels({"Concepto","Opción A","Opción B"});
        tabla->verticalHeader()->hide(); tabla->verticalHeader()->setDefaultSectionSize(42);
        tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tabla->setSelectionMode(QAbstractItemView::NoSelection);
        tabla->setFocusPolicy(Qt::NoFocus);
        tabla->setShowGrid(false); tabla->setAlternatingRowColors(true);
        tabla->setMinimumHeight(298);
        const QStringList conceptos={"Monto solicitado","Tasa nominal anual","Plazo","Mensualidad","Intereses totales","Total por pagar"};
        for(int r=0;r<6;++r) {
            tabla->setItem(r,0,new QTableWidgetItem(conceptos[r]));
            for(int col=1;col<3;++col) {
                auto *celda=new QTableWidgetItem;
                celda->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
                if(r>=3) { auto f=celda->font(); f.setBold(true); celda->setFont(f); }
                tabla->setItem(r,col,celda);
            }
        }
        raiz->addWidget(tabla);
        raiz->addWidget(etiqueta("MXN · Resultados precargados y redondeados. Sin comisiones, seguros ni impuestos. No se calculan ni guardan préstamos; este catálogo es independiente del historial.","caption"));
        actualizar();
    }
private:
    struct Ejemplo { const char *nombre; double monto,tasa; int meses; double cuota,intereses,total; };
    const std::array<Ejemplo,6> ejemplos{{
        {"Computadora",12000,0,3,4000,0,12000},
        {"Computadora a 6 meses",12000,0,6,2000,0,12000},
        {"Computadora a 12 meses",12000,12,12,1066.19,794.25,12794.25},
        {"Muebles a 12 meses",24000,0,12,2000,0,24000},
        {"Muebles a 24 meses",24000,0,24,1000,0,24000},
        {"Curso de idiomas",15000,0,10,1500,0,15000}
    }};
    static QLabel *etiqueta(const QString &s,const char *tipo) {
        auto *l=new QLabel(s); l->setObjectName(tipo); l->setWordWrap(true); return l;
    }
    static QVBoxLayout *tarjeta(QHBoxLayout *fila) {
        auto *f=new QFrame; f->setObjectName("card");
        auto *l=new QVBoxLayout(f); l->setContentsMargins(20,16,20,16); l->setSpacing(8);
        fila->addWidget(f,1); return l;
    }
    static QIcon icono(int tipo) {
        QPixmap pix(48,48); pix.fill(Qt::transparent);
        QPainter p(&pix); p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(QColor("#ed916f"),2.5,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        if(tipo==0) { p.drawEllipse(QRectF(9,9,22,22)); p.drawLine(28,28,39,39); }
        else if(tipo==3) {
            p.drawRoundedRect(QRectF(7,10,34,31),4,4); p.drawLine(7,20,41,20);
            p.drawLine(16,6,16,14); p.drawLine(32,6,32,14); p.drawLine(16,29,23,29);
        } else if(tipo==2) {
            p.drawRoundedRect(QRectF(6,12,36,27),4,4); p.drawRect(QRectF(28,20,14,11)); p.drawPoint(33,25);
        } else {
            p.drawEllipse(QRectF(6,6,36,36)); p.drawLine(24,13,24,34);
            p.drawLine(17,27,24,34); p.drawLine(24,34,31,27);
        }
        return QIcon(pix);
    }
    static QString dinero(double v) { return "$"+QLocale(QLocale::Spanish,QLocale::Mexico).toString(v,'f',2); }
    void actualizar() {
        // Mientras se escribe, no se muestran resultados de una selección anterior.
        if(!selectores[1] || !tabla) return;
        int indices[2];
        for(int i=0;i<2;++i) indices[i]=selectores[i]->findText(selectores[i]->currentText(),Qt::MatchFixedString);
        const bool validos=indices[0]>=0 && indices[1]>=0;
        for(int i=0;i<2;++i) {
            if(indices[i]<0) { cuotas[i]->setText("—"); detalles[i]->setText("Elige un ejemplo de la lista."); }
            else {
                const auto &e=ejemplos[indices[i]];
                cuotas[i]->setText(dinero(e.cuota));
                detalles[i]->setText(QString("MXN / mes · %1 meses · %2 % anual").arg(e.meses).arg(e.tasa));
            }
        }
        if(!validos) {
            aviso->setText("Selecciona un ejemplo de la lista o de las sugerencias para completar la comparación.");
            for(int r=0;r<6;++r) for(int c=1;c<3;++c) tabla->item(r,c)->setText("—");
            for(int i=0;i<3;++i) { ganadores[i]->setText("Pendiente"); diferencias[i]->setText("Elige las dos opciones."); }
            return;
        }
        const auto &a=ejemplos[indices[0]], &b=ejemplos[indices[1]];
        aviso->setText(indices[0]==indices[1]?"Elegiste el mismo ejemplo en ambas opciones. Selecciona otro para explorar diferencias."
            : a.monto!=b.monto?"Estas opciones tienen montos diferentes. Tenlo en cuenta al comparar sus pagos y totales."
            :"Mismo monto solicitado. Compara la mensualidad, el costo total y el tiempo para terminar de pagar.");
        for(int i=0;i<2;++i) {
            const auto &e=ejemplos[indices[i]];
            const QStringList valores={dinero(e.monto),QString::number(e.tasa)+" %",QString::number(e.meses)+" meses",dinero(e.cuota),dinero(e.intereses),dinero(e.total)};
            for(int r=0;r<6;++r) tabla->item(r,i+1)->setText(valores[r]);
        }
        const double va[]={a.cuota,a.total,double(a.meses)}, vb[]={b.cuota,b.total,double(b.meses)};
        for(int i=0;i<3;++i) {
            const double diferencia=qAbs(va[i]-vb[i]);
            ganadores[i]->setText(diferencia<0.005?(i==2?"Mismo plazo":"Mismo importe"):(va[i]<vb[i]?"Opción A":"Opción B"));
            diferencias[i]->setText(diferencia<0.005?"Sin diferencia entre opciones.":i==2?QString("%1 meses menos").arg(int(diferencia)):dinero(diferencia)+(i==0?" menos al mes":" menos en total"));
        }
    }
    QComboBox *selectores[2]={nullptr,nullptr};
    QLabel *cuotas[2], *detalles[2], *ganadores[3], *diferencias[3], *aviso;
    QTableWidget *tabla=nullptr;
};
