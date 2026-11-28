#include "HistorialSimulaciones.h"
#include "VentanaAmortizacion.h"
#include "ConexionBD.h"
#include "Simulador.h"
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
QString normalizar(const QString &s) {
    QString resultado;
    for(const QChar c:s.normalized(QString::NormalizationForm_D).toCaseFolded())
        if(c.category()!=QChar::Mark_NonSpacing && c.category()!=QChar::Mark_SpacingCombining && c.category()!=QChar::Mark_Enclosing)
            resultado.append(c);
    return resultado.trimmed();
}
constexpr int RolId=Qt::UserRole+1;
class CeldaHistorial : public QTableWidgetItem {
public:
    CeldaHistorial(const QString &texto,int id,qint64 valor=0,bool numerica=false):QTableWidgetItem(texto),esNumero(numerica) {
        setData(Qt::UserRole,valor);setData(RolId,id);setToolTip(texto);
        if(numerica)setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }
    bool operator<(const QTableWidgetItem &otra) const override {
        if(esNumero) {
            const auto a=data(Qt::UserRole).toLongLong(),b=otra.data(Qt::UserRole).toLongLong();
            if(a!=b)return a<b;
        } else {
            const int comparacion=QString::compare(normalizar(text()),normalizar(otra.text()));
            if(comparacion!=0)return comparacion<0;
        }
        return data(RolId).toInt()<otra.data(RolId).toInt();
    }
private:
    bool esNumero;
};
}

