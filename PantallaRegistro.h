#pragma once
#include <QWidget>
#include "Acceso.h"
#include <functional>
class PantallaRegistro : public QWidget {
    Q_OBJECT
public:
    explicit PantallaRegistro(QWidget *parent=nullptr);
    void limpiar() { if (restablecer) restablecer(); }
signals:
    void accesoCompletado(const Sesion &sesion);
    void volverSolicitado();
private:
    std::function<void()> restablecer;
};
