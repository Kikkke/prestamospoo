#pragma once
#include <QWidget>
#include "Acceso.h"
#include <functional>
class PantallaLogin : public QWidget {
    Q_OBJECT
public:
    explicit PantallaLogin(QWidget *parent=nullptr);
    void limpiar() { if (restablecer) restablecer(); }
signals:
    void accesoCompletado(const Sesion &sesion);
    void crearCuentaSolicitada();
private:
    std::function<void()> restablecer;
};
