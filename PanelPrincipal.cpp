#include "PanelPrincipal.h"
#include "FondoAcceso.h"
#include <QButtonGroup>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
QLabel *texto(const QString &contenido, const char *tipo = "")
{
    auto *label = new QLabel(contenido);
    label->setObjectName(tipo);
    label->setWordWrap(true);
    return label;
}
QVBoxLayout *pagina(QStackedWidget *stack, const QString &titulo, const QString &descripcion,
                    const QString &imagen = {})
{
    auto *contenido = new FondoAcceso;
    contenido->setObjectName("page");
    auto *layout = new QVBoxLayout(contenido);
    layout->setContentsMargins(36, 32, 36, 32);
    layout->setSpacing(20);
    auto *cabecera = new QHBoxLayout;
    auto *titulos = new QVBoxLayout;
    titulos->setSpacing(12);
    titulos->addWidget(texto(descripcion.section("  /  ", 0, 0), "eyebrow"));
    titulos->addWidget(texto(titulo, "title"));
    titulos->addWidget(texto(descripcion.section("  /  ", 1), "muted"));
    cabecera->addLayout(titulos, 1);
    if (!imagen.isEmpty()) {
        auto *ilustracion = new IlustracionAcceso(imagen);
        ilustracion->setFixedSize(150, 140);
        cabecera->addWidget(ilustracion);
    }
    layout->addLayout(cabecera);
    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setWidget(contenido);
    stack->addWidget(scroll);
    return layout;
}
QVBoxLayout *tarjeta(QBoxLayout *padre)
{
    auto *frame = new QFrame;
    frame->setObjectName("card");
    auto *layout = new QVBoxLayout(frame);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(14);
    padre->addWidget(frame);
    return layout;
}
QPushButton *boton(QBoxLayout *layout, const QString &titulo, bool principal = false)
{
    auto *b = new QPushButton(titulo);
    if (principal) b->setObjectName("primary");
    b->setCursor(Qt::PointingHandCursor);
    layout->addWidget(b);
    return b;
}
void campo(QVBoxLayout *layout, const QString &etiqueta, QWidget *control)
{
    auto *label = texto(etiqueta);
    label->setBuddy(control);
    layout->addWidget(label);
    layout->addWidget(control);
}
QTableWidget *tabla(QVBoxLayout *layout, const QStringList &cabeceras,
                     const QList<QStringList> &filas)
{
    auto *t = new QTableWidget(filas.size(), cabeceras.size());
    t->setHorizontalHeaderLabels(cabeceras);
    for (int r = 0; r < filas.size(); ++r)
        for (int c = 0; c < filas[r].size(); ++c)
            t->setItem(r, c, new QTableWidgetItem(filas[r][c]));
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setSelectionMode(QAbstractItemView::SingleSelection);
    t->setAlternatingRowColors(true);
    t->verticalHeader()->hide();
    t->verticalHeader()->setDefaultSectionSize(48);
    t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    t->setMinimumHeight(230);
    layout->addWidget(t);
    return t;
}
}

