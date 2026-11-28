#pragma once
#include "Amortizacion.h"
#include "TareaAcceso.h"
class QLabel;
class QPushButton;
class QTableWidget;
class QBoxLayout;
class VentanaAmortizacion : public DialogoGuardado {
    Q_OBJECT
public:
    VentanaAmortizacion(const Sesion &sesion,int simulacionId,QWidget *parent=nullptr);
    void actualizarDatos(bool silencioso=false);
protected:
    void resizeEvent(QResizeEvent *evento) override;
private:
    void mostrar(const DetalleAmortizacion &detalle);
    void limpiar();
    void exportar();
    Sesion sesion;
    int simulacionId;
    bool ocupado=false,valido=false;
    QBoxLayout *metricas=nullptr;
    QLabel *titulo,*condiciones,*fechas,*nota,*estado;
    QList<QLabel*> cifras;
    QTableWidget *tabla;
    QPushButton *actualizar,*pdf;
};
