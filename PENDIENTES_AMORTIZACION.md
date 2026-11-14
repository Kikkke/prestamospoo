# Acuerdos para la próxima sesión

El usuario pidió guardar esta propuesta y comenzar a codificarla en la siguiente sesión. No implementar todavía por este acuerdo. Seguimos en la etapa de frontend: separar la demostración visual de la futura lógica y persistencia.

## Historial y comparador

- Cada simulación guardada deberá conservar condiciones, resultados y tabla de amortización.
- Añadir una columna Acciones en el historial: botón con icono de tabla y ayuda «Ver tabla de amortización», junto al botón de eliminación con confirmación.
- Cada botón abre la tabla correspondiente a su propia simulación.
- En la integración futura, el comparador permitirá elegir dos simulaciones del historial del usuario que inició sesión. Actualmente ambos apartados tienen muestras independientes.

## Ventana de amortización

- Conservar fondo oscuro, tarjetas y acentos cobre del sistema.
- Encabezado con icono de tabla, nombre de la simulación, fecha de guardado y cierre o regreso al historial.
- Condiciones de consulta: monto, tasa anual, plazo y frecuencia de pago.
- Tres tarjetas de resumen: pago mensual, intereses totales y total por pagar; destacar mensualidad en cobre.
- Columnas: número de pago, saldo inicial, mensualidad, intereses, abono a capital y saldo final.
- Importes alineados a la derecha, separadores de miles y dos decimales.
- Alternancia sutil de filas, resaltado al pasar el mouse, encabezados fijos y ayudas para los conceptos financieros.
- Totales solo para pagos, intereses y abonos a capital; no sumar saldos.
- Desplazamiento continuo sin paginación de pagos. En anchos pequeños apilar tarjetas y mantener el desplazamiento dentro de la tabla sin ensanchar la ventana.
- Pie con moneda MXN, conceptos incluidos en el cálculo y enlace «¿Cómo interpreto esta tabla?» a Acerca de.

## Exportación PDF aprobada

- Botón «Exportar PDF» en la ventana de amortización.
- Incluir logo de FinancionantiMex, nombre y fecha de la simulación, condiciones, resumen de resultados y tabla completa.
- Repetir encabezados de tabla en cada página y numerar las páginas.
- Diseño para impresión: fondo blanco, texto oscuro y detalles cobre.
- Al implementar, aclarar si se exportan datos ilustrativos o resultados reales. No presentar el prototipo como cálculo o persistencia terminados.

## Avance del 13/11/2026

Implementados el acceso por fila del historial, ventana de amortización y exportación PDF paginada mediante QPdfWriter. Las cuotas se calculan localmente a partir de las condiciones ilustrativas de cada fila, con ajuste final de centavos. Compilación correcta. Pendiente revisión visual en ejecución y del PDF exportado. Persistencia real, fecha de guardado y conexión historial/comparador siguen pendientes.
