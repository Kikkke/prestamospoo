# Criterios visuales del prototipo

## Referencias y decisiones

El dashboard aprobado y el login existente son la referencia principal. El documento
DESIGN.md del usuario (Raycast — Style Reference) aporta detalles secundarios.
La consulta de estilos de Refero no estuvo disponible por suscripción inactiva.

| Decisión | Referencia | Aplicación |
| --- | --- | --- |
| Mantener negro, blanco y cobre #ED916F | Petición explícita del usuario y login aprobado | Sin sustituir por coral o azul de Raycast. |
| Fondo compartido | Login aprobado | FondoAcceso pinta la misma cinta y halo en Inicio, Historial, Comparador y Acerca de. |
| Conservar distribución del dashboard | Captura de referencia y aprobación del usuario | Formulario y métricas lado a lado; amortización debajo. |
| Bordes finos y relieve discreto | Feature Card / Edge-Highlight Card de DESIGN.md | Radio 16 px, borde superior más claro e inferior oscuro; sin sombras grandes. |
| Ritmo de espaciado | Escala de 8 px de DESIGN.md | Márgenes 24 px, separación de bloques 16 px y controles 8 px. |
| Legibilidad de datos | Roles de superficies de DESIGN.md | Tarjetas casi opacas y tablas oscuras sobre el fondo decorativo. |

Se conserva Segoe UI como tipografía nativa de Windows. El documento se utiliza
como referencia, no como instrucciones para reemplazar el diseño o agregar funciones.

## Comparador
Referencia principal: dashboard aprobado y captura del comparador. Se mantienen el fondo, Segoe UI y el cobre. Las tarjetas de Acerca de aportan bordes y espaciado. Dos opciones paralelas destacan la mensualidad; tres resúmenes separan pago, total y plazo sin declarar una opción universalmente mejor. Iconos vectoriales solo en búsquedas y resúmenes. Tabla sin cuadrícula, valores alineados a la derecha. Seis ejemplos independientes y resultados precargados; estados explícitos para selección incompleta, empate, mismo ejemplo y distinto monto.

## Identidad FinancionantiMex
Nombre y eslogan proporcionados por el usuario: FinancionantiMex / Una forma impresionanti de obtener un préstamo. MarcaFinancionanti.h dibuja un emblema vectorial basado en el boceto de bolsa, moneda y cuernos; conserva cobre y fondo transparente. La identidad aparece en acceso, registro y menú compartido, con el emblema como icono de ventana. Se conserva la ilustración del acceso y el fondo original.

## Administración y apelaciones
Referencias: capturas del usuario, tarjetas de Clientes y dashboard aprobado. Se conserva el fondo original y la paleta cobre. Administradores usa dos columnas de tarjetas con avatar vectorial circular, distintivo de sesión y acciones compactas; el alta es la acción principal de la barra de búsqueda. Apelaciones separa el resumen por estado, los filtros y el mensaje del cliente, con aprobar como acción principal y rechazar como secundaria. Las fichas administrativas requieren el folio de la sesión activa; no muestran contraseñas ni folios de otras cuentas.

Clientes adopta la referencia aprobada de Administradores: avatar de 56 px, distintivo de estado, tarjetas de dos columnas, márgenes de 20 px y acciones compactas. La ficha agrupa identidad y estado en un encabezado; separa monto, tasa y plazo en columnas y conserva la confirmación por folio para cambios.
