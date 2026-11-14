#include <memory>
#include "AvatarUsuario.h"
#include "FotoPerfil.h"
#include "EditarPerfil.h"
#include <QCryptographicHash>
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
    label->setTextFormat(Qt::PlainText);
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
    auto *avatarLateral=new AvatarCliente;avatarLateral->setFixedSize(56,56);menu->addWidget(avatarLateral);
    auto *saludoLateral=texto("Bienvenido, Usuario","section");menu->addWidget(saludoLateral);
    menu->addSpacing(8);
    menu->addWidget(texto("ESPACIO DE TRABAJO", "eyebrow"));
    auto *pantallas = new QStackedWidget;
    auto *grupo = new QButtonGroup(this);
    const QStringList nombres = {"Inicio · Simulador", "Historial", "Comparador", "Acerca de", "Mi cuenta"};
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
    connect(boton(menu, "Cerrar sesión"), &QPushButton::clicked, this, [this, pantallas]() {
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
    auto *cuenta=pagina(pantallas,"Mi cuenta","PERFIL  /  Consulta y edita tus datos de demostración.");
    auto *marco=new QFrame;marco->setObjectName("card");cuenta->addWidget(marco);
    auto *perfil=new QVBoxLayout(marco);perfil->setContentsMargins(24,24,24,24);perfil->setSpacing(24);
    auto *cabecera=new QHBoxLayout;cabecera->setSpacing(20);perfil->addLayout(cabecera);
    auto *avatar=new AvatarCliente;avatar->setFixedSize(80,80);auto *camara=botonFoto();cabecera->addWidget(avatarEditable(avatar,camara));
    auto *identidad=new QVBoxLayout;cabecera->addLayout(identidad,1);
    identidad->addWidget(texto("MI PERFIL","eyebrow"));
    auto *nombrePerfil=texto("","title");identidad->addWidget(nombrePerfil);
    identidad->addWidget(texto("Cuenta de cliente","muted"));
    auto *editar=new QPushButton;editar->setFixedSize(44,44);editar->setObjectName("primary");
    editar->setToolTip("Editar mis datos");editar->setAccessibleName("Editar mis datos");editar->setCursor(Qt::PointingHandCursor);
    QPixmap lapiz(24,24);lapiz.fill(Qt::transparent);
    {QPainter p(&lapiz);p.setRenderHint(QPainter::Antialiasing);p.setPen(QPen(QColor("#17100e"),1.8,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));p.setBrush(Qt::NoBrush);
     QPolygonF contorno;contorno<<QPointF(4,16)<<QPointF(16,4)<<QPointF(20,8)<<QPointF(8,20)<<QPointF(3,21)<<QPointF(4,16);p.drawPolyline(contorno);p.drawLine(QPointF(13,7),QPointF(17,11));}
    editar->setIcon(QIcon(lapiz));editar->setIconSize(QSize(24,24));cabecera->addWidget(editar);
    auto *campos=new QGridLayout;campos->setHorizontalSpacing(32);campos->setVerticalSpacing(10);perfil->addLayout(campos);
    campos->addWidget(texto("CORREO ELECTRÓNICO","caption"),0,0);
    campos->addWidget(texto("FECHA DE NACIMIENTO","caption"),0,1);
    auto *correoPerfil=texto("","section");auto *fechaPerfil=texto("","section");
    campos->addWidget(correoPerfil,1,0);campos->addWidget(fechaPerfil,1,1);
    campos->addWidget(texto("CONTRASEÑA","caption"),2,0);campos->addWidget(texto("••••••••","section"),3,0);
    campos->addWidget(texto("Puedes cambiarla desde el botón de edición.","muted"),3,1);
    struct PerfilDemo {QString nombre="Cliente de muestra",correo="cliente@demo.mx";QDate nacimiento{2000,1,1};QByteArray clave;QPixmap foto;};
    auto modelo=std::make_shared<PerfilDemo>();
    connect(camara,&QPushButton::clicked,this,[this,modelo,avatar,avatarLateral](){if(editarFoto(this,modelo->foto,[](){return true;})){avatar->setFoto(modelo->foto);avatarLateral->setFoto(modelo->foto);}});

    auto actualizar=[modelo,nombrePerfil,correoPerfil,fechaPerfil,saludoLateral](){saludoLateral->setText("Bienvenido, "+(modelo->nombre=="Cliente de muestra"?QString("Usuario"):modelo->nombre));nombrePerfil->setText(modelo->nombre);correoPerfil->setText(modelo->correo);fechaPerfil->setText(modelo->nacimiento.toString("dd/MM/yyyy"));};actualizar();
    connect(editar,&QPushButton::clicked,this,[this,modelo,actualizar](){
        if(editarPerfil(this,modelo->nombre,modelo->correo,modelo->nacimiento,[modelo](const QString &,const QString &clave){if(!clave.isEmpty())modelo->clave=QCryptographicHash::hash(clave.toUtf8(),QCryptographicHash::Sha256);return true;}))actualizar();
    });
    perfil->addWidget(texto("Cambios temporales de demostración. No modifican credenciales de acceso reales.","caption"));
    cuenta->addStretch();

    connect(interpretar,&QPushButton::clicked,this,[pantallas,ayuda]() {
        ayuda->mostrarTablaAmortizacion();
        pantallas->setCurrentWidget(ayuda);
    });
}
