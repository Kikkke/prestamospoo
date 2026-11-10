#include "PanelPrincipal.h"
#include "FondoAcceso.h"
#include "MarcaFinancionanti.h"
#include "PantallaAyuda.h"
#include "HistorialSimulaciones.h"
#include "ComparadorSimulaciones.h"
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
    menu->addWidget(logoFinancionanti(64));
    menu->addWidget(texto("FinancionantiMex", "brandSidebar"));
    menu->addWidget(texto("Una forma impresionanti de obtener un préstamo", "caption"));
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
    form->addWidget(texto("01 · IDENTIFICA TU PLAN", "eyebrow"));
    auto *nombre = new QLineEdit("Computadora");
    campo(form, "Nombre de la simulación", nombre);
    nombre->setPlaceholderText("Por ejemplo: computadora o estudios");
    form->addSpacing(8);
    form->addWidget(texto("02 · MONTO Y TASA", "eyebrow"));
    auto *condiciones = new QHBoxLayout;
    condiciones->setSpacing(16);
    form->addLayout(condiciones);
    auto *colMonto = new QVBoxLayout;
    auto *colTasa = new QVBoxLayout;
    condiciones->addLayout(colMonto, 1);
    condiciones->addLayout(colTasa, 1);
    auto campoIcono = [](QVBoxLayout *col, const QString &titulo, QWidget *control, int tipo) {
        auto *fila = new QHBoxLayout;
        auto *icono = new IconoTema(tipo); icono->setFixedSize(22,22);
        fila->addWidget(icono);
        auto *label = texto(titulo); label->setBuddy(control);
        fila->addWidget(label,1);
        col->addLayout(fila); col->addWidget(control);
    };
    auto *monto = new QDoubleSpinBox;
    monto->setRange(0.01, 999999999.99);
    monto->setPrefix("$ "); monto->setSuffix(" MXN"); monto->setValue(12000);
    monto->setMinimumWidth(0); monto->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Fixed);
    campoIcono(colMonto, "Monto solicitado", monto, 8);
    auto *tasa = new QDoubleSpinBox;
    tasa->setRange(0, 1000); tasa->setSuffix(" %");
    campoIcono(colTasa, "Tasa nominal anual", tasa, 2);
    form->addWidget(texto("Escribe 12 para una tasa anual del 12 %. No introduzcas el CAT.", "caption"));
    form->addSpacing(8);
    form->addWidget(texto("03 · ELIGE EL PLAZO", "eyebrow"));
    auto *meses = new QSpinBox;
    meses->setRange(1, 600); meses->setSuffix(" meses"); meses->setValue(3);
    campoIcono(form, "Tiempo para pagar", meses, 9);
    auto *plazos = new QHBoxLayout;
    plazos->setSpacing(8); form->addLayout(plazos);
    for(int plazo : {3,6,12,24}) {
        auto *rapido = boton(plazos, QString::number(plazo)+" meses");
        rapido->setObjectName("quickTerm"); rapido->setCheckable(true);
        rapido->setChecked(plazo==meses->value());
        connect(rapido,&QPushButton::clicked,this,[meses,plazo]() { meses->setValue(plazo); });
        connect(meses,qOverload<int>(&QSpinBox::valueChanged),rapido,[rapido,plazo](int valor) { rapido->setChecked(valor==plazo); });
        // Pulsar el plazo activo debe conservar su selección.
        connect(rapido,&QPushButton::clicked,rapido,[rapido,meses,plazo]() { rapido->setChecked(meses->value()==plazo); });
    }
    boton(form, "Calcular préstamo · Próximamente", true)->setEnabled(false);
    auto *estado = texto("Resultado de muestra: Computadora · $12,000 · 0 % anual · 3 meses.", "sampleNotice");
    form->addWidget(estado);
    auto actualizarAviso = [nombre,monto,tasa,meses,estado]() {
        const bool editado=nombre->text()!="Computadora" || monto->value()!=12000 || tasa->value()!=0 || meses->value()!=3;
        estado->setText(editado
            ? "Cambiaste las condiciones. Los resultados siguen correspondiendo al ejemplo inicial: $12,000 · 0 % anual · 3 meses."
            : "Resultado de muestra: Computadora · $12,000 · 0 % anual · 3 meses.");
    };
    connect(nombre,&QLineEdit::textChanged,this,actualizarAviso);
    connect(monto,qOverload<double>(&QDoubleSpinBox::valueChanged),this,actualizarAviso);
    connect(tasa,qOverload<double>(&QDoubleSpinBox::valueChanged),this,actualizarAviso);
    connect(meses,qOverload<int>(&QSpinBox::valueChanged),this,actualizarAviso);
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
        c->addWidget(texto("MXN · Resultado de muestra", "caption"));
    }
    auto *detalle = tarjeta(sim);
    auto *encabezado = new QHBoxLayout;
    detalle->addLayout(encabezado);
    encabezado->addWidget(texto("Tabla de amortización", "section"));
    encabezado->addStretch();
    encabezado->addWidget(texto("EJEMPLO · 3 MESES · 0 % ANUAL", "badge"));
    auto *amortizacion = tabla(detalle, {"Mes", "Saldo inicial", "Interés", "Capital", "Pago", "Saldo final"},
          {{"1", "12,000.00", "0.00", "4,000.00", "4,000.00", "8,000.00"},
           {"2", "8,000.00", "0.00", "4,000.00", "4,000.00", "4,000.00"},
           {"3", "4,000.00", "0.00", "4,000.00", "4,000.00", "0.00"}});
    amortizacion->setFixedHeight(170);
    amortizacion->setObjectName("amortizationTable");
    amortizacion->setShowGrid(false);
    amortizacion->setSelectionMode(QAbstractItemView::NoSelection);
    for(int r=0;r<amortizacion->rowCount();++r)
        for(int c=0;c<amortizacion->columnCount();++c)
            amortizacion->item(r,c)->setTextAlignment((c==0?Qt::AlignCenter:Qt::AlignRight)|Qt::AlignVCenter);
    const QStringList ayudas={"Número del pago mensual.","Lo que debes al comenzar el mes.",
        "Costo por usar el dinero durante ese mes.","Parte del pago que reduce tu deuda.",
        "Importe que pagas ese mes: capital más interés.","Lo que queda por pagar después del abono."};
    for(int c=0;c<ayudas.size();++c) amortizacion->horizontalHeaderItem(c)->setToolTip(ayudas[c]);
    auto *interpretar = boton(detalle, "¿Cómo se interpreta? →");
    interpretar->setObjectName("helpLink");
    auto *pie = new QHBoxLayout;
    detalle->addLayout(pie);
    pie->addWidget(texto("Importes en MXN. Sin comisiones, seguros, impuestos ni CAT.", "caption"), 1);
    boton(pie, "Guardar simulación")->setEnabled(false);
    sim->addStretch();

    auto *hist = pagina(pantallas, "Historial de simulaciones", "HISTORIAL  /  Registros de muestra. No se han guardado simulaciones reales.", "historial.png");
    hist->addWidget(new HistorialSimulaciones, 1);

    auto *comp = pagina(pantallas, "Compara tus opciones", "COMPARADOR  /  Explora dos ejemplos y descubre cómo cambian sus pagos y plazos.", "comparador.png");
    comp->addWidget(new ComparadorSimulaciones);
    comp->addStretch();
    auto *ayuda = new PantallaAyuda;
    pantallas->addWidget(ayuda);
    connect(interpretar,&QPushButton::clicked,this,[pantallas,ayuda]() {
        ayuda->mostrarTablaAmortizacion();
        pantallas->setCurrentWidget(ayuda);
    });
}
