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
- Historial y vista previa de confirmación de eliminación.
- Comparador de dos plazos.
- Acerca de e instrucciones breves.

La navegación funciona. Los campos son editables, pero los resultados son ejemplos fijos: no cambian con lo capturado. Los botones de inicio de sesión y registro solo permiten navegar al inicio, sin validar ni crear cuentas. Los botones de cálculo, guardado, comparación y eliminación están deshabilitados. No hay conexión a MySQL ni almacenamiento. La vista previa de eliminación no borra registros.

## Archivos

- main.cpp: inicio y aplicación del tema.
- Tema.h: estilos compartidos.
- VentanaAcceso.h/.cpp: composición del acceso y navegación al panel.
- PantallaLogin.h/.cpp y PantallaRegistro.h/.cpp: formularios de cuenta.
- PanelPrincipal.h/.cpp: menú lateral, pantallas y datos de muestra.
- CMakeLists.txt: compilación con Qt Widgets y AUTOMOC.

Requiere C++17, Qt Widgets, CMake y un compilador compatible. Admite Qt 5 o Qt 6. El HTML anterior permanece separado en ../Cascaron.
