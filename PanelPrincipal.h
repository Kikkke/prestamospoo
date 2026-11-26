#pragma once
#include <QWidget>
#include <QDate>

class PanelPrincipal : public QWidget
{
    Q_OBJECT
public:
    explicit PanelPrincipal(QWidget *parent = nullptr);
signals:
    void salirSolicitado();
    void cargarPerfilSesion(const QString &nombres, const QString &paterno, const QString &materno, const QString &correo, const QDate &nacimiento);
};
