#pragma once
#include "FondoAcceso.h"
#include "MarcaFinancionanti.h"
#include "PantallaAyuda.h"
#include <QDialog>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QFileDialog>
#include <QPdfWriter>
#include <QMessageBox>
#include <QDate>
#include <QLocale>
#include <QResizeEvent>
#include <cmath>
class VentanaAmortizacion : public QDialog {
    struct Pago {double inicial,pago,interes,capital,final;};
    QList<Pago> pagos;QString nombre;double monto,tasa,intereses=0,total=0;int meses;
    QBoxLayout *metricas;
    static QString dinero(double n){return QLocale(QLocale::Spanish,QLocale::Mexico).toString(n,'f',2);}
    static QLabel *texto(QString s,const char *tipo=""){auto *l=new QLabel(s);l->setTextFormat(Qt::PlainText);l->setObjectName(tipo);l->setWordWrap(true);return l;}
    void exportar(){
        QString archivo=QFileDialog::getSaveFileName(this,"Exportar amortización","Amortizacion.pdf","PDF (*.pdf)");
        if(archivo.isEmpty())return;if(!archivo.endsWith(".pdf",Qt::CaseInsensitive))archivo+=".pdf";
        QPdfWriter pdf(archivo);pdf.setResolution(96);pdf.setPageSize(QPageSize(QPageSize::A4));pdf.setPageMargins(QMarginsF(15,15,15,15));
        QPainter p;if(!p.begin(&pdf)){QMessageBox::warning(this,"Exportación","No se pudo escribir el PDF.");return;}
        const int ancho=pdf.width(),alto=pdf.height();const int porPagina=22;int paginas=(pagos.size()+porPagina-1)/porPagina;
        for(int pagina=0;pagina<paginas;++pagina){
            if(pagina&&!pdf.newPage()){p.end();QMessageBox::warning(this,"Exportación","No se pudo completar el PDF.");return;}
            p.fillRect(QRect(0,0,ancho,alto),Qt::white);p.setPen(QColor("#222222"));
            p.drawPixmap(0,0,50,50,iconoFinancionanti().pixmap(100,100));
            p.setFont(QFont("Segoe UI",16,QFont::Bold));p.drawText(65,27,"FinancionantiMex");
            p.setFont(QFont("Segoe UI",11));p.drawText(QRect(0,62,ancho,42),Qt::TextWordWrap,nombre+" · Tabla de amortización");
            p.setFont(QFont("Segoe UI",9));p.drawText(0,120,"Fecha de emisión: "+QDate::currentDate().toString("dd/MM/yyyy")+" · EJEMPLO ILUSTRATIVO");
            p.drawText(0,143,QString("Monto: $%1 MXN | Tasa anual: %2 % | %3 pagos mensuales").arg(dinero(monto)).arg(tasa).arg(meses));
            p.drawText(0,166,QString("Pago regular: $%1 | Intereses: $%2 | Total: $%3").arg(dinero(pagos.first().pago),dinero(intereses),dinero(total)));
            QStringList encabezados={"Pago","Saldo inicial","Mensualidad","Intereses","Capital","Saldo final"};
            int y=190,cw=ancho/6;
            p.fillRect(0,y,ancho,30,QColor("#ed916f"));p.setFont(QFont("Segoe UI",8,QFont::Bold));
            for(int c=0;c<6;++c)p.drawText(QRect(c*cw,y,cw-4,30),Qt::AlignRight|Qt::AlignVCenter,encabezados[c]);
            y+=30;p.setFont(QFont("Segoe UI",8));
            for(int n=pagina*porPagina;n<qMin(int(pagos.size()),(pagina+1)*porPagina);++n){
                const auto &r=pagos[n];if(n%2==0)p.fillRect(0,y,ancho,25,QColor("#f3f1f0"));
                QStringList valores={QString::number(n+1),dinero(r.inicial),dinero(r.pago),dinero(r.interes),dinero(r.capital),dinero(r.final)};
                for(int c=0;c<6;++c)p.drawText(QRect(c*cw,y,cw-4,25),Qt::AlignRight|Qt::AlignVCenter,valores[c]);y+=25;
            }
            if(pagina==paginas-1){p.setFont(QFont("Segoe UI",9,QFont::Bold));p.drawText(0,y+28,QString("Totales: pagos $%1 · intereses $%2 · capital $%3").arg(dinero(total),dinero(intereses),dinero(monto)));}
            p.setFont(QFont("Segoe UI",8));p.drawText(QRect(0,alto-65,ancho,40),Qt::TextWordWrap,"Importes en MXN. Tasa nominal anual / 12, cuota fija sobre saldo insoluto. Sin comisiones, seguros ni impuestos. La última cuota ajusta el redondeo.");
            p.drawText(QRect(0,alto-23,ancho,23),Qt::AlignRight,QString("Página %1 de %2").arg(pagina+1).arg(paginas));
        }
        p.end();QMessageBox::information(this,"PDF exportado","Se guardó la tabla completa en:\n"+archivo);
    }
protected:
    void resizeEvent(QResizeEvent *e) override {QDialog::resizeEvent(e);if(metricas)metricas->setDirection(width()<700?QBoxLayout::TopToBottom:QBoxLayout::LeftToRight);}
public:
    VentanaAmortizacion(QString n,double m,double t,int plazo,QWidget *parent=nullptr):QDialog(parent),nombre(n),monto(m),tasa(t),meses(plazo),metricas(nullptr){
        setWindowTitle("Amortización · "+nombre);resize(1000,760);
        double saldo=monto,r=tasa/1200.0,cuota=r==0?monto/meses:monto*r/(1-std::pow(1+r,-meses));
        auto centavos=[](double v){return std::round(v*100)/100;};cuota=centavos(cuota);
        for(int i=0;i<meses;++i){double interes=centavos(saldo*r),capital=i==meses-1?saldo:qMin(saldo,centavos(cuota-interes)),pago=centavos(capital+interes);
            pagos.append({saldo,pago,interes,capital,centavos(saldo-capital)});saldo=pagos.last().final;intereses+=interes;total+=pago;}
        auto *raiz=new QVBoxLayout(this);raiz->setContentsMargins(0,0,0,0);auto *fondo=new FondoAcceso;raiz->addWidget(fondo);auto *l=new QVBoxLayout(fondo);l->setContentsMargins(24,24,24,24);l->setSpacing(14);
        l->addWidget(texto(nombre,"title"));l->addWidget(texto("TABLA DE AMORTIZACIÓN · EJEMPLO ILUSTRATIVO","eyebrow"));
        l->addWidget(texto(QString("Monto: $%1 MXN · Tasa anual: %2 % · Plazo: %3 meses · Pagos mensuales").arg(dinero(monto)).arg(tasa).arg(meses),"muted"));
        metricas=new QHBoxLayout;l->addLayout(metricas);
        QStringList titulos={"Pago mensual regular","Intereses totales","Total por pagar"};QList<double> valores={cuota,intereses,total};
        for(int i=0;i<3;++i){auto *f=new QFrame;f->setObjectName("card");auto *c=new QVBoxLayout(f);c->setContentsMargins(16,12,16,12);c->addWidget(texto(titulos[i],"caption"));c->addWidget(texto("$"+dinero(valores[i]),i==0?"metricAccent":"metric"));metricas->addWidget(f);}
        auto *tabla=new QTableWidget(meses+1,6);tabla->setMouseTracking(true);tabla->setStyleSheet("QTableWidget::item:hover { background: #33241f; }");tabla->setHorizontalHeaderLabels({"Pago","Saldo inicial","Mensualidad","Intereses","Abono a capital","Saldo final"});tabla->verticalHeader()->hide();tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);tabla->setAlternatingRowColors(true);tabla->setShowGrid(false);
        tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);tabla->horizontalHeader()->setMinimumSectionSize(110);tabla->setMinimumHeight(200);
        tabla->horizontalHeaderItem(3)->setToolTip("Costo del periodo calculado sobre el saldo pendiente.");tabla->horizontalHeaderItem(4)->setToolTip("Parte del pago que reduce la deuda.");tabla->horizontalHeaderItem(5)->setToolTip("Deuda pendiente después del pago.");
        for(int i=0;i<meses;++i){auto r=pagos[i];QStringList fila={QString::number(i+1),dinero(r.inicial),dinero(r.pago),dinero(r.interes),dinero(r.capital),dinero(r.final)};for(int c=0;c<6;++c){auto *item=new QTableWidgetItem(fila[c]);item->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);tabla->setItem(i,c,item);}}
        QStringList sumas={"Totales","—",dinero(total),dinero(intereses),dinero(monto),"—"};
        for(int c=0;c<6;++c){auto *item=new QTableWidgetItem(sumas[c]);item->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);item->setForeground(QColor("#ed916f"));tabla->setItem(meses,c,item);}
        l->addWidget(tabla,1);l->addWidget(texto("MXN · Cuota fija, tasa nominal anual / 12. Sin comisiones, seguros ni impuestos. Última cuota ajustada por redondeo. Datos de muestra, sin fecha de guardado real.","caption"));
        auto *acciones=new QHBoxLayout;l->addLayout(acciones);auto *ayuda=new QPushButton("¿Cómo interpreto esta tabla?");acciones->addWidget(ayuda);acciones->addStretch();auto *pdf=new QPushButton("Exportar PDF");pdf->setObjectName("primary");acciones->addWidget(pdf);auto *cerrar=new QPushButton("Cerrar");acciones->addWidget(cerrar);
        connect(pdf,&QPushButton::clicked,this,[this](){exportar();});connect(cerrar,&QPushButton::clicked,this,&QDialog::accept);
        connect(ayuda,&QPushButton::clicked,this,[this](){QDialog d(this);d.resize(800,650);auto *l=new QVBoxLayout(&d);auto *a=new PantallaAyuda;l->addWidget(a);a->mostrarTablaAmortizacion();d.exec();});
    }
};
