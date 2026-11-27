# Frontend Qt — diseño aprobado y contrato de las pantallas

Fecha: 25/11/2026. Leer [00](00_SISTEMA_Y_REQUERIMIENTOS.md) para alcance y estado, [02](02_BACKEND_CPP.md) para comportamiento definitivo. Este documento preserva el frontend, no solicita un rediseño.

## 1. Identidad y dirección visual

Aplicación de escritorio C++/Qt Widgets. Referencias principales: login, dashboard y tarjetas ya aprobados por el usuario. El antiguo DESIGN.md con referencia Raycast solo aportó bordes, ritmo y composición; nunca autorizó sustituir la paleta. La referencia de preguntas frecuentes de Samsung inspiró la cuadrícula de Acerca de, no el fondo blanco.

Nombre: FinancionantiMex. Eslogan: «Una forma impresionanti de obtener un préstamo». Logo: bolsa/moneda con símbolo monetario y cuernos, reinterpretada del boceto del usuario; cobre y transparente. Está dibujado en `MarcaFinancionanti.h`, disponible como PNG en `assets/FinancionantiMex-logo.png`, y aparece en acceso, navegación, iconos de ventanas y PDF. No cambiar por un logo genérico.

La ilustración de acceso muestra a una persona con documento, monedas y calculadora, delante de un círculo gris cálido. Afuera es transparente. El usuario rechazó el contorno blanco grueso tipo pegatina: conservar trazos oscuros finos, círculo de fondo y detalles cobre. Login y registro comparten ilustración.

## 2. Paleta y tipografía reales

| Uso | Valor actual / criterio |
|---|---|
| Fondo base | #08090C, casi negro |
| Navegación lateral/superior | #141416 |
| Tarjeta | #171719 o rgba(23,23,25,245) |
| Campo editable | #1B1B20 |
| Texto principal | #F4F4F5 |
| Texto secundario | #B9ADA9; caption #A89F9C |
| Acento, foco y primario | #ED916F, cobre/durazno |
| Primario al pasar mouse | #FFAB89 |
| Texto sobre primario | #17100E |
| Selección lateral | Fondo #33241F, texto #F1A281, borde #624235 |
| Borde controles | #484248 |
| Tabla | #111116, alternas #1B181C, selección #633E31 |
| Encabezado tabla | #222124, texto #C4B9B5 |
| Badge | Fondo #27201E, texto #D4AFA0, borde #49362E |
| Deshabilitado | Fondo #242124, texto #AAA0A0 |

Fuente Segoe UI; base 14 px, títulos 26–28 px, métricas 25–29 px, secundarios 12 px. Tarjetas con radio 16 px, campos/botones 8 px, tarjeta de acceso 20 px. Bordes finos, borde superior más claro, sin sombras grandes. Espaciado basado en 8 px: bloques 16–24, interiores 20–24, controles 8–12.
`Tema.h` centraliza QSS; revisar también estilos locales en componentes antes de corregir un selector. Evitar cambios globales que arreglen un control y rompan login/calendario.

## 3. Fondo, imágenes y recursos

`FondoAcceso.h` pinta la cinta y el halo cobre sobre negro. No sustituirlo por un rectángulo negro plano en Inicio, Historial, Comparador, Acerca de ni administración. Paneles y etiquetas exteriores deben dejar visible el fondo; usar superficies oscuras solo donde ayudan a leer datos.

