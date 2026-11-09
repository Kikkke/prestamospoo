#pragma once
#include <QString>
#include <QList>
#include <QPair>

// Contenido educativo independiente de la lógica de cálculo del futuro simulador.
inline QList<QPair<QString, QString>> guiaPrestamos()
{
    return {
        {"1. ¿Qué hace este programa?",
         "Un préstamo es dinero que recibes con el compromiso de devolverlo en un plazo acordado. Normalmente también pagas intereses por usarlo. Este programa busca ayudarte a entender esos pagos y comparar condiciones antes de decidir. No entrega dinero ni contrata préstamos.\n\n"
         "ESTADO ACTUAL: estás usando un prototipo visual. Iniciar sesión y Crear cuenta abren el inicio sin validar ni guardar cuentas. Puedes recorrer las pantallas y editar los campos, pero los cálculos, el guardado y la comparación automática todavía no están disponibles. Las cifras visibles son ejemplos fijos: no cambian al modificar los campos.\n\n"
         "El modelo previsto usa pesos mexicanos (MXN), tasa fija, pagos mensuales y amortización sobre el saldo pendiente. No contempla comisiones, seguros, impuestos, atrasos ni abonos extraordinarios. Por eso su total no representa necesariamente el costo de un crédito real."},
        {"2. Cómo recorrer el software",
         "ACCESO Y REGISTRO\nPulsa Iniciar sesión para entrar al prototipo. Si eliges Crear una cuenta, verás nombre, correo y contraseña; el botón Crear cuenta también abre el inicio. Actualmente no necesitas proporcionar datos reales y no se crea una cuenta.\n\n"
         "INICIO / SIMULADOR\nNombre: una etiqueta para reconocer tu plan, como ‘Computadora’. Monto: el dinero que pedirías, no el precio completo del producto si ya pagarás una parte. Tasa nominal anual: escribe el porcentaje anual, por ejemplo 12 para 12 %, no 0.12. Plazo: número de meses, por ejemplo 3. El programa no solicita CAT en el campo de tasa.\n\n"
         "A la derecha están la mensualidad, los intereses y el total. Debajo aparece la tabla de amortización. Hoy muestran siempre el ejemplo de $12,000, tasa de 0 % y tres meses, aunque captures otros valores. Calcular y Guardar están deshabilitados; cuando se implementen, actualizarán los resultados y permitirán conservar una simulación.\n\n"
         "HISTORIAL\nMuestra 20 registros ilustrativos. Busca por nombre sin importar mayúsculas ni acentos, o pulsa un encabezado para ordenar. La X de cada fila pide confirmación antes de quitarla. Cancelar conserva el registro; Eliminar lo retira durante esta ejecución. Al reiniciar vuelven los ejemplos. Estos cambios no modifican el simulador ni el comparador.\n\n"
         "COMPARADOR\nPuedes elegir opciones en las listas, pero la tabla es fija por ahora. El ejemplo contrasta $12,000 sin intereses a tres y seis meses: $4,000 frente a $2,000 mensuales, con el mismo total de $12,000.\n\n"
         "ACERCA DE Y SALIDA\nAquí puedes consultar esta guía. Usa la rueda del ratón o la barra de desplazamiento para seguir leyendo. Volver al acceso te regresa al formulario inicial; no hay una sesión autenticada ni información guardada."},
        {"3. La tasa de interés, explicada paso a paso",
         "La tasa es un porcentaje; el interés es una cantidad de dinero. Decir ‘1 % mensual’ no significa pagar $1: significa $1 por cada $100 de saldo durante ese mes. Para convertir un porcentaje a decimal se divide entre 100: 1 % = 0.01.\n\n"
         "En el modelo de este programa, una tasa NOMINAL anual de 12 % se divide entre 12 meses: 12 % / 12 = 1 % mensual. En decimal: 12 / 100 / 12 = 0.01. Esta conversión no sirve para una tasa efectiva anual ni para el CAT.\n\n"
         "Si al comenzar el mes debes $12,000, el interés de ese mes es $12,000 × 0.01 = $120. Si tu pago es $4,080.27, primero se cubren esos $120 y los $3,960.27 restantes reducen la deuda. El nuevo saldo es $8,039.73.\n\n"
         "El mes siguiente se aplica el mismo 1 %, pero al saldo menor: $8,039.73 × 0.01 ≈ $80.40. Aunque la tasa no cambie, los intereses en pesos bajan al disminuir la deuda. De tu pago se destina cada vez más a capital.\n\n"
         "Por eso no basta multiplicar monto × tasa anual × años para calcular este tipo de préstamo: esa operación supone una base que no se reduce. Aquí el saldo cambia después de cada pago."},
        {"4. Ejemplo completo: $12,000 al 12 % anual, en 3 meses",
         "Este es un ejemplo educativo diferente al de 0 % que muestra el tablero. Supone tasa nominal anual fija, pagos puntuales cada mes y ningún cargo adicional.\n\n"
         "Mes 1: saldo inicial $12,000.00; interés $120.00; pago $4,080.27; abono a capital $3,960.27; saldo final $8,039.73.\n\n"
         "Mes 2: saldo inicial $8,039.73; interés $80.40; pago $4,080.27; abono a capital $3,999.87; saldo final $4,039.86.\n\n"
         "Mes 3: saldo inicial $4,039.86; interés $40.40; pago $4,080.26; abono a capital $4,039.86; saldo final $0.00.\n\n"
         "En total devuelves $12,240.80: $12,000.00 de capital y $240.80 de intereses. El último pago es un centavo menor para cerrar el saldo por el redondeo a centavos.\n\n"
         "¿Y si la tasa fuera 0 %? Dividirías $12,000 entre 3: serían tres pagos de $4,000, intereses de $0 y total de $12,000. En este modelo sin cargos, todos los pagos irían a capital."},
        {"5. Qué cambia al modificar monto, tasa o plazo",
         "MÁS MONTO: manteniendo tasa y plazo, aumenta la mensualidad y, si la tasa es positiva, también los intereses en pesos.\n\n"
         "MÁS TASA: manteniendo monto y plazo, aumentan los intereses y el pago mensual. Una tasa de 12 % anual no equivale a una tasa de 12 % mensual: la segunda es mucho mayor.\n\n"
         "MÁS PLAZO: manteniendo monto y una tasa positiva, normalmente baja la mensualidad, pero aumenta el interés total porque debes dinero durante más tiempo.\n\n"
         "Ejemplo calculado sin cargos, con $12,000 al 12 % nominal anual: a 12 meses, la mensualidad aproximada es $1,066.19 y los intereses totales rondan $794.23; a 24 meses, la mensualidad aproximada es $564.88 y los intereses rondan $1,557.16. Los totales usan el cálculo antes del redondeo de cada pago; una tabla redondeada puede variar algunos centavos.\n\n"
         "MENOR PAGO NO SIEMPRE SIGNIFICA MENOR COSTO. Revisa mensualidad, plazo y total juntos. Compara preferentemente el mismo monto y los mismos cargos incluidos. Si los montos son distintos, un total menor no demuestra por sí solo que una oferta sea más barata."},
        {"6. Cómo leer la tabla y de dónde sale la mensualidad",
         "Cada fila representa un mes. Saldo inicial: lo que debes antes del pago. Interés: el costo por ese mes. Capital: la parte del pago que reduce la deuda. Pago: capital más interés. Saldo final: lo que queda por devolver. El saldo final de una fila pasa a ser el inicial de la siguiente.\n\n"
         "En el ejemplo sin cargos se cumplen estas relaciones: pago = interés + abono a capital; saldo final = saldo inicial − abono a capital. La suma del capital debe recuperar el monto prestado y el último saldo debe quedar en cero.\n\n"
         "El método previsto es el sistema francés: se calcula una cuota mensual constante, salvo un posible ajuste final por centavos. Al principio una mayor parte de cada cuota va a intereses; después, más va a capital.\n\n"
         "Si quieres conocer la fórmula: M = P × i / [1 − (1 + i)^(−n)]. P es el monto, i la tasa mensual en decimal y n el número de meses. El símbolo ^ significa elevar a una potencia. Con tasa de 0 %, se usa M = P / n. No necesitas resolver esta fórmula para usar la interfaz.\n\n"
         "La precisión depende de las reglas de cálculo. Un crédito real puede usar días exactos, otros calendarios, cargos y reglas contractuales distintas; el simulador no los reproduce."}
    };
}

