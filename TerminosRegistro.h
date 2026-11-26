#pragma once
#include "Acceso.h"
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QTimer>

inline bool mostrarTerminos(QWidget *padre) {
    QDialog dialogo(padre);
    dialogo.setWindowTitle("Términos y condiciones · FinancionantiMex");
    dialogo.resize(680,620);
    auto *layout=new QVBoxLayout(&dialogo);
    layout->setContentsMargins(24,24,24,24);layout->setSpacing(12);
    auto *titulo=new QLabel("Términos y condiciones");titulo->setObjectName("title");layout->addWidget(titulo);
    auto *indicacion=new QLabel("Recorre el documento hasta el final para habilitar la aceptación en el registro.");
    indicacion->setWordWrap(true);indicacion->setObjectName("muted");layout->addWidget(indicacion);
    auto *texto=new QTextBrowser;
    texto->setOpenExternalLinks(false);
    texto->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    texto->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    texto->setStyleSheet("QTextBrowser { background:#1b1b20; color:#f4f4f5; border:1px solid #484248; border-radius:8px; padding:14px; }");
    texto->setHtml(QString::fromUtf8(R"TERMINOS(
<h2>FinancionantiMex</h2><p>Versión %1 · 17 de noviembre de 2026</p>
<h3>1. Finalidad del sistema</h3>
<p>FinancionantiMex permite simular préstamos, consultar tablas de amortización, guardar simulaciones y comparar opciones de financiamiento. Los resultados son informativos y dependen de los datos ingresados. <b>No constituyen una oferta, aprobación ni garantía de obtener un préstamo.</b> El sistema no entrega dinero ni celebra contratos de crédito.</p>
<h3>2. Registro y responsabilidad sobre la cuenta</h3>
<p>Para crear una cuenta debes tener al menos 18 años y proporcionar información veraz. Eres responsable de mantener actualizados tu nombre, apellidos, correo electrónico y fecha de nacimiento.</p>
<p>Tu cuenta es personal. Protege tu contraseña y no la compartas. Si sospechas que alguien la conoce y aún puedes acceder, cámbiala desde «Mi cuenta».</p>
<p>No existe un chat ni un canal de comunicación directa entre clientes y administradores. La apelación está destinada exclusivamente a solicitar la revisión de una cuenta deshabilitada.</p>
<h3>3. Uso permitido y contenido prohibido</h3>
<p>Utiliza el sistema de manera respetuosa y para sus funciones de simulación y consulta. No está permitido:</p>
<ul><li>Escribir palabras obscenas, insultos, amenazas, expresiones discriminatorias o mensajes de acoso en simulaciones, perfiles o apelaciones.</li><li>Utilizar nombres, textos o imágenes que promuevan o faciliten actividades ilícitas.</li><li>Usar fotografías de perfil con contenido sexual explícito, violento, discriminatorio u ofensivo.</li><li>Suplantar identidades o utilizar datos ajenos sin autorización.</li><li>Acceder a cuentas ajenas, modificar información sin permiso o afectar el funcionamiento del sistema.</li></ul>
<h3>4. Consulta de información por los administradores</h3>
<p><b>Los administradores pueden consultar los datos de tu cuenta y tus simulaciones guardadas</b>, incluidos nombres, montos, tasas de interés, plazos y tablas de amortización. Este acceso permite gestionar cuentas, revisar estas condiciones y atender apelaciones. El contenido de tus simulaciones no está oculto al equipo administrador.</p>
<p>Tus contraseñas no deben mostrarse en fichas ni registros administrativos.</p>
<h3>5. Edición y eliminación de simulaciones</h3>
<p><b>Los administradores pueden editar o eliminar simulaciones que incumplan estas condiciones</b>, especialmente si contienen palabras obscenas, amenazas, insultos, discriminación o contenido que promueva actividades ilícitas.</p>
<p>La revisión deberá considerar el contexto. Una sospecha o palabra aislada no demuestra por sí misma una actividad ilegal. Las modificaciones y eliminaciones deberán tener un motivo concreto y quedar registradas en la actividad administrativa.</p>
<p>Cambiar el monto, tasa o plazo puede modificar los resultados y la tabla de amortización. Una simulación eliminada dejará de estar disponible para consulta, comparación o exportación. Esto no afecta a los PDF descargados previamente.</p>
<h3>6. Deshabilitación de cuentas</h3>
<p>Los administradores pueden <b>deshabilitar tu cuenta</b> por incumplimiento de estas condiciones, uso inapropiado o situaciones que comprometan la seguridad. Mientras esté deshabilitada no podrás utilizar sus funciones habituales. Al intentar ingresar se mostrará un aviso y la opción de apelar.</p>
<p>La deshabilitación no implica por sí misma eliminar tus datos ni simulaciones.</p>
<h3>7. Inactividad</h3>
<p>Una cuenta puede clasificarse como inactiva tras seis meses sin iniciar sesión. <b>Esto no la deshabilita automáticamente.</b> Si un administrador decide deshabilitarla por inactividad, deberá registrar la razón y podrás solicitar su revisión mediante una apelación.</p>
<h3>8. Apelaciones</h3>
<p>Si tu cuenta está deshabilitada, podrás apelar desde la opción habilitada al intentar acceder. Explica clara y respetuosamente por qué solicitas su reactivación. No incluyas contraseñas, datos bancarios ni información confidencial innecesaria.</p>
<p>Los administradores podrán aprobar la solicitud y reactivar tu cuenta, o rechazarla, mantener la deshabilitación y registrar el motivo. Apelar no garantiza la aprobación. Esta función no es un chat ni un servicio de mensajería directa.</p>
<h3>9. Datos personales y privacidad</h3>
<p>Los datos de tu cuenta permiten identificarte y ofrecer las funciones del sistema. Los administradores autorizados pueden consultar la información necesaria para gestionar cuentas, revisar simulaciones y resolver apelaciones.</p>
<p>No introduzcas números de tarjetas, datos bancarios, documentos oficiales u otra información sensible en simulaciones, fotografías o campos de texto.</p>
<p>El tratamiento de datos, plazos de conservación y mecanismos para ejercer los derechos correspondientes deberán detallarse en un <b>aviso de privacidad independiente</b>. Aceptar estos términos no sustituye dicho aviso.</p>
<h3>10. Simulaciones y documentos exportados</h3>
<p>Los resultados son estimaciones. Pueden diferir de un préstamo real por comisiones, seguros, impuestos, redondeos u otras condiciones no contempladas.</p>
<p><b>Los PDF no acreditan la aprobación de un préstamo, una deuda real ni una relación contractual con una institución financiera.</b> Revisa los datos y las condiciones reales antes de tomar decisiones.</p>
<h3>11. Registro de acciones administrativas</h3>
<p>El sistema contempla registrar modificaciones y eliminaciones de simulaciones, deshabilitaciones de cuentas y resoluciones de apelaciones. Todos los administradores podrán consultar estas acciones y sus responsables. El registro no deberá mostrar contraseñas ni folios de autorización.</p>
<h3>12. Actualización de los términos</h3>
<p>Estos términos podrán actualizarse cuando cambien las funciones o reglas del sistema. La versión vigente deberá indicar su fecha de actualización. Los cambios relevantes deberán comunicarse dentro de la aplicación antes de aplicarse.</p>
<h3>13. Aceptación</h3>
<p>Al aceptar, declaras haber leído y comprendido estas reglas. En particular, reconoces que <b>los administradores pueden consultar tus simulaciones, editarlas o eliminarlas por incumplimientos y deshabilitar tu cuenta</b>, y que puedes solicitar la revisión de una deshabilitación mediante una apelación.</p>
<p><b>Fin del documento.</b> Vuelve al registro y marca la casilla si aceptas estos términos.</p>
)TERMINOS").arg(VERSION_TERMINOS));
    layout->addWidget(texto,1);
    auto *acciones=new QHBoxLayout;
    auto *cerrar=new QPushButton("Cerrar");auto *continuar=new QPushButton("Volver al registro");
    continuar->setObjectName("primary");continuar->setEnabled(false);
    acciones->addWidget(cerrar);acciones->addWidget(continuar);layout->addLayout(acciones);
    QObject::connect(cerrar,&QPushButton::clicked,&dialogo,&QDialog::reject);
    QObject::connect(continuar,&QPushButton::clicked,&dialogo,&QDialog::accept);
    auto revisar=[=](){auto *barra=texto->verticalScrollBar();if(barra->value()>=barra->maximum()){continuar->setEnabled(true);indicacion->setText("Llegaste al final. Puedes volver al registro y marcar la casilla si aceptas.");}};
    QObject::connect(texto->verticalScrollBar(),&QScrollBar::valueChanged,&dialogo,revisar);
    QTimer::singleShot(0,&dialogo,revisar);
    return dialogo.exec()==QDialog::Accepted;
}