HistorialSimulaciones::HistorialSimulaciones(QWidget *parent):QFrame(parent) {
    setObjectName("card");
    auto *layout=new QVBoxLayout(this);layout->setContentsMargins(24,24,24,24);layout->setSpacing(16);
    auto *barra=new QHBoxLayout;
    buscar=new QLineEdit;buscar->setObjectName("buscarHistorial");buscar->setMaxLength(160);
    buscar->setPlaceholderText("Buscar simulación por nombre…");buscar->setAccessibleName("Buscar simulación por nombre");buscar->setClearButtonEnabled(true);
    QPixmap lupa(40,40);lupa.fill(Qt::transparent);
    {QPainter p(&lupa);p.setRenderHint(QPainter::Antialiasing);p.setPen(QPen(QColor("#ed916f"),3));p.drawEllipse(QRectF(7,7,19,19));p.drawLine(24,24,34,34);}
    buscar->addAction(QIcon(lupa),QLineEdit::LeadingPosition);barra->addWidget(buscar,1);
    contador=new QLabel;contador->setObjectName("caption");barra->addWidget(contador);
    actualizar=new QPushButton("Actualizar");actualizar->setObjectName("actualizarHistorial");barra->addWidget(actualizar);layout->addLayout(barra);
    auto *indicacion=new QLabel("Pulsa un encabezado para ordenar. El icono de tabla abre la amortización y permite exportar el PDF. La X elimina después de confirmar.");
    indicacion->setObjectName("muted");indicacion->setWordWrap(true);layout->addWidget(indicacion);
    tabla=new QTableWidget(0,5);tabla->setObjectName("tablaHistorial");tabla->setAccessibleName("Historial de tus simulaciones");
    tabla->setHorizontalHeaderLabels({"Nombre","Monto MXN","Tasa anual","Plazo","Acciones"});
    tabla->verticalHeader()->hide();tabla->verticalHeader()->setDefaultSectionSize(48);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);tabla->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabla->setSelectionMode(QAbstractItemView::SingleSelection);tabla->setAlternatingRowColors(true);tabla->setShowGrid(false);tabla->setMinimumHeight(430);
    tabla->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    tabla->horizontalHeader()->setMinimumSectionSize(100);
    for(int c=1;c<4;++c)tabla->horizontalHeader()->setSectionResizeMode(c,QHeaderView::ResizeToContents);
    tabla->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Fixed);tabla->setColumnWidth(4,144);
    layout->addWidget(tabla,1);
    vacio=new QLabel;vacio->setObjectName("vacioHistorial");vacio->setWordWrap(true);vacio->setAlignment(Qt::AlignCenter);layout->addWidget(vacio);
    estado=new QLabel;estado->setObjectName("estadoHistorial");estado->setWordWrap(true);estado->setTextFormat(Qt::PlainText);layout->addWidget(estado);
    tabla->setSortingEnabled(true);tabla->sortItems(0,Qt::AscendingOrder);
    connect(buscar,&QLineEdit::textChanged,this,[this]{filtrar();});
    connect(tabla->model(),&QAbstractItemModel::layoutChanged,this,[this]{if(!ocupado)filtrar();});
    connect(actualizar,&QPushButton::clicked,this,[this]{recargar(sesion);});
    connect(tabla,&QTableWidget::itemActivated,this,[this](QTableWidgetItem *item){
        if(item && tabla->item(item->row(),0))abrirAmortizacion(tabla->item(item->row(),0)->data(RolId).toInt());
    });
    limpiar();
}
void HistorialSimulaciones::limpiar() {
    cargado=false;tabla->setRowCount(0);tabla->setEnabled(false);vacio->hide();contador->setText("Sin cargar");
}
void HistorialSimulaciones::recargar(const Sesion &actual) {
    const bool otraCuenta=sesion.usuarioId!=actual.usuarioId || sesion.rol!=actual.rol;
    sesion=actual;++revision;
    if(otraCuenta){buscar->clear();limpiar();}
    if(!ocupado)ejecutar(); // Si está ocupado, se descarta la respuesta anterior y se vuelve a cargar.
}
void HistorialSimulaciones::ejecutar(int eliminarId) {
    if(ocupado)return;
    ocupado=true;tabla->setEnabled(false);actualizar->setEnabled(false);
    if(!eliminarId)limpiar();
    estado->setText(eliminarId?"Eliminando simulación…":"Cargando historial…");
    const Sesion identidad=sesion;const quint64 solicitud=revision;
    auto *observador=new QFutureWatcher<ResultadoHistorial>(this);
    connect(observador,&QFutureWatcher<ResultadoHistorial>::finished,this,[=] {
        const auto resultado=observador->result();observador->deleteLater();ocupado=false;
        if(solicitud!=revision){ejecutar();return;}
        actualizar->setEnabled(true);
        if(!resultado.correcto()) {
            if(!eliminarId || resultado.accesoDenegado)limpiar();
            tabla->setEnabled(cargado);
            estado->setText(resultado.error);return;
        }
        mostrar(resultado.registros);
        estado->setText(resultado.eliminadoId?"Simulación eliminada del historial.":QString());
    });
    observador->setFuture(QtConcurrent::run([identidad,eliminarId] {
        ConexionBD conexion;QString error;
        if(!conexion.abrir(error)){ResultadoHistorial r;r.error=error+" Pulsa Actualizar para reintentar.";return r;}
        ServicioHistorial servicio(conexion.base());
        return eliminarId?servicio.eliminar(identidad,eliminarId):servicio.listar(identidad);
    }));
}
void HistorialSimulaciones::mostrar(const QVector<RegistroHistorial> &registros) {
    const int columna=tabla->horizontalHeader()->sortIndicatorSection();
    const auto orden=tabla->horizontalHeader()->sortIndicatorOrder();
    tabla->setSortingEnabled(false);tabla->setRowCount(0);
    const QLocale mx(QLocale::Spanish,QLocale::Mexico);
    for(const auto &r:registros) {
        const int fila=tabla->rowCount();tabla->insertRow(fila);
        tabla->setItem(fila,0,new CeldaHistorial(r.nombre,r.id));
        tabla->setItem(fila,1,new CeldaHistorial(MotorPrestamo::dinero(r.montoCentavos),r.id,r.montoCentavos,true));
        tabla->setItem(fila,2,new CeldaHistorial(mx.toString(r.tasaDiezmilesimas/10000.0,'f',r.tasaDiezmilesimas%100?4:2)+" %",r.id,r.tasaDiezmilesimas,true));
        tabla->setItem(fila,3,new CeldaHistorial(QString::number(r.meses)+" meses",r.id,r.meses,true));
        auto *contenedor=new QWidget;contenedor->setObjectName("historyActionCell");
        auto *accion=new QHBoxLayout(contenedor);accion->setContentsMargins(6,4,6,4);accion->setSpacing(8);accion->addStretch();
        auto *ver=new QPushButton;ver->setFixedSize(34,34);ver->setToolTip("Ver amortización y exportar PDF");ver->setAccessibleName(ver->toolTip());ver->setObjectName("verAmortizacion");ver->setProperty("simulacionId",r.id);
        connect(ver,&QPushButton::clicked,this,[this,id=r.id]{abrirAmortizacion(id);});
        QPixmap icono(24,24);icono.fill(Qt::transparent);
        {QPainter p(&icono);p.setPen(QPen(QColor("#ed916f"),2));p.drawRect(3,4,18,16);p.drawLine(3,10,21,10);p.drawLine(3,15,21,15);p.drawLine(10,4,10,20);}
        ver->setIcon(QIcon(icono));accion->addWidget(ver);
        auto *eliminar=new QPushButton(QString::fromUtf8("×"));eliminar->setObjectName("deleteRow");eliminar->setFixedSize(34,34);
        eliminar->setProperty("simulacionId",r.id);eliminar->setCursor(Qt::PointingHandCursor);
        eliminar->setToolTip("Eliminar simulación: "+r.nombre);eliminar->setAccessibleName(eliminar->toolTip());
        accion->addWidget(eliminar,0,Qt::AlignCenter);accion->addStretch();tabla->setCellWidget(fila,4,contenedor);
        connect(eliminar,&QPushButton::clicked,this,[this,id=r.id]{confirmarEliminacion(id);});
    }
    cargado=true;tabla->setSortingEnabled(true);tabla->sortItems(columna,orden);tabla->setEnabled(true);filtrar();
}
void HistorialSimulaciones::filtrar() {
    if(!cargado)return;
    const auto consulta=normalizar(buscar->text());int visibles=0;
    for(int fila=0;fila<tabla->rowCount();++fila) {
        const auto *item=tabla->item(fila,0);if(!item)continue;
        const bool coincide=normalizar(item->text()).contains(consulta);tabla->setRowHidden(fila,!coincide);if(coincide)++visibles;
    }
    contador->setText(QString("Mostrando %1 de %2").arg(visibles).arg(tabla->rowCount()));
    vacio->setText(tabla->rowCount()==0?"Aún no tienes simulaciones guardadas.":"No encontramos simulaciones con ese nombre. Prueba otra búsqueda.");
    vacio->setVisible(visibles==0);
}
void HistorialSimulaciones::confirmarEliminacion(int id) {
    if(ocupado || confirmando || !cargado)return;
    QString nombre;bool encontrado=false;
    for(int fila=0;fila<tabla->rowCount();++fila)if(tabla->item(fila,0)->data(RolId).toInt()==id){nombre=tabla->item(fila,0)->text();encontrado=true;break;}
    if(!encontrado)return;
    confirmando=true;const auto version=revision;
    QMessageBox dialogo(QMessageBox::Question,"Eliminar simulación",
        QString("¿Eliminar «%1»?\n\nDejará de aparecer en tu historial. No podrás recuperarla desde la aplicación.").arg(nombre),QMessageBox::NoButton,this);
    dialogo.setTextFormat(Qt::PlainText);
    auto *cancelar=dialogo.addButton("Cancelar",QMessageBox::RejectRole);
    auto *confirmar=dialogo.addButton("Eliminar",QMessageBox::DestructiveRole);
    dialogo.setDefaultButton(cancelar);dialogo.setEscapeButton(cancelar);dialogo.exec();confirmando=false;
    if(dialogo.clickedButton()==confirmar && revision==version && !ocupado)ejecutar(id);
}

void HistorialSimulaciones::abrirAmortizacion(int id) {
    if(ocupado || confirmando || !cargado)return;
    confirmando=true;
    VentanaAmortizacion ventana(sesion,id,this);ventana.exec();
    confirmando=false;recargar(sesion);
}
