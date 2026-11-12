#pragma once

#include <QWidget>

class PantallaLogin : public QWidget
{
    Q_OBJECT

public:
    explicit PantallaLogin(QWidget *parent = nullptr);

signals:
    void administradorSolicitado();
    void clienteDeshabilitadoSolicitado();
    void inicioSolicitado();
    void crearCuentaSolicitada();
};
