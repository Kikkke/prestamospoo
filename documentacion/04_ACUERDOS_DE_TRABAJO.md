# Acuerdos de trabajo y memoria del proyecto

Actualizado: 27/11/2026. Este documento transmite decisiones y preferencias a una persona o chat que no puede ver la conversación. El punto de entrada es [00_SISTEMA_Y_REQUERIMIENTOS.md](00_SISTEMA_Y_REQUERIMIENTOS.md). No es un permiso para realizar todos los pendientes sin que el usuario pida la siguiente etapa.

## 1. Cómo colaborar con el propietario

El proyecto es académico de POO y el usuario quiere poder explicar el código ante su profesor. Prefiere claridad, C++/Qt y SQL sencillo, avances entendibles y un acabado profesional. No añadir frameworks, servidores ni complejidad que no resuelvan una necesidad del alcance local.

- Hablar en español, con explicaciones sencillas y ejemplos cuando aclaren conceptos. Distinguir frontend, lógica de interfaz, backend, MySQL Server y Workbench.
- Antes de codificar una pantalla nueva, pensar en su uso: jerarquía, composición, estados, permisos, tamaño mínimo, filtros, vacíos, errores y confirmaciones. «Profesional» significa coherencia y facilidad de uso, no adornar cada elemento.
- Ante una petición de opinión/propuesta o «todavía no hagas nada», responder primero sin modificar. Si autoriza «hazlo», ejecutar el alcance acordado y comprobarlo; no volver a pedir permiso para lo mismo.
- Preguntar antes de elegir una regla funcional importante que no esté definida o cuando dos instrucciones actuales se contradigan. No pedir contexto que ya está en estos documentos/código. Avanzar en lo independiente de la respuesta.
- Mostrar concretamente qué se propone cuando pida opciones; no generar prototipos completamente distintos sin mantener identidad.
- Preservar trabajo del usuario, no sobrescribir cambios ajenos ni reiniciar la BD para facilitar pruebas. No cerrar su aplicación por fuerza para recompilar sin explicarlo.
- Informar brevemente avances durante trabajos largos y al terminar decir qué cambió, qué se comprobó y qué sigue pendiente. No afirmar «100 % terminado» solo porque se ve bien o compila.
- Ahorro de uso: comenzar por 00 y la sección del área afectada; búsquedas acotadas, lecturas por bloques y verificaciones pertinentes. No releer repetidamente toda la historia o el PDF. Usar búsquedas locales antes de preguntar.
- No crear otros chats ni delegar en agentes sin petición o instrucción aplicable. No enviar correos ni mensajes a las personas de los datos iniciales.

## 2. Organización documental final: exactamente cinco archivos

| Archivo | Responsabilidad |
|---|---|
| 00_SISTEMA_Y_REQUERIMIENTOS.md | Índice, alcance, estado, decisiones, recorrido y texto íntegro extraído del PDF con sus IDs |
| 01_FRONTEND.md | Identidad, paleta, fondo, componentes, pantallas y regresiones visuales |
| 02_BACKEND_CPP.md | Validaciones, sesiones, permisos, cálculos, seguridad, casos límite y plan de pruebas |
| 03_BASE_DE_DATOS.md | SQL vigente, relaciones, conexión, datos iniciales y operaciones atómicas |
| 04_ACUERDOS_DE_TRABAJO.md | Esta memoria de colaboración, decisiones, dudas y mantenimiento |

Todos viven en `documentacion/`. El propietario pasó de proponer un solo documento a pedir cinco por responsabilidad; **la última decisión son estos cinco**. No crear un README por ventana, duplicar instrucciones en cada carpeta ni añadir un sexto Markdown sin solicitud nueva.
Actualizar cada regla en su documento principal y enlazarla desde los otros. No mantener dos tablas de valores definitivos que discrepen. Las decisiones posteriores del usuario prevalecen; fecha y motivo de cambios sustanciales deben quedar aquí o en 00.

Documentación histórica reemplazada y eliminada tras consolidarla:

