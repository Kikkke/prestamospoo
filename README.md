# Calculador de préstamos — prototipo visual en Qt

Aplicación C++ / Qt Widgets con fondo negro con destellos cobre, texto blanco, tarjetas redondeadas y navegación lateral. Los estilos están centralizados en Tema.h.

## Ejecutar

1. Abre CMakeLists.txt en Qt Creator.
2. Selecciona Desktop Qt 6.12.0 MinGW 64-bit con compilador MinGW 15.1.
3. Configura y pulsa Ctrl + R. Cierra la aplicación anterior antes de compilar.
4. Pulsa «Iniciar sesión» o «Crear cuenta» en el formulario de registro. Ambos abren Inicio / Simulador sin requerir credenciales.

## Pantallas

- Inicio de sesión y creación de cuenta.
- Simulador como pantalla principal.
- Resultados de ejemplo y tabla de amortización.
- Historial con 20 ejemplos, búsqueda, ordenación y eliminación temporal con confirmación.
- Comparador de dos ejemplos, con selectores de búsqueda, resúmenes y tabla actualizada.
- Acerca de e instrucciones breves.

La navegación funciona. Los campos son editables, pero los resultados son ejemplos fijos: no cambian con lo capturado. Los botones de inicio de sesión y registro solo permiten navegar al inicio, sin validar ni crear cuentas. Los botones de cálculo y guardado están deshabilitados. El comparador muestra resultados precargados al elegir entre seis ejemplos; no calcula nuevos préstamos. No hay conexión a MySQL ni almacenamiento. El historial permite borrar ejemplos solo durante la ejecución; al reiniciar se restauran los 20 registros, sin afectar al simulador ni al comparador.

## Archivos

- main.cpp: inicio y aplicación del tema.
- Tema.h: estilos compartidos.
- VentanaAcceso.h/.cpp: composición del acceso y navegación al panel.
- PantallaLogin.h/.cpp y PantallaRegistro.h/.cpp: formularios de cuenta.
- PanelPrincipal.h/.cpp: menú lateral, pantallas y datos de muestra.
- HistorialSimulaciones.h: búsqueda por nombre sin distinguir mayúsculas ni acentos, ordenación numérica, contador y eliminación por fila con confirmación.
- CMakeLists.txt: compilación con Qt Widgets y AUTOMOC.

Requiere C++17, Qt Widgets, CMake y un compilador compatible. Admite Qt 5 o Qt 6. El HTML anterior permanece separado en ../Cascaron.

## Distribución del tablero

Inicio reúne condiciones del préstamo, tres tarjetas de resultados y tabla de amortización. El menú lateral contiene Inicio, Historial, Comparador y Acerca de; la barra superior indica la sección actual. El interior comparte el fondo decorativo del login, con ilustraciones pequeñas y acentos cobre. Las tarjetas tienen bordes finos y relieve discreto inspirado en DESIGN.md; los criterios están en DISENO.md. Acceso y registro conservan la distribución de dos columnas y comparten el estilo de tarjetas. Todos los resultados siguen siendo ejemplos fijos.

## Guía y glosario

Acerca de incluye ocho tarjetas de temas: alcance, recorrido de pantallas, tasa de interés, ejemplo de amortización, efecto de monto/tasa/plazo, lectura de tablas, glosario de 32 conceptos y fuentes oficiales. El contenido está en ContenidoAyuda.h. PantallaAyuda.h presenta tarjetas completas pulsables, iconos vectoriales y vistas de lectura con Volver a los temas. La cuadrícula cambia entre tres y dos columnas según el ancho disponible. Los ejemplos educativos no activan cálculos reales ni cambian los datos fijos del tablero.

## Interacciones del inicio
El formulario se organiza en tres pasos con iconos y ayudas. Los accesos de 3, 6, 12 y 24 meses actualizan solo el campo de plazo; el aviso cambia cuando las condiciones difieren del ejemplo inicial. Las métricas y la amortización permanecen fijas. Los encabezados de la tabla tienen explicaciones al pasar el cursor y ¿Cómo se interpreta? abre directamente su tema de Acerca de.

## Panel administrativo de demostración
El directorio de administradores presenta tarjetas con avatar, búsqueda por nombre o correo y acceso a una ficha mediante el folio del administrador de la sesión activa. El alta solicita nombre, correo, contraseña y fecha de nacimiento; valida mayoría de edad y correo único, y entrega un folio automático para copiar. La contraseña no se conserva en este prototipo ni se muestra en las fichas. Apelaciones incluye contadores de pendientes, aprobadas y rechazadas, búsqueda y filtro por estado.

En el login, Demo · Administrador abre la confirmación de folio. Usa ENR310307GB34 para entrar y confirmar cambios. Clientes permite buscar, filtrar, abrir fichas, deshabilitar cuentas y editar o eliminar sus simulaciones. Administradores permite crear cuentas de muestra y retirar otras; la sesión actual está protegida. Apelaciones permite aprobar y reactivar o rechazar con un motivo. Demo · Cuenta deshabilitada representa a Ana López y comparte el estado de su apelación con el panel. Todos los cambios son temporales. Las nuevas cuentas no autentican accesos reales; las simulaciones editadas aún no se recalculan. La verificación del folio es solo una interacción local de prototipo, no seguridad de producción.
