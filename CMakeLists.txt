cmake_minimum_required(VERSION 3.16)
project(CalculadorPrestamos VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Network Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Network Concurrent)

add_executable(CalculadorPrestamos WIN32
    controles.qrc
    Acceso.h
    Acceso.cpp
    Perfil.cpp
    Simulador.h
    Simulador.cpp
    TareaAcceso.h
    ConexionBD.h
    ConexionBD.cpp
    main.cpp
    VentanaAcceso.h
    VentanaAcceso.cpp
    PantallaLogin.h
    PantallaLogin.cpp
    PantallaRegistro.h
    PantallaRegistro.cpp
    PanelPrincipal.h
    PanelAdministrador.h
    PanelPrincipal.cpp
    Tema.h
    MarcaFinancionanti.h
    CampoNacimiento.h
    CampoContrasena.h
    FondoAcceso.h
    ContenidoAyuda.h
    PantallaAyuda.h
    HistorialSimulaciones.h
    HistorialSimulaciones.cpp
    ServicioHistorial.h
    ServicioHistorial.cpp
    Amortizacion.h
    Amortizacion.cpp
    VentanaAmortizacion.h
    VentanaAmortizacion.cpp
    ComparadorSimulaciones.h
)
target_compile_definitions(CalculadorPrestamos PRIVATE ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets")
target_link_libraries(CalculadorPrestamos PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Concurrent)
if(MSVC)
    target_compile_options(CalculadorPrestamos PRIVATE /utf-8)
endif()


target_compile_definitions(CalculadorPrestamos PRIVATE CONFIG_LOCAL="${CMAKE_CURRENT_SOURCE_DIR}/conexion.local.ini")
add_custom_command(TARGET CalculadorPrestamos POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/runtime" "$<TARGET_FILE_DIR:CalculadorPrestamos>")

option(PROBAR_ACCESO "Compilar pruebas de acceso sobre una base aislada" OFF)
if(PROBAR_ACCESO)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)
    get_target_property(FUENTES_APP CalculadorPrestamos SOURCES)
    list(REMOVE_ITEM FUENTES_APP main.cpp)
    add_executable(PruebasAcceso pruebas/PruebasAcceso.cpp ${FUENTES_APP})
    target_include_directories(PruebasAcceso PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_definitions(PruebasAcceso PRIVATE
        ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
        CONFIG_LOCAL="${CMAKE_CURRENT_SOURCE_DIR}/conexion.local.ini")
    target_link_libraries(PruebasAcceso PRIVATE Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Concurrent)
    add_custom_command(TARGET PruebasAcceso POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/runtime" "$<TARGET_FILE_DIR:PruebasAcceso>")
    add_executable(PruebasSimulador pruebas/PruebasSimulador.cpp ${FUENTES_APP})
    target_include_directories(PruebasSimulador PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_definitions(PruebasSimulador PRIVATE
        ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
        CONFIG_LOCAL="${CMAKE_CURRENT_SOURCE_DIR}/conexion.local.ini")
    target_link_libraries(PruebasSimulador PRIVATE Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Concurrent)
    add_custom_command(TARGET PruebasSimulador POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/runtime" "$<TARGET_FILE_DIR:PruebasSimulador>")
    add_executable(PruebasHistorial pruebas/PruebasHistorial.cpp ${FUENTES_APP})
    target_include_directories(PruebasHistorial PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_definitions(PruebasHistorial PRIVATE
        ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
        CONFIG_LOCAL="${CMAKE_CURRENT_SOURCE_DIR}/conexion.local.ini")
    target_link_libraries(PruebasHistorial PRIVATE Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Concurrent)
    add_custom_command(TARGET PruebasHistorial POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/runtime" "$<TARGET_FILE_DIR:PruebasHistorial>")
    add_executable(PruebasAmortizacion pruebas/PruebasAmortizacion.cpp ${FUENTES_APP})
    target_include_directories(PruebasAmortizacion PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_definitions(PruebasAmortizacion PRIVATE
        ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
        CONFIG_LOCAL="${CMAKE_CURRENT_SOURCE_DIR}/conexion.local.ini")
    target_link_libraries(PruebasAmortizacion PRIVATE Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Concurrent)
    add_custom_command(TARGET PruebasAmortizacion POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/runtime" "$<TARGET_FILE_DIR:PruebasAmortizacion>")
endif()