| Markdown anterior | Contenido preservado / destino |
|---|---|
| aplicacion-cpp/README.md | Estado del prototipo, ejecución y ventanas en 00/01 |
| aplicacion-cpp/DISENO.md | Identidad, espaciado, estilos y referencia visual en 01 |
| aplicacion-cpp/PENDIENTES_AMORTIZACION.md | Detalle/PDF/compartir historial en 01/02; separar hecho y pendiente |
| aplicacion-cpp/ACUERDOS_BASE_DE_DATOS.md | Acuerdos de migración, folios y credenciales en 02/03 |
| aplicacion-cpp/DATOS_MUESTRA_MIGRACION.md | Inventario de ejemplos, resultados, fotos y límites en 03 |
| aplicacion-cpp/assets/README.md | Rutas, usos y recursos en 01 |
| aplicacion-cpp/assets/GENERACION.md | Criterios para ilustraciones, transparencia y paleta en 01 |
| aplicacion-cpp/assets/GENERACION-ACCESO.md | Persona/documento/círculo, rechazo de contorno blanco en 01 |
| aplicacion-cpp/bd/CONTEXTO_Y_DECISIONES.md | Ocho tablas y simplificaciones vigentes en 03 |
| base-datos/CONTEXTO_Y_DECISIONES.md | Decisiones históricas sustituidas; advertencia sobre SQL antiguo en 00/03 |
| Cascaron/README.md | Boceto HTML histórico separado de aplicación Qt en 00 |

Eliminar esos textos no significa borrar assets, el cascarón, código, SQL, PDF ni builds. El PDF original de requisitos y otros documentos históricos permanecen donde están. La consolidación conserva decisiones vigentes y contexto útil; no recicla reglas descartadas como si siguieran autorizadas.

## 3. Decisiones que no se deben volver a discutir por falta de memoria

### Producto y aspecto

- Nombre FinancionantiMex; «Una forma impresionanti de obtener un préstamo». No corregir la palabra distintiva impresionanti. Corregir solo espacios/ortografía accidental del resto.
- Fondo negro con cinta/halo cobre del login en todos los módulos; letras claras y acentos cobre. La paleta es prioritaria sobre imágenes externas y el antiguo DESIGN.md.
- Logo de moneda/bolsa con cuernos, común a acceso, laterales, ventanas y PDF. Avatar de persona es otra cosa: imagen por defecto cuando no hay foto personal.
- Login/registro conservan el detalle `:)` donde encaje. No restaurar leyendas retiradas del registro para hacer espacio a términos.
- Diseño del frontend aprobado. No rediseñar por iniciativa al conectar datos; corregir únicamente lo necesario para comportamiento, accesibilidad y adaptabilidad.
- Referencia dashboard inspiró composición; referencia Samsung, tarjetas de ayuda totalmente clicables. No copiar sus marcas ni colores blancos.
- Fotos elegidas por cliente/admin deben verse en sidebar. Saludo usa todos los nombres y omite apellidos; menú de salida dice Cerrar sesión.

### Sistema y permisos