inline QList<QPair<QString, QString>> glosarioPrestamos()
{
    return {
        {"Préstamo", "Dinero que alguien entrega a otra persona con el compromiso de devolverlo bajo condiciones acordadas. Ejemplo: recibir $12,000 y pagarlos durante tres meses."},
        {"Crédito", "Acuerdo para disponer de dinero o financiar una compra y devolverlo después. Puede ser un préstamo con plazo definido o una línea reutilizable, como una tarjeta. Este proyecto solo representa préstamos de pagos mensuales definidos."},
        {"Prestamista o acreedor", "Persona o institución a la que se le debe el dinero."},
        {"Prestatario, acreditado o deudor", "Persona que recibe el dinero y se compromete a devolverlo."},
        {"Monto o capital inicial", "Cantidad prestada al comienzo. No incluye los intereses futuros. Si pides $12,000, ese es el capital inicial."},
        {"Saldo pendiente o saldo insoluto", "Capital que todavía no has devuelto. No es la suma de todos los pagos futuros, porque esos pagos también pueden incluir intereses."},
        {"Plazo", "Tiempo acordado para terminar de pagar. En el programa se captura en meses: 12 meses equivalen a un año."},
        {"Periodicidad", "Frecuencia de los pagos: semanal, quincenal, mensual, etc. Aquí se considera un pago al mes."},
        {"Tasa de interés", "Porcentaje usado para calcular el costo de utilizar el dinero durante un periodo. Siempre debe indicar si es mensual, anual u otro. El mismo número puede significar costos muy distintos según el periodo."},
        {"Interés ordinario", "Importe que se paga por usar el capital según las condiciones normales del préstamo. En este modelo se obtiene multiplicando el saldo inicial del mes por la tasa mensual."},
        {"Tasa fija", "Porcentaje que se mantiene durante el plazo pactado. No significa que el interés en pesos sea siempre igual: cambia al disminuir el saldo."},
        {"Tasa variable", "Porcentaje que puede cambiar según las condiciones del contrato y una referencia. No se contempla en este simulador."},
        {"Tasa nominal anual", "Forma de expresar una tasa anual cuya conversión al periodo debe especificarse. Aquí se supone conversión mensual: tasa nominal anual dividida entre 12. Es el dato que solicita el formulario."},
        {"Tasa mensual", "Porcentaje aplicable a un mes. Bajo el supuesto del programa, 12 % nominal anual corresponde a 1 % mensual."},
        {"Tasa efectiva anual", "Tasa que expresa el efecto acumulado de la capitalización durante un año. Por ejemplo, 1 % mensual capitalizado equivale aproximadamente a 12.68 % efectivo anual: (1.01^12 − 1) × 100. No es el porcentaje del monto que necesariamente pagarás en intereses si vas amortizando la deuda, ni es el CAT."},
        {"Interés simple", "Interés calculado sobre un capital que se mantiene como base, sin sumar intereses al capital para generar otros intereses. No describe por sí solo el calendario de pagos de este simulador."},
        {"Interés compuesto o capitalización", "Se produce cuando los intereses se incorporan al capital y también generan intereses. En el ejemplo de cuotas puntuales se paga el interés de cada mes; no se acumulan intereses vencidos al saldo."},
        {"Cuota, mensualidad o pago", "Dinero que se entrega en cada periodo. Aquí incluye capital e interés. En un crédito real también puede incluir seguros, comisiones o impuestos."},
        {"Abono a capital", "Parte del pago que reduce lo prestado. Si pagas $4,080.27 y $120 son intereses, $3,960.27 se destinan a capital."},
        {"Amortización", "Reducción gradual de la deuda mediante abonos a capital. La tabla muestra cómo sucede mes a mes."},
        {"Sistema francés o cuota fija", "Método que calcula pagos periódicos constantes con tasa fija. La proporción de capital e intereses cambia en cada pago y puede ajustarse la última cuota por redondeo."},
        {"Intereses totales", "Suma de los intereses de todas las mensualidades. Es una cantidad en pesos, no una tasa."},
        {"Total por pagar", "Suma de los pagos del préstamo. En este modelo equivale a capital inicial más intereses totales; no incluye costos adicionales."},
        {"Comisión", "Cargo por un servicio u operación relacionado con el crédito, por ejemplo su apertura. Puede ser una cantidad fija o un porcentaje. No se incluye en los ejemplos del programa."},
        {"Seguro e impuestos", "El seguro cubre riesgos según una póliza y puede tener un costo. Los impuestos son cargos fiscales aplicables según la operación. No se calculan aquí."},
        {"CAT: Costo Anual Total", "Indicador expresado como porcentaje anual para comparar el costo del financiamiento. Considera intereses y costos asociados, como ciertas comisiones y seguros, conforme a su metodología; no incluye el IVA aplicable. No es la tasa nominal ni el total en pesos. El programa no calcula CAT: no lo escribas como tasa de interés."},
        {"Fecha de vencimiento y mora", "El vencimiento es la fecha límite acordada para pagar. La mora es el retraso en el cumplimiento de ese pago. El prototipo no asigna fechas ni simula atrasos."},
        {"Interés moratorio", "Interés asociado al atraso, según las condiciones del contrato. Es distinto del interés ordinario y no se considera en el programa."},
        {"Pago anticipado o abono extraordinario", "Dinero adicional que se entrega antes de lo previsto. Si se aplica a capital, puede modificar el plazo o la cuota según las reglas del crédito. No equivale necesariamente a adelantar mensualidades y no está implementado aquí."},
        {"Enganche, garantía y aval", "El enganche es una parte del precio que pagas con recursos propios. Una garantía respalda una obligación; un aval puede asumir obligaciones de pago según lo pactado. El simulador no solicita ni evalúa estos elementos."},
        {"Capacidad de pago", "Dinero disponible para cubrir cuotas después de considerar gastos y otros compromisos. El programa no analiza ingresos ni determina si puedes asumir un préstamo."},
        {"Simulación y redondeo", "Una simulación es una estimación basada en supuestos. Redondear significa expresar los importes en centavos; pequeñas diferencias pueden requerir ajustar el último pago para que el saldo final sea cero."}
    };
}
