#pragma once

#include <QWidget>
#include <QDate>

class PantallaRegistro : public QWidget
{
    Q_OBJECT

public:
    explicit PantallaRegistro(QWidget *parent = nullptr);

signals:
    void inicioSolicitado();
    void perfilDemoCreado(const QString &nombres, const QString &paterno, const QString &materno, const QString &correo, const QDate &nacimiento);
    void volverSolicitado();
};