- Simulador educativo local, MXN, cuota fija mensual francesa. No es prestamista ni portal de pagos; no modelar préstamos aprobados, cobros recibidos o transferencias reales.
- 0 % = sin intereses dentro del mismo método. 1 % no significa interés simple; no volver a introducir PrestamoInteresSimple solo para demostrar herencia. Usuario → Cliente/Administrador es una herencia natural para la POO.
- Cinco administradores iniciales, pero sin máximo de cuentas; todos tienen los mismos permisos. No rol superior o superadministrador.
- Registro público solo de clientes. Login usa correo y contraseña; nombre y apellidos separados pertenecen al alta/perfil, no a nuevas credenciales de login.
- Folio complementa credenciales y confirmaciones. Prefijo fijo tres letras + ddMMyy + dos letras + dos números; la comparación de unicidad usa todo el folio. No rotación de letras ni regeneración por cambiar datos.
- Conservar literalmente los cinco folios iniciales, incluso JAR para Christian Jared. No corregir nombres a costa de invalidar folios conocidos.
- Cada modificación administrativa se confirma con el folio de quien tiene la sesión activa. Para consultar fichas de administradores también. Para revelar su folio propio necesita el de un tercero distinto y vigente.
- Nadie puede revelar el folio ajeno. Foto administrativa solo propia. Los demás datos del administrador pueden editarse conforme a permisos iguales; folio nunca editable. Contraseña editable con doble campo y ambos vacíos para conservar.
- No eliminar al administrador en sesión ni al último disponible. Los clientes solo se deshabilitan/reactivan, no se eliminan sus cuentas.
- Activo/inactivo depende de últimos seis meses calendario; deshabilitado es otra condición y bloquea acceso. No autoeliminar ni autosuspender por antigüedad.
- Apelación es la vía interna de revisión: no hay chat, correo de soporte ni mensajería. Aprobación restaura acceso; no inventa login reciente.
- Todos los administradores ven toda Actividad, como consulta; no borran/editan eventos desde la aplicación.
- **Eliminación lógica de simulaciones** es la última decisión expresa, después de considerar borrado físico. Conservar cuotas/relaciones, ocultar de consultas normales. No agregar papelera/restauración sin nueva solicitud.
- Historial es fuente de simulaciones propias del comparador; detalle y PDF usan la misma amortización. No catálogos independientes por ventana en backend final.
- Guardar condiciones + todas las cuotas; al editar, recalcular juntas. No tabla de versión de método ni historial completo de cuotas: actividad guarda resumen del cambio administrativo.

### Datos, validaciones y contraseñas

- Usuario pidió validaciones en C++ y SQL con comandos básicos; no restaurar CHECK/UNIQUE/ENGINE/COLLATE ni las tablas de normalización retiradas. Documentar límites de integridad por acceso directo a SQL sin ocultarlos.
- Base vigente está en aplicacion-cpp/bd/Financionanti.sql, no en base-datos/01_esquema_y_muestras.sql.
- Contraseñas: acuerdo consolidado PBKDF2-HMAC-SHA-256 mediante Qt, explicado en 02. No SHA-256 rápido simple ni cifrado reversible. Los parámetros de implementación recomendados no son texto literal del PDF.
- Proyecto local con MySQL y Workbench; root es el usuario de conexión pedido. La contraseña local se configura privadamente, no en estos archivos ni en CMake.
- Actualización 26/11/2026: el propietario proporcionó las cinco claves administrativas y aceptó las siete propuestas para clientes. Se incluyen solo sus hashes en el SQL; nunca usar el folio como contraseña.
- Conservar datos iniciales/asignaciones SQL y aclarar lo que era muestra de interfaz. Al conectar, eliminar accesos y leyendas demo; no presentar como real actividad administrativa inventada.
- Términos: lectura hasta abajo habilita posibilidad de aceptar, clic prematuro avisa, nunca aceptación automática. Guardar última versión/fecha. Excepción acordada: las 12 cuentas iniciales se consideran ya aceptadas por declaración del propietario.

## 4. Lecciones de errores visuales anteriores

1. QSS global de desplegables puede modificar marcos de otros widgets. La mejora de filtros terminó afectando login; revisar alcance de selectores y scroll antes de cambiar dimensiones a ciegas.
2. Campos de apellidos en una fila hicieron que correo se encimara y los campos se aplastaran. Solución vigente: filas nativas separadas y scroll propio del registro; no alturas fijas insuficientes en contenedores.
3. Se perdió visualmente la casilla de términos. Ahora necesita indicador definido, marca visible y control de interacción; no basta el texto «He leído…».
4. El menú lateral era toggle independiente y quedaban varias opciones encendidas. El estado activo deriva de la pantalla actual y no se apaga al volver a pulsarla.
5. Los filtros no mostraban todas las opciones. El popup necesita máximo razonable y scroll vertical, navegación de teclado y flechas claras.
6. Etiquetas con fondo negro detrás de filtros rompían la continuidad. Mantenerlas transparentes y controles de igual altura; reacomodar columnas al reducir ancho.
7. Motivo largo de rechazo se desplazaba horizontalmente. Usar QTextEdit y ajuste de palabra, sin barra horizontal; revisar también su visualización después de guardar.
8. Sliders de foto parecían rotos. El recorte necesita escala real y rango de desplazamiento relacionado con el margen disponible; deshabilitar/explicar cuando no hay margen, no mover un valor sin cambiar imagen.
9. Dos iconos de historial parecían estar en columnas distintas. Reservar ancho suficiente de acciones y usar un único contenedor por fila.
10. PDF no se limita a una o dos páginas. Repetir encabezados, paginar, mostrar totales correctos; no reducir meses para ocultar un problema de renderizado.
11. Cambiar una foto/perfil debe refrescar todas sus vistas. Un pixmap nuevo en un diálogo no asegura que sidebar o lista representen al mismo usuario.
12. Compilar o medir widgets offscreen no demuestra que se vean bien. A veces offscreen produjo cuadrados por fuentes: validar ventana real y escalado Windows.

