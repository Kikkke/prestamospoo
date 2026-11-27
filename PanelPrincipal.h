#pragma once
#include <QWidget>
#include <QDate>
#include "Acceso.h"

class PanelPrincipal : public QWidget
{
    Q_OBJECT
public:
    explicit PanelPrincipal(QWidget *parent = nullptr);
    Sesion sesionActual;
signals:
    void salirSolicitado();
    void cargarPerfilSesion(const QString &nombres, const QString &paterno, const QString &materno, const QString &correo, const QDate &nacimiento);
};