| Recurso | Uso y conservación |
|---|---|
| assets/acceso.png | Hero compartido login/registro; círculo cálido y persona, sin contorno blanco exterior |
| assets/simulador.png | Calculadora/monedas, cabecera |
| assets/historial.png | Carpeta/documentos/reloj |
| assets/comparador.png | Balanza y dos documentos |
| assets/acerca-de.png | Libro y bombilla |
| assets/FinancionantiMex-logo.png | Versión raster del emblema |
| assets/controles/*.xpm | Flechas blancas y marca de aceptación; empaquetadas en controles.qrc |
| assets/referencias/ | Originales/referencias históricas si están presentes; no borrar con documentación |

Iconos de cabecera simples, imágenes nítidas (originales alrededor de 1024 px), sin texto incrustado ni deformación. Para regenerar: ilustración editorial plana de finanzas, carbón/cobre/marfil, fondo transparente, margen alrededor, sin fotografía ni 3D. Las ilustraciones pequeñas pueden tener trazos claros discretos; el rechazo del usuario es al gran contorno blanco del hero de acceso.
Carga de imágenes: assets junto al ejecutable, después carpeta fuente definida por ASSETS_DIR; confirmar mecanismo en FondoAcceso. Distribuir assets y recursos necesarios; las rutas fuente no sirven en otro equipo. No regenerar imágenes para arreglar un layout.

## 4. Mapa de archivos de interfaz

| Archivo | Responsabilidad |
|---|---|
| main.cpp | QApplication, tema, estilo de selectores |
| VentanaAcceso.cpp/.h | Composición de marca y formulario, contenedor de paneles |
| PantallaLogin.cpp/.h | Correo, contraseña, ojo, navegación |
| PantallaRegistro.cpp/.h | Datos separados, scroll, aceptación y CasillaTerminos |
| TerminosRegistro.h | Documento completo v1 de 17/11/2026 y diálogo de lectura |
| PanelPrincipal.cpp/.h | Barra lateral cliente, Inicio y Mi cuenta, integración de módulos |
| PanelAdministrador.h | Clientes, fichas, administradores, folio, apelaciones |
| HistorialSimulaciones.h | Tabla buscable, acciones por fila |
| VentanaAmortizacion.h | Tabla de detalle, resumen, cálculo local actual, PDF |
| ComparadorSimulaciones.h | Dos opciones, métricas y tabla |
| PantallaAyuda.h / ContenidoAyuda.h | Tarjetas de temas, glosario y explicaciones |
| ActividadAdministrativa.h | Filtros, métricas, lista y detalle de auditoría |
| FiltrosClientes.h | Filtros adaptables y alturas uniformes |
| EditarPerfil.h / CamposNombre.h | Formulario común, nombre y apellidos |
| CampoNacimiento.h | Calendario español con día resaltado al pasar mouse |
| CampoContrasena.h | Mostrar/ocultar contraseña |
| FotoPerfil.h / AvatarUsuario.h | Selección, recorte y avatar circular |
| SelectorDesplegable.h | Lista emergente con scroll y EstiloSelectores |

## 5. Controles y estados comunes

- Hover cobre y foco visible. No confundir hover con selección permanente. Sidebar exclusiva: iluminar solo la ventana actual; volver a pulsarla no la apaga.
- Todos los campos deben permitir leer texto y placeholder sin recortes. Flechas de desplegables, números y calendario blancas sobre fondo oscuro.
- Calendario: selector mes/año, navegación clara y resaltado del día bajo el mouse. Mantener teclado y formato dd/MM/yyyy.
- Filtros: etiquetas transparentes, altura común (40 px en grupos administrativos), lista con barra lateral visible; seis opciones visibles como referencia, sin cortar las restantes. Popup limitado al espacio de la pantalla.
- FiltrosClientes reacomoda 4/2/1 columnas en torno a 950/500 px. Actividad también se adapta. No reintroducir bloques negros detrás de las etiquetas.
- Importe MXN con separadores de miles y dos decimales, alineado a la derecha; fechas localizadas. Ordenar números por valor y no por su cadena formateada.
- Iconos con tooltip y nombre accesible: foto, editar, tabla, eliminar. No añadir iconos decorativos en cada palabra.
- Mensajes de error claros junto al campo cuando sea posible. Diálogos para confirmación/errores de operación; cancelar nunca modifica datos.
- Texto de usuarios se muestra como texto plano (no interpretar HTML). Los términos educativos son contenido controlado.

## 6. Pantallas y contrato visual

### Acceso y registro

Dos zonas: marca/ilustración izquierda y formulario derecha. Fondo compartido. Ventana de referencia 1180×820, mínimo actual 920×700. La tarjeta actual tiene máximo 470 px lógicos; respetar escalado Windows, no deducir medidas directamente de capturas a 125 %.
Login conserva correo y contraseña (no nombres/apellidos para autenticarse), ojo, iniciar sesión, crear cuenta y el detalle amistoso `:)` aprobado. Los atajos Demo fueron retirados: el formulario valida las credenciales reales y solicita folio solo después de verificar la contraseña administrativa.
Registro: Nombre(s), Apellido paterno, Apellido materno, correo, nacimiento, contraseña y confirmación. Los apellidos están en **filas independientes nativas de QFormLayout**: la versión en dos columnas comprimió campos y encimó correo. No regresar a ella sin verificar su geometría.
Scroll **solo dentro del registro** si falta altura; sin borde ni fondo adicional. No envolver login y registro completos con un scroll que agregue marcos grises o encoga el formulario. Ya se quitaron las leyendas «Empieza a explorar…» y «Acceso de demostración» del registro.
Enlace Leer términos → diálogo con scroll vertical → al llegar abajo habilita Volver al registro. Casilla visible 20×20, borde cobre, marca XPM al aceptar, texto en dos líneas si hace falta. No marcarla automáticamente. Clic/espacio antes de completar muestra advertencia y la deja desmarcada. Crear cuenta depende de aceptación y validaciones. Restablecer estado para una nueva alta.

### Navegación y perfil

Sidebar con logo, nombre de la aplicación, foto del propietario o avatar por defecto y «Bienvenido, Nombre(s)». No mostrar apellidos en el saludo; sí en tarjetas/fichas. Ejemplos: Bienvenido, Enrique; Bienvenido, Christian Jared; Bienvenido, Uziel Alexander.
Cliente: Inicio · Simulador, Historial, Comparador, Acerca de, Mi cuenta y **Cerrar sesión**. Administrador: Clientes, Administradores, Apelaciones, **Actividad** y Cerrar sesión. El título de la página puede ser Actividad administrativa, pero el menú dice Actividad.
Mi cuenta usa tarjeta con avatar/cámara, datos con jerarquía, botón de edición con lápiz. Contraseña representada como protegida; no mostrar hash. En ambos campos de cambio de contraseña aparece «Deja vacío si desea conservarla».

### Inicio y simulador

Formulario por pasos, monto/tasa y plazo, accesos rápidos 3/6/12/24 meses. Métricas de mensualidad, intereses y total; mensualidad destacada. Tabla debajo con ayudas en encabezados, moneda MXN y alcance sin cargos adicionales. Calcular y Guardar ya están conectados conservando la composición. Explicación enlaza al tema de amortización.
Cuando cambien condiciones, indicar que se requiere recalcular: no presentar cifras viejas como nuevas. No añadir otro método «interés simple».

### Historial, detalle y PDF

Buscador por nombre, contador, ordenación, tabla con nombre/monto/tasa/plazo/acciones. Columna acciones de referencia 144 px lógicos, con icono tabla y X dentro de la MISMA columna. No restaurar los botones grandes «Consultar ejemplo» ni «Ver confirmación de eliminación».
Toda acción usa el ID asociado a la fila incluso después de ordenar/filtrar. Detalle abre la simulación correcta, con fondo, condiciones, fecha de guardado real, métricas, tabla de pagos y totales. No sumar saldos. Scroll dentro de la tabla; tarjetas apiladas en ventanas estrechas.
Exportación blanca para impresión, texto oscuro, cobre, logo, nombre, condiciones, fecha de emisión y fecha de guardado diferenciadas, tabla completa, encabezados repetidos y páginas numeradas. Actualmente QPdfWriter A4, 22 cuotas/página. Conservar multipágina; no reducir el plazo solo para que quepa en una hoja. Sustituir «EJEMPLO ILUSTRATIVO» al conectar datos reales por «Simulación informativa», no por una afirmación de crédito concedido.

### Comparador

Opciones A/B con selectores buscables. Resúmenes de menor mensualidad, menor total y menor plazo; empates explícitos. Tres columnas Concepto/A/B. Actualización automática al seleccionar registros distintos válidos; no botón extra. Avisar cuando montos son diferentes, sin presentar una opción universalmente mejor. Vacío/menos de dos registros y búsquedas sin resultados deben ser comprensibles.

### Acerca de

Ocho tarjetas completas pulsables con icono, título y breve descripción: alcance, recorrido, tasas, amortización, condiciones, lectura de tablas, glosario de 32 conceptos y fuentes. No adoptar fondo blanco de la referencia. Vistas de lectura con regresar a temas y scroll. Lenguaje sencillo y ejemplos; diferenciar nominal anual, efectiva, CAT, tasa cero y cuota fija. El contenido actual menciona ejemplos independientes: actualizarlo al integrar el historial real.

### Clientes y ficha

Tarjetas siguiendo Administradores: avatar circular de persona, no inicial suelta; nombre completo, correo, último acceso, estado y simulaciones. Buscador nombre/correo; estado; A–Z/Z–A, accesos recientes/antiguos, más/menos simulaciones; con/sin simulaciones; limpiar y contador. Tarjetas con márgenes uniformes.
Ficha con identidad, fecha de nacimiento, último acceso, estado y número de simulaciones; bloques de condiciones con acciones compactas. Suspender no elimina al cliente. Confirmación por folio para cambios. Cargar foto real del cliente, que hoy no se comparte con la ficha temporal.

### Administradores y ficha

Tarjetas con avatar, nombre completo, correo y distintivo de sesión. Alta compacta; no grandes botones de eliminar ocupando toda la tarjeta. Consultar requiere folio propio. Foto editable únicamente en la ficha de la cuenta en sesión; no añadir controles de cámara en fichas ajenas. Folio oculto salvo flujo autorizado de revelar propio; contraseña nunca visible en ficha. Alta muestra folio recién generado para copiar. Mantener confirmación de contraseña en alta y edición.

### Apelaciones y actividad

Apelaciones: métricas pendiente/aprobada/rechazada, buscador, filtro y tarjetas con mensaje/estado/motivo/respuesta. Aprobar primario, rechazar secundario. Motivo de rechazo en QTextEdit con ajuste de palabra y sin scroll horizontal. Mensajes largos no ensanchan ventana. Cuenta restringida ve estado y respuesta de sus solicitudes.
Actividad: tres métricas (acciones filtradas, administradores distintos participantes, apelaciones resueltas), búsqueda, filtros administrador/acción/desde/hasta, limpiar. Tabla fecha-hora/responsable/acción/elemento/detalle. Referencia actual ocho filas por página y anterior/siguiente. Todos los administradores ven el mismo registro, no solo sus actos. Estado vacío con métricas cero.

### Editor de foto compartido

Avatar circular por defecto; vista previa circular del recorte cuadrado. Seleccionar imagen, acercamiento 100–300 %, horizontal/vertical 0–100 con centro 50. Selección actual inicia zoom 120 % para facilitar movimiento. Deshabilitar desplazamiento solo si no existe margen real y explicar que se aumente zoom. Foto final actual 512×512; carga se reduce a 2048×2048, rechaza >12000 px por lado o decodificación inválida.
Admite PNG/JPG/JPEG/WEBP/BMP según QImageReader disponible. Restablecer icono solo modifica vista previa; guardar confirma; cancelar descarta todo. Administrador confirma con su folio; cliente solo cambia su propia imagen. Mantener foto coherente en sidebar, perfil y listados después de guardar.

## 7. Comprobaciones visuales obligatorias

Probar Windows a 100/125/150 % si están disponibles, tamaño mínimo y maximizado; formularios largos y diálogos deben tener scroll sin campos comprimidos. Revisar nombres compuestos, correos largos, mensajes multilinea, nombres de simulación de 50 caracteres, filtros con muchas opciones, tabla de 480 cuotas y PDF multipágina.
Verificar hover/foco/selección/deshabilitado y casilla marcada/desmarcada; teclado Tab, espacio y Enter; acciones cancelar; ningún campo tapado por etiqueta. No asumir que `sizeHint` sin estilo es igual al tamaño final.
El plugin offscreen usado en pruebas anteriores a veces renderizó cuadrados en lugar de letras por fuentes: sirve para medidas y lógica, NO valida legibilidad visual real. Validar con ventana real/captura antes de afirmar que está perfecto.

## Retirada completa del prototipo al integrar el backend

Acuerdo explícito del propietario, 25/11/2026: revisar **todos los textos y comportamientos del frontend**, incluidos los mensajes pequeños, para que la versión integrada no conserve referencias al prototipo. Esta revisión es obligatoria para cada función que se conecte y para el cierre de la integración.

- Quitar o sustituir «Demo», «Demostración», «Modo prototipo», «Datos de muestra», «Datos ilustrativos», «Ejemplo ilustrativo», «Cambios temporales», «No se guardan al cerrar», «Próximamente» y cualquier frase equivalente, aunque sea una nota, etiqueta, placeholder o tooltip.
- Revisar login, registro, barras laterales/superiores, tarjetas, filtros, tablas, perfiles, fichas administrativas, diálogos de folio y confirmación, errores, notificaciones, estados vacíos, ayuda/Acerca de, títulos de ventanas y PDF exportados. No limitar la limpieza a los encabezados visibles.
- Retirar accesos de demostración que eviten autenticar, credenciales sugeridas del prototipo, navegación directa sin validación, resultados fijos y listas locales independientes. Conectar las pantallas a la sesión y a los datos persistidos correctos.
- Sustituir botones deshabilitados provisionalmente por acciones reales cuando se implementen. Conservar estados deshabilitados legítimos por permisos, validaciones o ausencia de datos.
- Reescribir ayudas y mensajes según el comportamiento real. No afirmar que se guardó, autenticó, autorizó o exportó algo hasta comprobar el éxito de la operación.
- Conservar el fondo, paleta, logo y composición aprobados. Esta limpieza no autoriza un rediseño ni eliminar los datos iniciales SQL acordados; tampoco borrar ejemplos educativos útiles del glosario.
- Mantener el aviso de que las simulaciones son informativas y no constituyen una oferta, aprobación o contrato de préstamo. En PDF puede usarse «Simulación informativa» al sustituir «Ejemplo ilustrativo»: el alcance informativo sigue siendo verdadero en la versión final.

**Criterio de finalización:** revisar cadenas del código y recorrer las pantallas de ambos roles, incluidos diálogos, errores y PDF. No debe quedar ninguna referencia de prototipo presentada al usuario en los flujos ya integrados. Las menciones históricas de esta documentación sí se conservan para explicar el punto de partida. Buscar palabras es una ayuda; comprobar su contexto evita eliminar avisos o ejemplos educativos válidos.

La retirada se hace junto con la implementación real de cada función; ocultar los mensajes por sí solo no completa el backend. Esta actualización modifica únicamente documentación.


## Integración de acceso — 26/11/2026
Se conservan paleta, fondo, ilustración, tarjeta y controles aprobados. Login muestra
errores y estado de verificación; los formularios se bloquean mientras procesan
en segundo plano. Ambos apellidos son obligatorios. Contraseñas: 8–15 caracteres
con mayúscula, minúscula, dígito y símbolo; sin espacios ni controles.
Registro mantiene lectura hasta el final y aceptación manual, sin marcar la casilla
automáticamente. Al completar o abandonar un flujo se limpian campos y visibilidad
de contraseñas. Cerrar sesión destruye el panel anterior; no reutiliza sus datos.
El acceso restringido muestra información real del cliente autenticado y consultas
de apelaciones de su propiedad. El envío de apelaciones aún no está conectado.
Los módulos no integrados conservan avisos explícitos de datos temporales.

## Perfil propio persistente — 26/11/2026
Mi cuenta y la ficha del administrador en sesión guardan datos y foto propios.
Validaciones compartidas: nombre y ambos apellidos obligatorios, letras/espacios y
máximo 40 caracteres; correo válido y único entre todas las cuentas; mayoría de edad;
nueva contraseña de 8–15 caracteres, mayúscula, minúscula, número y símbolo, sin espacios.
Para cambiarla se verifica la contraseña actual y se rechaza reutilizarla. Ambos campos
nuevos vacíos conservan la contraseña. Se rechaza guardar cuando no hay cambios reales.
ServicioAcceso revalida el rol/estado, usa transacción y bloqueo compartido con registro.
Los editores esperan el resultado fuera del hilo gráfico antes de actualizar las vistas.

Foto: se conserva selección/recorte; se limita entrada a 10 MiB y 12000 píxeles por lado,
y se guarda PNG 512×512 en AppDataLocation/fotos. MySQL guarda foto_ruta, no los bytes.
Guardar escribe un archivo nuevo, confirma SQL y retira el anterior solo dentro de la
carpeta gestionada. Fallo SQL retira el nuevo; restablecer guarda NULL. La misma foto
se rechaza. Respaldar base y carpeta de fotos juntas. No se requiere modificar el SQL.
Los cambios propios del administrador registran actividad sin claves, hashes ni folios.
Edición administrativa de terceros, directorio de clientes y sus vistas siguen pendientes.

## Simulador conectado — 26/11/2026
Formulario inicialmente vacío y métricas sin resultados ficticios. Nombre limitado a
50 caracteres; monto/tasa/plazo se capturan como texto para evitar que un spinbox
corrija entradas inválidas al perder foco. Punto decimal, sin separadores de miles,
como indica la ayuda. Se conservan los accesos rápidos de plazo y el estilo de campos.
Calcular muestra las tres métricas y todas las filas; cambiar cualquier campo limpia
resultados y deshabilita Guardar hasta recalcular. Confirmación junto al formulario;
no se vacían entradas tras error. Durante la escritura se bloquean controles y navegación.
Tras éxito, otro clic o recalcular las mismas condiciones no repite el último guardado.
La tabla tiene scroll vertical y, en ventanas estrechas, horizontal para leer los importes.
Se inspeccionaron capturas nativas a 1180×820 y 920×700, incluida tasa 200 % a 480 meses.