## 5. Qué se conoce y qué no se debe afirmar

Se revisaron archivos fuente, SQL, Markdown anteriores y PDF suministrado para esta consolidación. El frontend existe y ha pasado revisiones visuales previas con el usuario. La amortización de muestras y exportación multipágina tienen implementación real local. Aun así, sus listas, autorizaciones y perfiles siguen siendo temporales en muchos módulos.
Actualización 26/11/2026: conexión MySQL, autenticación, registro y aceptación persistida ya cuentan con pruebas reales en una base aislada. La auditoría y los módulos de gestión siguen pendientes. El plan completo de pruebas de 02 no equivale a que todas esas funciones estén implementadas.
No afirmar acceso a conversaciones externas ni al último diagrama completo que no se haya abierto. El diagrama puede necesitar actualización al implementar roles/servicios: el modelo de 02 es una propuesta de organización coherente, no una certificación del PDF .drawio histórico.
El original de requisitos está en Descargas; su texto completo queda en 00. Texto extraído conserva requisitos, pero no toda la maquetación del PDF. Reglas corregidas se indican antes del anexo para no implementar contradicciones accidentalmente.

## 6. Dudas pendientes reales para la futura implementación

No bloquean la entrega de documentación. Preguntar solo cuando afecten la tarea que se vaya a implementar:

| Tema | Qué falta acordar / propuesta claramente identificada |
|---|---|
| Contraseñas iniciales | Resuelto: hashes PBKDF2 directamente en los INSERT; administradores según claves del propietario. |
| Apellidos | Resuelto: paterno y materno obligatorios, tanto en alta como en edición. |
| Nombres cortos de admin | El folio exige tres letras; propuesta rechazar alta si la normalización no reúne tres, sin impedir ese nombre para un cliente. |
| Términos en cuentas semilla | Resuelto: el propietario declaró que ya los aceptaron. INSERT con versión 1 y fecha inicial; registro público conserva aceptación manual. |
| Foto máxima | Límite de bytes no fijado por usuario/PDF. Propuesta 10 MiB y límites de dimensiones/decodificación; conservar 512×512 final. |
| Textos de apelación/rechazo | PDF exige contenido y motivo de rechazo, pero no longitud máxima. Propuesta 2000 caracteres con contador. |
| Usuario sin último acceso | Propuesta Inactivo con «Sin acceso», ordenar fechas nulas al final. No fecha inventada. |
| Acceso restringido por suspensión | Definir si se registra como acceso en otro lugar; propuesta no actualizar ultimo_acceso operativo hasta entrar plenamente. |
| Casos extremos de amortización | Resuelto el 26/11: permitir pagos cero por redondeo, ajustar la última cuota para liquidar, nunca generar capital negativo e indicar ajustes visibles. |
| Datos temporales/históricos | Cuentas SQL contienen fechas de acceso anteriores a la fecha común de alta de semilla. Corregir si se exige cronología estricta; no fabricar actividad de respaldo. |
| Auditoría de consultas | Registrar cambios siempre. Consultas protegidas de fichas pueden dejar evento sin datos sensibles; definir granularidad para evitar ruido. |

