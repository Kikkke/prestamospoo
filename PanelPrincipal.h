#pragma once
#include <QWidget>

class PanelPrincipal : public QWidget
{
    Q_OBJECT
public:
    explicit PanelPrincipal(QWidget *parent = nullptr);
signals:
    void salirSolicitado();
};
