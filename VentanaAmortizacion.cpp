#include "VentanaAmortizacion.h"
#include "FondoAcceso.h"
#include "PantallaAyuda.h"
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <QResizeEvent>
#include <QLocale>

namespace {
QLabel *texto(const QString &s,const char *tipo="") {
    auto *l=new QLabel(s);l->setTextFormat(Qt::PlainText);l->setObjectName(tipo);l->setWordWrap(true);return l;
}
}
VentanaAmortizacion::VentanaAmortizacion(const Sesion &identidad,int id,QWidget *parent):DialogoGuardado(parent),sesion(identidad),simulacionId(id) {
    setWindowTitle("Amortización");resize(1000,760);setMinimumSize(620,550);
    auto *raiz=new QVBoxLayout(this);raiz->setContentsMargins(0,0,0,0);
    auto *scroll=new QScrollArea;scroll->setWidgetResizable(true);scroll->setFrameShape(QFrame::NoFrame);raiz->addWidget(scroll);
    auto *fondo=new FondoAcceso;scroll->setWidget(fondo);auto *l=new QVBoxLayout(fondo);l->setContentsMargins(24,24,24,24);l->setSpacing(14);
    titulo=texto("Tabla de amortización","title");l->addWidget(titulo);
    l->addWidget(texto("SIMULACIÓN GUARDADA · PAGOS MENSUALES","eyebrow"));
    condiciones=texto({},"muted");fechas=texto({},"caption");l->addWidget(condiciones);l->addWidget(fechas);
    metricas=new QHBoxLayout;l->addLayout(metricas);
    const QStringList titulos={"Primer pago","Intereses totales","Total por pagar"};
    for(int i=0;i<3;++i) {
        auto *f=new QFrame;f->setObjectName("card");auto *c=new QVBoxLayout(f);c->setContentsMargins(16,12,16,12);
        c->addWidget(texto(titulos[i],"caption"));auto *valor=texto("—",i==0?"metricAccent":"metric");c->addWidget(valor);cifras.append(valor);metricas->addWidget(f);
    }
    tabla=new QTableWidget(0,6);tabla->setObjectName("detalleAmortizacion");tabla->setMouseTracking(true);
    tabla->setStyleSheet("QTableWidget::item:hover { background: #33241f; }");tabla->setHorizontalHeaderLabels({"Pago","Saldo inicial","Mensualidad","Intereses","Abono a capital","Saldo final"});
    tabla->verticalHeader()->hide();tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);tabla->setAlternatingRowColors(true);tabla->setShowGrid(false);
    tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);tabla->horizontalHeader()->setMinimumSectionSize(130);tabla->setMinimumHeight(260);
    tabla->horizontalHeaderItem(3)->setToolTip("Interés guardado para ese periodo.");tabla->horizontalHeaderItem(4)->setToolTip("Parte del pago que reduce la deuda.");tabla->horizontalHeaderItem(5)->setToolTip("Deuda pendiente después del abono.");l->addWidget(tabla,1);
    nota=texto({},"caption");l->addWidget(nota);estado=texto({},"estadoAmortizacion");l->addWidget(estado);
    auto *acciones=new QHBoxLayout;l->addLayout(acciones);auto *ayuda=new QPushButton("¿Cómo interpreto esta tabla?");acciones->addWidget(ayuda);acciones->addStretch();
    actualizar=new QPushButton("Actualizar");actualizar->setObjectName("actualizarAmortizacion");acciones->addWidget(actualizar);
    pdf=new QPushButton("Exportar PDF");pdf->setObjectName("primary");pdf->setEnabled(false);acciones->addWidget(pdf);
    auto *cerrar=new QPushButton("Cerrar");acciones->addWidget(cerrar);
    for(auto *b:{ayuda,actualizar,pdf,cerrar})b->setAutoDefault(false);
    connect(cerrar,&QPushButton::clicked,this,&QDialog::reject);connect(actualizar,&QPushButton::clicked,this,[this]{actualizarDatos();});
    connect(pdf,&QPushButton::clicked,this,[this]{exportar();});
    connect(ayuda,&QPushButton::clicked,this,[this]{QDialog d(this);d.resize(800,650);auto *l=new QVBoxLayout(&d);auto *a=new PantallaAyuda;l->addWidget(a);a->mostrarTablaAmortizacion();d.exec();});
    auto *revisar=new QTimer(this);revisar->setInterval(5000);
    connect(revisar,&QTimer::timeout,this,[this]{if(isVisible())actualizarDatos(true);});revisar->start();
    QTimer::singleShot(0,this,[this]{actualizarDatos();});
}
void VentanaAmortizacion::resizeEvent(QResizeEvent *e) {
    DialogoGuardado::resizeEvent(e);if(metricas)metricas->setDirection(width()<780?QBoxLayout::TopToBottom:QBoxLayout::LeftToRight);
}
void VentanaAmortizacion::limpiar() {
    valido=false;pdf->setEnabled(false);titulo->setText("Amortización no disponible");condiciones->clear();fechas->clear();nota->clear();tabla->setRowCount(0);
    for(auto *valor:cifras)valor->setText("—");
}
void VentanaAmortizacion::actualizarDatos(bool silencioso) {
    if(ocupado)return;
    ocupado=true;actualizar->setEnabled(false);pdf->setEnabled(false);
    if(!silencioso)estado->setText("Consultando las cuotas guardadas…");
    auto *observador=new QFutureWatcher<ResultadoDetalle>(this);
    connect(observador,&QFutureWatcher<ResultadoDetalle>::finished,this,[=]{
        const auto r=observador->result();observador->deleteLater();ocupado=false;actualizar->setEnabled(true);
        if(!r.correcto()){limpiar();estado->setText(r.error);return;}
        const bool recuperado=!valido;mostrar(r.detalle);if(!silencioso || recuperado)estado->clear();
    });
    const auto identidad=sesion;const int id=simulacionId;
    observador->setFuture(QtConcurrent::run([identidad,id]{
        ConexionBD conexion;QString error;if(!conexion.abrir(error)){ResultadoDetalle r;r.error=error;return r;}
        return ServicioAmortizacion(conexion.base()).consultar(identidad,id);
    }));
}
void VentanaAmortizacion::mostrar(const DetalleAmortizacion &d) {
    valido=true;titulo->setText(d.nombre);setWindowTitle("Amortización · "+d.nombre);
    const QLocale mx(QLocale::Spanish,QLocale::Mexico);
    condiciones->setText(QString("Monto: $%1 MXN · Tasa anual: %2 % · Plazo: %3 meses").arg(MotorPrestamo::dinero(d.monto),mx.toString(d.tasaDiezmilesimas/10000.0,'f',d.tasaDiezmilesimas%100?4:2)).arg(d.meses));
    fechas->setText("Guardada: "+d.creada.toString("dd/MM/yyyy HH:mm:ss")+" · Actualizada: "+d.actualizada.toString("dd/MM/yyyy HH:mm:ss"));
    const QList<qint64> valores={d.cuotas.first().pago,d.intereses,d.total};for(int i=0;i<3;++i)cifras[i]->setText("$"+MotorPrestamo::dinero(valores[i]));
    const int vertical=tabla->verticalScrollBar()->value(),horizontal=tabla->horizontalScrollBar()->value();
    tabla->setRowCount(d.cuotas.size()+1);
    for(int i=0;i<=d.cuotas.size();++i) {
        QStringList fila;
        if(i==d.cuotas.size())fila={"Totales","—",MotorPrestamo::dinero(d.total),MotorPrestamo::dinero(d.intereses),MotorPrestamo::dinero(d.monto),"—"};
        else {const auto &r=d.cuotas[i];fila={QString::number(r.numero),MotorPrestamo::dinero(r.inicial),MotorPrestamo::dinero(r.pago),MotorPrestamo::dinero(r.interes),MotorPrestamo::dinero(r.capital),MotorPrestamo::dinero(r.final)};}
        for(int c=0;c<6;++c){auto *item=new QTableWidgetItem(fila[c]);item->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);if(i==d.cuotas.size())item->setForeground(QColor("#ed916f"));tabla->setItem(i,c,item);}
    }
    tabla->verticalScrollBar()->setValue(vertical);tabla->horizontalScrollBar()->setValue(horizontal);
    nota->setText("Último pago: $"+MotorPrestamo::dinero(d.cuotas.last().pago)+" MXN. Importes según las cuotas guardadas, sin comisiones, seguros ni impuestos.");pdf->setEnabled(true);
}
void VentanaAmortizacion::exportar() {
    if(ocupado || !valido)return;
    ocupado=true; // Evita consultas periódicas mientras el usuario elige el destino.
    QString ruta=QFileDialog::getSaveFileName(this,"Exportar amortización",QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).filePath("Amortizacion_"+QString::number(simulacionId)+".pdf"),"Archivos PDF (*.pdf)",nullptr,QFileDialog::DontConfirmOverwrite);
    if(ruta.isEmpty()){ocupado=false;return;}
    if(!ruta.endsWith(".pdf",Qt::CaseInsensitive))ruta+=".pdf";
    if(QFileInfo::exists(ruta) && QMessageBox::question(this,"Reemplazar PDF","Ya existe el archivo:\n"+ruta+"\n\n¿Deseas reemplazarlo?",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes){ocupado=false;return;}
    setEnabled(false);estado->setText("Verificando acceso y generando el PDF completo…");
    auto *observador=new QFutureWatcher<ResultadoPdf>(this);
    connect(observador,&QFutureWatcher<ResultadoPdf>::finished,this,[=]{
        const auto r=observador->result();observador->deleteLater();ocupado=false;setEnabled(true);
        if(r.consulta.correcto())mostrar(r.consulta.detalle);else limpiar();
        estado->setText(r.correcto()?"PDF guardado correctamente en: "+ruta:r.error);
    });
    const auto identidad=sesion;const int id=simulacionId;
    observador->setFuture(QtConcurrent::run([identidad,id,ruta]{
        ConexionBD conexion;QString error;if(!conexion.abrir(error)){ResultadoPdf r;r.error=error;return r;}
        return ServicioAmortizacion(conexion.base()).exportar(identidad,id,ruta);
    }));
}
