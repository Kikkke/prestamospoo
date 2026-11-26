#pragma once
#include <QWidget>
#include <QSqlDatabase>
#include "Acceso.h"
class VentanaAcceso : public QWidget {
public:
    explicit VentanaAcceso(QSqlDatabase base, QWidget *parent=nullptr);
private:
    Sesion sesion;
};
