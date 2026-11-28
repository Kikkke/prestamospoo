#pragma once
#include "ServicioHistorial.h"
#include <QFrame>

class QLineEdit;
class QTableWidget;
class QLabel;
class QPushButton;

class HistorialSimulaciones : public QFrame {
    Q_OBJECT
public:
    explicit HistorialSimulaciones(QWidget *parent=nullptr);
    void recargar(const Sesion &sesion);
private:
    void ejecutar(int eliminarId=0);
    void mostrar(const QVector<RegistroHistorial> &registros);
    void filtrar();
    void confirmarEliminacion(int id);
    void abrirAmortizacion(int id);
    void limpiar();
    Sesion sesion;
    quint64 revision=0;
    bool ocupado=false, confirmando=false, cargado=false;
    QLineEdit *buscar;
    QTableWidget *tabla;
    QLabel *contador,*vacio,*estado;
    QPushButton *actualizar;
};
