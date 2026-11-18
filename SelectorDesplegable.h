#pragma once
#include <QComboBox>
#include <QListView>
#include <QStyledItemDelegate>
#include <QScreen>
#include <QScrollBar>
#include <QProxyStyle>

// Evita el modo menú nativo: usa una lista desplazable con seis filas.
class EstiloSelectores : public QProxyStyle {
public:
    EstiloSelectores():QProxyStyle("Fusion"){}
    int styleHint(StyleHint hint,const QStyleOption *option=nullptr,const QWidget *widget=nullptr,QStyleHintReturn *ret=nullptr) const override {
        if(hint==QStyle::SH_ComboBox_Popup)return 0;
        return QProxyStyle::styleHint(hint,option,widget,ret);
    }
};
class FilaSelector : public QStyledItemDelegate {
public:
    explicit FilaSelector(QObject *parent):QStyledItemDelegate(parent){}
    QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const override {
        QSize s=QStyledItemDelegate::sizeHint(option,index);s.setHeight(qMax(36,s.height()));return s;
    }
};
class SelectorDesplegable : public QComboBox {
public:
    explicit SelectorDesplegable(QWidget *parent=nullptr):QComboBox(parent){
        setMaxVisibleItems(6);
        auto *lista=new QListView;
        lista->setObjectName("selectorOpciones");
        lista->setItemDelegate(new FilaSelector(lista));
        lista->setUniformItemSizes(true);lista->setMouseTracking(true);
        lista->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        lista->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        lista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        lista->setSpacing(0);setView(lista);
    }
    void showPopup() override {
        int ancho=width();
        for(int i=0;i<count();++i)ancho=qMax(ancho,fontMetrics().horizontalAdvance(itemText(i))+56);
        const QRect pantalla=screen()->availableGeometry();
        view()->setMinimumWidth(qMin(ancho,pantalla.width()-24));
        view()->setMaximumHeight(qMin(6*40+14,pantalla.height()-40));
        QComboBox::showPopup();
    }
};
