#include "PanelPrincipal.h"
#include "FondoAcceso.h"
#include "PantallaAyuda.h"
#include "HistorialSimulaciones.h"
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
    if (QString::fromLatin1(tipo) == "badge" || QString::fromLatin1(tipo) == "eyebrow")
        label->setWordWrap(false);
    return label;
}
QVBoxLayout *pagina(QStackedWidget *stack, const QString &titulo, const QString &descripcion,
                    const QString &imagen = {})
{
    // El mismo fondo del acceso, compartido por todos los apartados.
    auto *contenido = new FondoAcceso;
    contenido->setObjectName("page");
    auto *layout = new QVBoxLayout(contenido);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);
    auto *cabecera = new QHBoxLayout;
    auto *titulos = new QVBoxLayout;
    titulos->setSpacing(12);
    titulos->addWidget(texto(descripcion.section("  /  ", 0, 0), "eyebrow"));
    titulos->addWidget(texto(titulo, "title"));
    titulos->addWidget(texto(descripcion.section("  /  ", 1), "muted"));
    cabecera->addLayout(titulos, 1);
    if (!imagen.isEmpty()) {
        auto *ilustracion = new IlustracionAcceso(imagen);
        ilustracion->setFixedSize(64, 64);
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
    layout->setContentsMargins(24, 16, 24, 16);
    layout->setSpacing(8);
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
    t->verticalHeader()->setDefaultSectionSize(40);
    t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    t->setMinimumHeight(230);
    layout->addWidget(t);
    return t;
}
}

