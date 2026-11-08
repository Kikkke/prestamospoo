#pragma once

#include <QWidget>

class PantallaRegistro : public QWidget
{
    Q_OBJECT

public:
    explicit PantallaRegistro(QWidget *parent = nullptr);

signals:
    void inicioSolicitado();
    void volverSolicitado();
};
