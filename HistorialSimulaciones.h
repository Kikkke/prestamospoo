#pragma once
#include "VentanaAmortizacion.h"
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

class CeldaHistorial : public QTableWidgetItem
{
public:
    CeldaHistorial(const QString &texto, double valor) : QTableWidgetItem(texto)
    { setData(Qt::UserRole, valor); setTextAlignment(Qt::AlignRight | Qt::AlignVCenter); }
    bool operator<(const QTableWidgetItem &otra) const override
    { return data(Qt::UserRole).toDouble() < otra.data(Qt::UserRole).toDouble(); }
};

class HistorialSimulaciones : public QFrame
{
public:
    explicit HistorialSimulaciones(QWidget *parent = nullptr) : QFrame(parent)
    {
        setObjectName("card");
        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(24,24,24,24);
        layout->setSpacing(16);
        auto *barra = new QHBoxLayout;
        buscar = new QLineEdit;
        buscar->setPlaceholderText("Buscar simulación por nombre…");
        buscar->setAccessibleName("Buscar simulación por nombre");
        buscar->setClearButtonEnabled(true);
        QPixmap lupa(40,40); lupa.fill(Qt::transparent);
        { QPainter p(&lupa); p.setRenderHint(QPainter::Antialiasing);
          p.setPen(QPen(QColor("#ed916f"),3)); p.drawEllipse(QRectF(7,7,19,19)); p.drawLine(24,24,34,34); }
        buscar->addAction(QIcon(lupa),QLineEdit::LeadingPosition);
        barra->addWidget(buscar,1);
        contador = new QLabel;
        contador->setObjectName("caption");
        barra->addWidget(contador);
        layout->addLayout(barra);
        auto *indicacion = new QLabel("Pulsa un encabezado para ordenar. El icono de tabla abre la amortización. La X elimina el registro después de confirmar.");
        indicacion->setObjectName("muted"); indicacion->setWordWrap(true);
        layout->addWidget(indicacion);

        tabla = new QTableWidget(0,5);
        tabla->setAccessibleName("Historial de simulaciones de muestra");
        tabla->setHorizontalHeaderLabels({"Nombre", "Monto MXN", "Tasa anual", "Plazo", "Acciones"});
        tabla->verticalHeader()->hide();
        tabla->verticalHeader()->setDefaultSectionSize(48);
        tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tabla->setSelectionBehavior(QAbstractItemView::SelectRows);
        tabla->setSelectionMode(QAbstractItemView::SingleSelection);
        tabla->setAlternatingRowColors(true);
        tabla->setShowGrid(false);
        tabla->setMinimumHeight(430);
        tabla->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
        for(int c=1;c<4;++c) tabla->horizontalHeader()->setSectionResizeMode(c,QHeaderView::ResizeToContents);
        tabla->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Fixed);
        tabla->setColumnWidth(4,144);
        layout->addWidget(tabla,1);
        vacio = new QLabel;
        vacio->setObjectName("muted"); vacio->setWordWrap(true);
        vacio->setAlignment(Qt::AlignCenter);
        layout->addWidget(vacio);
        auto *nota = new QLabel("Datos ilustrativos. Las eliminaciones solo duran hasta cerrar la aplicación; al reiniciar volverán los 20 ejemplos.");
        nota->setObjectName("caption"); nota->setWordWrap(true);
        layout->addWidget(nota);