PanelPrincipal::PanelPrincipal(QWidget *parent) : QWidget(parent)
{
    setObjectName("dashboard");
    auto *raiz = new QHBoxLayout(this);
    raiz->setContentsMargins(0, 0, 0, 0);
    raiz->setSpacing(0);
    auto *lateral = new QFrame;
    lateral->setObjectName("sidebar");
    lateral->setFixedWidth(206);
    auto *menu = new QVBoxLayout(lateral);
    menu->setContentsMargins(16, 24, 16, 20);
    menu->setSpacing(12);
    menu->addWidget(texto("PRÉSTAMOS", "brand"));
    menu->addWidget(texto("Planea con claridad.", "muted"));
    menu->addSpacing(24);
    menu->addWidget(texto("ESPACIO DE TRABAJO", "eyebrow"));
    auto *pantallas = new QStackedWidget;
    auto *grupo = new QButtonGroup(this);
    const QStringList nombres = {"Inicio · Simulador", "Historial", "Comparador", "Acerca de"};
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
    auto *cuerpo = new QVBoxLayout;
    cuerpo->setSpacing(0);
    auto *barra = new QFrame;
    barra->setObjectName("topbar");
    auto *filaBarra = new QHBoxLayout(barra);
    filaBarra->setContentsMargins(28, 12, 28, 12);
    auto *ruta = texto("Mi espacio  /  Inicio · Simulador", "muted");
    ruta->setWordWrap(false);
    filaBarra->addWidget(ruta);
    filaBarra->addStretch();
    filaBarra->addWidget(texto("MXN  ·  PROTOTIPO", "badge"));
    connect(pantallas, &QStackedWidget::currentChanged, this, [ruta, nombres](int i) {
        ruta->setText("Mi espacio  /  " + nombres.value(i));
    });
    cuerpo->addWidget(barra);
    cuerpo->addWidget(pantallas, 1);
    raiz->addLayout(cuerpo, 1);

    auto *sim = pagina(pantallas, "Simula tu próximo paso", "INICIO  /  Define tu préstamo y consulta su desglose en una sola vista.", "simulador.png");
    auto *zona = new QHBoxLayout;
    zona->setSpacing(18);
    sim->addLayout(zona);
    auto *form = tarjeta(zona);
    zona->setStretch(0, 3);
    form->addWidget(texto("Condiciones del préstamo", "section"));
    auto *nombre = new QLineEdit("Computadora");
    campo(form, "Nombre de la simulación", nombre);
    auto *monto = new QDoubleSpinBox;
    monto->setRange(0.01, 999999999.99);
    monto->setPrefix("$ "); monto->setSuffix(" MXN"); monto->setValue(12000);
    campo(form, "Monto solicitado", monto);
    auto *opciones = new QHBoxLayout;
    opciones->setSpacing(16);
    form->addLayout(opciones);
    auto *colTasa = new QVBoxLayout;
    auto *colPlazo = new QVBoxLayout;
    opciones->addLayout(colTasa, 1);
    opciones->addLayout(colPlazo, 1);
    auto *tasa = new QDoubleSpinBox;
    tasa->setRange(0, 1000); tasa->setSuffix(" %");
    campo(colTasa, "Tasa nominal anual", tasa);
    auto *meses = new QSpinBox;
    meses->setRange(1, 600); meses->setSuffix(" meses"); meses->setValue(3);
    campo(colPlazo, "Plazo", meses);
    boton(form, "Calcular préstamo · Próximamente", true)->setEnabled(false);
    form->addWidget(texto("Los resultados son un ejemplo fijo: no cambian al editar estos campos.", "muted"));
    form->addStretch();

    auto *metricas = new QVBoxLayout;
    metricas->setSpacing(12);
    zona->addLayout(metricas, 2);
    const QStringList titulos = {"Mensualidad", "Intereses totales", "Total por pagar"};
    const QStringList valores = {"$4,000.00", "$0.00", "$12,000.00"};
    for (int i = 0; i < 3; ++i) {
        auto *c = tarjeta(metricas);
        c->setSpacing(5);
        c->addWidget(texto(titulos[i], "muted"));
        auto *valor = texto(valores[i], i == 0 ? "metricAccent" : "metric");
        c->addWidget(valor);
        c->addWidget(texto("MXN · Ejemplo", "caption"));
    }
    auto *detalle = tarjeta(sim);
    auto *encabezado = new QHBoxLayout;
    detalle->addLayout(encabezado);
    encabezado->addWidget(texto("Tabla de amortización", "section"));
    encabezado->addStretch();
    encabezado->addWidget(texto("EJEMPLO · 3 MESES · 0 % ANUAL", "badge"));
    tabla(detalle, {"Mes", "Saldo inicial", "Interés", "Capital", "Pago", "Saldo final"},
          {{"1", "12,000.00", "0.00", "4,000.00", "4,000.00", "8,000.00"},
           {"2", "8,000.00", "0.00", "4,000.00", "4,000.00", "4,000.00"},
           {"3", "4,000.00", "0.00", "4,000.00", "4,000.00", "0.00"}})->setFixedHeight(170);
    auto *pie = new QHBoxLayout;
    detalle->addLayout(pie);
    pie->addWidget(texto("Importes en MXN. Sin comisiones, seguros, impuestos ni CAT.", "caption"), 1);
    boton(pie, "Guardar simulación")->setEnabled(false);
    sim->addStretch();

    auto *hist = pagina(pantallas, "Historial de simulaciones", "HISTORIAL  /  Registros de muestra. No se han guardado simulaciones reales.", "historial.png");
    hist->addWidget(new HistorialSimulaciones, 1);

    auto *comp = pagina(pantallas, "Compara tus opciones", "COMPARADOR  /  Ejemplo fijo de un mismo monto con diferentes plazos.", "comparador.png");
    auto *seleccion = tarjeta(comp);
    auto *a = new QComboBox; a->addItems({"Computadora", "Computadora a 6 meses"});
    auto *b = new QComboBox; b->addItems({"Computadora", "Computadora a 6 meses"}); b->setCurrentIndex(1);
    auto *pares = new QHBoxLayout;
    pares->setSpacing(18);
    seleccion->addLayout(pares);
    auto *primera = new QVBoxLayout;
    auto *segunda = new QVBoxLayout;
    pares->addLayout(primera, 1);
    pares->addLayout(segunda, 1);
    campo(primera, "Primera simulación", a);
    campo(segunda, "Segunda simulación", b);
    boton(seleccion, "Comparar (próximamente)")->setEnabled(false);
    seleccion->addWidget(texto("La tabla de muestra no cambia con la selección.", "muted"));
    tabla(comp, {"Concepto", "Computadora", "Computadora a 6 meses"},
          {{"Monto", "$12,000.00", "$12,000.00"}, {"Tasa anual", "0 %", "0 %"},
           {"Plazo", "3 meses", "6 meses"}, {"Mensualidad", "$4,000.00", "$2,000.00"},
           {"Intereses", "$0.00", "$0.00"}, {"Total", "$12,000.00", "$12,000.00"}})->setMinimumHeight(290);
    comp->addWidget(texto("Menor mensualidad: opción a 6 meses.\nTotal por pagar: ambas opciones cuestan lo mismo. Importes en MXN.", "muted"));
    comp->addStretch();

    pantallas->addWidget(new PantallaAyuda);
}