PanelPrincipal::PanelPrincipal(QWidget *parent) : QWidget(parent)
{
    auto *raiz = new QHBoxLayout(this);
    raiz->setContentsMargins(0, 0, 0, 0);
    raiz->setSpacing(0);
    auto *lateral = new QFrame;
    lateral->setObjectName("sidebar");
    lateral->setFixedWidth(225);
    auto *menu = new QVBoxLayout(lateral);
    menu->setContentsMargins(20, 32, 20, 24);
    menu->setSpacing(12);
    menu->addWidget(texto("PRÉSTAMOS", "brand"));
    menu->addWidget(texto("Planea con claridad.", "muted"));
    menu->addSpacing(32);
    auto *pantallas = new QStackedWidget;
    auto *grupo = new QButtonGroup(this);
    const QStringList nombres = {"Inicio · Simulador", "Resultados", "Historial", "Comparador", "Acerca de"};
    for (int i = 0; i < nombres.size(); ++i) {
        auto *b = boton(menu, nombres[i]);
        b->setCheckable(true);
        grupo->addButton(b, i);
        connect(b, &QPushButton::clicked, this, [pantallas, i]() { pantallas->setCurrentIndex(i); });
    }
    grupo->button(0)->setChecked(true);
    connect(pantallas, &QStackedWidget::currentChanged, this, [grupo](int i) {
        if (auto *b = grupo->button(i)) b->setChecked(true);
    });
    menu->addStretch();
    menu->addWidget(texto("MODO PROTOTIPO\nDatos ilustrativos", "muted"));
    connect(boton(menu, "Volver al acceso"), &QPushButton::clicked, this, [this, pantallas]() {
        pantallas->setCurrentIndex(0);
        emit salirSolicitado();
    });
    raiz->addWidget(lateral);
    raiz->addWidget(pantallas, 1);

    auto *sim = pagina(pantallas, "Tu próximo plan empieza aquí.", "INICIO / SIMULADOR  /  Define las condiciones de tu préstamo.", "simulador.png");
    auto *form = tarjeta(sim);
    campo(form, "Nombre de la simulación", new QLineEdit);
    auto *monto = new QDoubleSpinBox;
    monto->setRange(0.01, 999999999.99);
    monto->setPrefix("$ "); monto->setSuffix(" MXN"); monto->setValue(12000);
    campo(form, "Monto solicitado", monto);
    auto *tasa = new QDoubleSpinBox;
    tasa->setRange(0, 1000); tasa->setSuffix(" %");
    campo(form, "Tasa nominal anual", tasa);
    auto *meses = new QSpinBox;
    meses->setRange(1, 600); meses->setSuffix(" meses"); meses->setValue(3);
    campo(form, "Plazo", meses);
    auto *calcular = boton(form, "Calcular préstamo");
    calcular->setEnabled(false);
    form->addWidget(texto("El cálculo se habilitará después. Puedes recorrer un resultado fijo de ejemplo.", "muted"));
    connect(boton(form, "Ver resultado de ejemplo", true), &QPushButton::clicked,
            this, [pantallas]() { pantallas->setCurrentIndex(1); });
    sim->addWidget(texto("Tasa fija · Pagos mensuales · Sin comisiones, seguros, impuestos ni CAT.", "muted"));
    sim->addStretch();

    auto *res = pagina(pantallas, "Cada pago, en detalle.", "RESULTADOS  /  Ejemplo fijo: Computadora · $12,000 MXN · 0 % anual · 3 meses.");
    auto *metricas = new QHBoxLayout;
    res->addLayout(metricas);
    const QStringList titulos = {"Mensualidad", "Intereses totales", "Total por pagar"};
    const QStringList valores = {"$4,000.00", "$0.00", "$12,000.00"};
    for (int i = 0; i < 3; ++i) {
        auto *c = tarjeta(metricas);
        c->addWidget(texto(titulos[i], "muted"));
        c->addWidget(texto(valores[i], "metric"));
        c->addWidget(texto("MXN", "muted"));
    }
    res->addWidget(texto("Tabla de amortización · Importes en MXN"));
    tabla(res, {"Mes", "Saldo inicial", "Interés", "Capital", "Pago", "Saldo final"},
          {{"1", "12,000.00", "0.00", "4,000.00", "4,000.00", "8,000.00"},
           {"2", "8,000.00", "0.00", "4,000.00", "4,000.00", "4,000.00"},
           {"3", "4,000.00", "0.00", "4,000.00", "4,000.00", "0.00"}});
    boton(res, "Guardar simulación (próximamente)")->setEnabled(false);
    res->addStretch();

    auto *hist = pagina(pantallas, "Tus planes, en un solo lugar.", "HISTORIAL  /  Registros de muestra. No se han guardado simulaciones reales.", "historial.png");
    auto *lista = tabla(hist, {"Nombre", "Monto MXN", "Tasa anual", "Plazo"},
                       {{"Computadora", "12,000.00", "0 %", "3 meses"},
                        {"Computadora a 6 meses", "12,000.00", "0 %", "6 meses"}});
    lista->selectRow(0);
    auto *acciones = new QHBoxLayout;
    hist->addLayout(acciones);
    connect(boton(acciones, "Consultar ejemplo", true), &QPushButton::clicked, this, [this, lista, pantallas]() {
        if (lista->currentRow() == 0) pantallas->setCurrentIndex(1);
        else if (lista->currentRow() == 1) pantallas->setCurrentIndex(3);
        else QMessageBox::information(this, "Selecciona una simulación", "Selecciona una fila para consultar su ejemplo.");
    });
    connect(boton(acciones, "Ver confirmación de eliminación"), &QPushButton::clicked, this, [this, lista]() {
        if (lista->currentRow() < 0) return;
        QMessageBox dialogo(QMessageBox::Question, "Eliminar simulación",
            QString("¿Eliminar «%1»?\n\nVista previa: los datos de muestra no se eliminarán.")
                .arg(lista->item(lista->currentRow(), 0)->text()), QMessageBox::Cancel, this);
        dialogo.button(QMessageBox::Cancel)->setText("Volver");
        dialogo.addButton("Eliminar (próximamente)", QMessageBox::DestructiveRole)->setEnabled(false);
        dialogo.exec();
    });
    hist->addStretch();

    auto *comp = pagina(pantallas, "Dos opciones. Una vista clara.", "COMPARADOR  /  Ejemplo fijo de un mismo monto con diferentes plazos.", "comparador.png");
    auto *seleccion = tarjeta(comp);
    auto *a = new QComboBox; a->addItems({"Computadora", "Computadora a 6 meses"});
    auto *b = new QComboBox; b->addItems({"Computadora", "Computadora a 6 meses"}); b->setCurrentIndex(1);
    campo(seleccion, "Primera simulación", a);
    campo(seleccion, "Segunda simulación", b);
    boton(seleccion, "Comparar (próximamente)")->setEnabled(false);
    seleccion->addWidget(texto("La tabla de muestra no cambia con la selección.", "muted"));
    tabla(comp, {"Concepto", "Computadora", "Computadora a 6 meses"},
          {{"Monto", "$12,000.00", "$12,000.00"}, {"Tasa anual", "0 %", "0 %"},
           {"Plazo", "3 meses", "6 meses"}, {"Mensualidad", "$4,000.00", "$2,000.00"},
           {"Intereses", "$0.00", "$0.00"}, {"Total", "$12,000.00", "$12,000.00"}})->setMinimumHeight(350);
    comp->addWidget(texto("Menor mensualidad: opción a 6 meses.\nTotal por pagar: ambas opciones cuestan lo mismo. Importes en MXN.", "muted"));
    comp->addStretch();

    auto *info = pagina(pantallas, "Entiende antes de decidir.", "ACERCA DE  /  Calculador de préstamos", "acerca-de.png");
    auto *ayuda = tarjeta(info);
    ayuda->addWidget(texto("Una herramienta para explorar tus opciones.", "metric"));
    ayuda->addWidget(texto("1. Define el monto, la tasa anual y el plazo.\n\n2. Consulta el pago mensual y la amortización.\n\n3. Guarda tus simulaciones y compara dos alternativas."));
    ayuda->addWidget(texto("Alcance", "metric"));
    ayuda->addWidget(texto("Proyecto académico con préstamos de tasa fija y pagos mensuales en pesos mexicanos. No incluye comisiones, seguros, impuestos ni CAT, ni realiza operaciones bancarias."));
    ayuda->addWidget(texto("Esta versión permite recorrer las pantallas. El registro, los cálculos y el almacenamiento todavía no están conectados.", "muted"));
    info->addStretch();
}