        struct Ejemplo { const char *nombre; double monto; double tasa; int meses; };
        const Ejemplo ejemplos[] = {
            {"Computadora",12000,0,3},{"Computadora a 6 meses",12000,0,6},
            {"Automóvil",180000,14.5,48},{"Estudios universitarios",45000,10,24},
            {"Remodelación de cocina",65000,16,36},{"Motocicleta",38000,18,24},
            {"Viaje familiar",25000,12,12},{"Equipo de fotografía",32000,15,18},
            {"Muebles para el hogar",28000,9.5,12},{"Paneles solares",85000,11,36},
            {"Herramientas de trabajo",18500,13,18},{"Curso de idiomas",15000,8,12},
            {"Consultorio dental",120000,17,48},{"Negocio de repostería",55000,14,24},
            {"Reparación de automóvil",22000,19,12},{"Equipo de sonido",16500,12.5,18},
            {"Bicicleta eléctrica",27000,10.5,24},{"Gastos médicos",40000,9,24},
            {"Teléfono móvil",14000,0,12},{"Ampliación de vivienda",150000,15.5,60}
        };
        const QLocale mx(QLocale::Spanish,QLocale::Mexico);
        int id=0;
        for(const auto &e:ejemplos) {
            const int fila=tabla->rowCount(); tabla->insertRow(fila);
            auto *nombre=new QTableWidgetItem(QString::fromUtf8(e.nombre));
            nombre->setData(Qt::UserRole,++id);
            tabla->setItem(fila,0,nombre);
            tabla->setItem(fila,1,new CeldaHistorial(mx.toString(e.monto,'f',2),e.monto));
            tabla->setItem(fila,2,new CeldaHistorial(mx.toString(e.tasa,'f',1)+" %",e.tasa));
            tabla->setItem(fila,3,new CeldaHistorial(QString::number(e.meses)+" meses",e.meses));
            auto *contenedor=new QWidget;
            contenedor->setObjectName("historyActionCell");
            auto *accion=new QHBoxLayout(contenedor);
            accion->setContentsMargins(6,4,6,4);
            accion->setSpacing(8);
            accion->addStretch();
            auto *ver=new QPushButton;ver->setFixedSize(34,34);ver->setToolTip("Ver tabla de amortización");ver->setAccessibleName(ver->toolTip());ver->setCursor(Qt::PointingHandCursor);
            QPixmap icono(24,24);icono.fill(Qt::transparent);{QPainter p(&icono);p.setPen(QPen(QColor("#ed916f"),2));p.drawRect(3,4,18,16);p.drawLine(3,10,21,10);p.drawLine(3,15,21,15);p.drawLine(10,4,10,20);}
            ver->setIcon(QIcon(icono));accion->addWidget(ver);
            const QString titulo=nombre->text();const double monto=e.monto,tasa=e.tasa;const int meses=e.meses;
            connect(ver,&QPushButton::clicked,this,[this,titulo,monto,tasa,meses](){VentanaAmortizacion d(titulo,monto,tasa,meses,this);d.exec();});
            auto *eliminar=new QPushButton(QString::fromUtf8("×"));
            eliminar->setObjectName("deleteRow"); eliminar->setFixedSize(34,34);
            eliminar->setCursor(Qt::PointingHandCursor);
            eliminar->setToolTip("Eliminar simulación: "+nombre->text());
            eliminar->setAccessibleName(eliminar->toolTip());
            accion->addWidget(eliminar,0,Qt::AlignCenter);
            accion->addStretch();
            tabla->setCellWidget(fila,4,contenedor);
            // La identidad no depende de la posición, que cambia al ordenar o filtrar.
            connect(eliminar,&QPushButton::clicked,this,[this,id]() { confirmarEliminacion(id); });
        }
        connect(tabla,&QTableWidget::itemActivated,this,[this](QTableWidgetItem *item){
            int r=item->row();VentanaAmortizacion d(tabla->item(r,0)->text(),tabla->item(r,1)->data(Qt::UserRole).toDouble(),tabla->item(r,2)->data(Qt::UserRole).toDouble(),tabla->item(r,3)->data(Qt::UserRole).toInt(),this);d.exec();
        });
        tabla->setSortingEnabled(true);
        tabla->sortItems(0,Qt::AscendingOrder);
        connect(buscar,&QLineEdit::textChanged,this,[this]() { filtrar(); });
        connect(tabla->horizontalHeader(),&QHeaderView::sortIndicatorChanged,this,[this]() { filtrar(); });
        filtrar();
    }
private:
    static QString normalizar(const QString &s)
    {
        QString resultado;
        for(const QChar c:s.normalized(QString::NormalizationForm_D).toCaseFolded())
            if(c.category()!=QChar::Mark_NonSpacing && c.category()!=QChar::Mark_SpacingCombining)
                resultado.append(c);
        return resultado.trimmed();
    }
    void filtrar()
    {
        const QString consulta=normalizar(buscar->text());
        int visibles=0;
        for(int f=0;f<tabla->rowCount();++f) {
            const bool coincide=normalizar(tabla->item(f,0)->text()).contains(consulta);
            tabla->setRowHidden(f,!coincide); if(coincide) ++visibles;
        }
        contador->setText(QString("Mostrando %1 de %2").arg(visibles).arg(tabla->rowCount()));
        vacio->setText(tabla->rowCount()==0 ? "No quedan simulaciones de muestra. Al reiniciar volverán los ejemplos."
                     : "No encontramos simulaciones con ese nombre. Prueba otra búsqueda.");
        vacio->setVisible(visibles==0);
    }
    void confirmarEliminacion(int id)
    {
        for(int fila=0;fila<tabla->rowCount();++fila) {
            if(tabla->item(fila,0)->data(Qt::UserRole).toInt()!=id) continue;
            QMessageBox dialogo(QMessageBox::Question,"Eliminar simulación",
                QString("¿Eliminar «%1»?\n\nSe quitará del historial durante esta ejecución. Volverá al reiniciar la aplicación.")
                .arg(tabla->item(fila,0)->text()),QMessageBox::NoButton,this);
            auto *cancelar=dialogo.addButton("Cancelar",QMessageBox::RejectRole);
            auto *confirmar=dialogo.addButton("Eliminar",QMessageBox::DestructiveRole);
            dialogo.setDefaultButton(cancelar); dialogo.setEscapeButton(cancelar);
            dialogo.exec();
            if(dialogo.clickedButton()==confirmar) { tabla->removeRow(fila); filtrar(); }
            return;
        }
    }
    QLineEdit *buscar;
    QTableWidget *tabla;
    QLabel *contador;
    QLabel *vacio;
};