No volver a preguntar si el entorno será local, si todos los administradores ven actividad, si el folio se puede editar, si la foto administrativa puede ser ajena o si borrar simulaciones es físico: ya está resuelto.

## 7. Rutina de entrega para el siguiente programador

1. Leer índice/estado de 00 y sección relevante. Comprobar archivos actuales, ya que pueden cambiar después de 25/11/2026.
2. Delimitar lo pedido: frontend, documentación, SQL o backend. Si solo se pide opinión, no editar.
3. Revisar componentes compartidos antes de crear otro widget o replicar lógica. La sesión por ID, validaciones y motor serán comunes.
4. Para cada acción anotar entradas, permisos, validación, cambio de datos, auditoría, errores y refresco visual. Mantener transacciones de operaciones relacionadas.
5. Implementar por fases pequeñas comprobables; no reemplazar todo el proyecto ni desarrollar un sitio web paralelo.
6. Probar casos normales, límites, cancelación, permisos y fallo real pertinentes. Para persistencia, cerrar/reabrir y revisar BD; para diseño, ventana real a varios tamaños.
7. Actualizar estado y requisitos en estos cinco documentos; quitar «pendiente» solo con evidencia. Escribir cambios en el lugar correcto, no nuevas notas dispersas.
8. Entregar resumen breve, rutas modificadas, comprobaciones y límites reales. Si falta algo externo, decir exactamente qué falta y qué se pudo terminar.

La entrega documental del 25/11/2026 sustituyó once Markdown antiguos por cinco documentos consolidados. En esa entrega no se implementó el backend ni se aplicó el SQL. El alcance de la implementación posterior se detalla al final de este documento.

## Acuerdo adicional: limpieza completa del prototipo

La integración incluye retirar todas las referencias visibles y comportamientos provisionales, incluso mensajes pequeños, tooltips, diálogos y PDF. Aplicar el alcance y los criterios de la sección «Retirada completa del prototipo al integrar el backend» de [01_FRONTEND.md](01_FRONTEND.md). Conservar los avisos informativos propios del simulador y el diseño aprobado. No declarar completada una función únicamente por quitar sus leyendas temporales.


## Alcance autorizado y entregado — 26/11/2026
El usuario autorizó implementar login, registro y cierre de sesión y actualizar el
SQL existente listo para ejecutar completo. No pidió borrar automáticamente su base.
Se conservaron folios, diseño, ocho tablas y datos de préstamos. Se requiere que el
usuario reinstale su base para utilizar los hashes precargados nuevos.
La sesión de clientes deshabilitados ya consulta sus propias apelaciones; el envío
y resolución persistente se dejan para administración, según la delimitación previa.
No extender esta entrega a cálculo, simulaciones o persistencia de perfiles sin
la siguiente indicación. Las claves administrativas no se reproducen en documentación.

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

## Aprobación del simulador — 26/11/2026
El propietario aprobó validación, cálculo, invalidación al editar, guardado transaccional
y pruebas, incluida la política de redondeo extremo. Etapa terminada: calcular/mostrar/
guardar. Próxima etapa: conectar historial, PDF y comparador; no presentar sus muestras
como registros persistidos. Se actualizaron también los límites del editor administrativo
provisional, sin convertir su edición en persistente ni ampliar el alcance a gestión.

## Trabajo por módulos y aprobación de historial — 27/11/2026
El propietario pidió presentar plan y nivel de pensamiento recomendado antes de cada
módulo. Niveles que utiliza: mínimo, medio, alto, muy alto, máximo y ultra. Para historial
se recomendó Astra en muy alto y el propietario dio luz verde para codificar.
Alcance entregado: listado real, búsqueda, orden, actualización, eliminación lógica,
permisos y pruebas. No se adelantó amortización/PDF: su botón queda deshabilitado y
explicado. Próximo módulo propuesto: consulta de cuotas persistidas y exportación PDF;
presentar su plan y recomendación de nivel, y esperar aprobación antes de implementarlo.
