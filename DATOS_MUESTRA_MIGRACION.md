# Inventario de muestras para migración

Revisado el 14/11/2026. Conservar estos ejemplos al preparar la futura base MySQL en Workbench. Este documento no crea tablas ni inicia la migración.

## Clientes del panel administrativo

Las fechas de último acceso son relativas al día en que se abre la aplicación, no fechas históricas fijas. Al migrar se deberá fijar una fecha de referencia documentada.

| Cliente | Correo | Nacimiento | Último acceso relativo | Cuenta |
|---|---|---|---|---|
| Ana López | cliente1@demo.mx | 10/03/1995 | Hoy | Deshabilitada por inactividad |
| Luis Pérez | cliente2@demo.mx | 11/04/1996 | Hace 2 meses | Habilitada |
| María Torres | cliente3@demo.mx | 12/05/1997 | Hace 4 meses | Habilitada |
| Carlos Ruiz | cliente4@demo.mx | 13/06/1998 | Hace 6 meses | Habilitada |
| Sofía García | cliente5@demo.mx | 14/07/1999 | Hace 8 meses | Habilitada, inactiva |
| Diego Hernández | cliente6@demo.mx | 15/08/2000 | Hace 10 meses | Habilitada, inactiva |

Actividad y habilitación son conceptos distintos. El prototipo clasifica como activo a quien accedió en los últimos seis meses, incluyendo el límite.

Cada uno de estos seis clientes tiene dos simulaciones propias (12 registros en total, no dos compartidos):
- Computadora: 12,000 MXN, tasa anual 0 %, 3 meses.
- Estudios: 24,000 MXN, tasa anual 12 %, 12 meses.

El panel permite modificar temporalmente estas condiciones; no guarda tablas de amortización para estos registros todavía. En la migración se deberá asociar una tabla coherente a cada simulación individual.

## Perfil del cliente de demostración

Mi cuenta usa actualmente un perfil independiente: Cliente de muestra, cliente@demo.mx, nacimiento 01/01/2000. No está conectado automáticamente con Ana ni con otro cliente del directorio. No inventar esa asociación durante la migración.

## Historial: 20 simulaciones independientes

El historial todavía no tiene un propietario real asignado. Conservar sus condiciones; decidir explícitamente su cuenta de demostración al integrar usuarios. No duplicar automáticamente este catálogo para todos los clientes.

| Simulación | Monto MXN | Tasa anual % | Meses |
|---|---:|---:|---:|
| Computadora | 12000 | 0 | 3 |
| Computadora a 6 meses | 12000 | 0 | 6 |
| Automóvil | 180000 | 14.5 | 48 |
| Estudios universitarios | 45000 | 10 | 24 |
| Remodelación de cocina | 65000 | 16 | 36 |
| Motocicleta | 38000 | 18 | 24 |
| Viaje familiar | 25000 | 12 | 12 |
| Equipo de fotografía | 32000 | 15 | 18 |
| Muebles para el hogar | 28000 | 9.5 | 12 |
| Paneles solares | 85000 | 11 | 36 |
| Herramientas de trabajo | 18500 | 13 | 18 |
| Curso de idiomas | 15000 | 8 | 12 |
| Consultorio dental | 120000 | 17 | 48 |
| Negocio de repostería | 55000 | 14 | 24 |
| Reparación de automóvil | 22000 | 19 | 12 |
| Equipo de sonido | 16500 | 12.5 | 18 |
| Bicicleta eléctrica | 27000 | 10.5 | 24 |
| Gastos médicos | 40000 | 9 | 24 |
| Teléfono móvil | 14000 | 0 | 12 |
| Ampliación de vivienda | 150000 | 15.5 | 60 |

## Tablas de amortización y exportación

Preservar el desglose completo y su vínculo con la simulación: número de pago, saldo inicial, pago, interés, abono a capital y saldo final. Conservar también condiciones, resultados, moneda, frecuencia y fecha real de guardado cuando exista.

El historial genera sus tablas al abrir la ventana; no existen filas persistidas todavía. Reproducir el criterio de VentanaAmortizacion.h al preparar muestras:
- Cuota fija, tasa mensual = tasa nominal anual / 1200.
- Con tasa cero, cuota = monto / meses.
- Cuota e intereses redondeados a centavos; capital = cuota menos intereses.
- Último pago ajustado para liquidar el saldo; comprobar saldo final cero.
- Totales de pagos, intereses y capital; nunca sumar saldos.
- Sin comisiones, seguros ni impuestos.
- No inventar fechas de guardado: el PDF actual muestra fecha de emisión.

Referencia comprobada: Ampliación de vivienda, 150,000 MXN, 15.5 % anual, 60 meses. Cuota regular 3,607.98; última cuota 3,607.86; intereses totales 66,478.68; total 216,478.68. PDF con pagos 1–22, 23–44 y 45–60.

## Comparador: catálogo precargado independiente

| Ejemplo | Monto | Tasa anual % | Meses | Cuota precargada | Intereses precargados | Total precargado |
|---|---:|---:|---:|---:|---:|---:|
| Computadora | 12000 | 0 | 3 | 4000 | 0 | 12000 |
| Computadora a 6 meses | 12000 | 0 | 6 | 2000 | 0 | 12000 |
| Computadora a 12 meses | 12000 | 12 | 12 | 1066.19 | 794.25 | 12794.25 |
| Muebles a 12 meses | 24000 | 0 | 12 | 2000 | 0 | 24000 |
| Muebles a 24 meses | 24000 | 0 | 24 | 1000 | 0 | 24000 |
| Curso de idiomas | 15000 | 0 | 10 | 1500 | 0 | 15000 |

Son resultados precargados, no una segunda fuente definitiva de cálculos. Al integrar el comparador debe leer las simulaciones del historial del usuario. Revisar diferencias de redondeo; Curso de idiomas tiene condiciones distintas en ambos catálogos, por lo que el nombre no sirve como identificador único.

## Otras muestras

- Inicio / Simulador: Computadora, 12,000 MXN, 0 %, 3 meses, tres cuotas de 4,000; resultados fijos de demostración.
- Apelación de Ana López: pendiente, fecha del día de ejecución, motivo de deshabilitación Inactividad; texto: «Deseo retomar mis simulaciones y solicito reactivar mi cuenta.» Sin respuesta inicial.
- Actividad administrativa: 24 entradas ilustrativas generadas en ActividadAdministrativa.h. No representan acciones reales ni una auditoría histórica. Conservar como catálogo de prueba separado, nunca presentarlas como actividad real al migrar.
- Fotos: icono predeterminado al iniciar. Las imágenes elegidas durante la ejecución solo viven en memoria; no están recuperables tras cerrar. La persistencia futura debe conservar imagen/ubicación y propietario.
- Contraseñas: no hay credenciales reales iniciales de clientes. No inventar contraseñas ni migrar hashes temporales como autenticación definitiva.

## Integridad pendiente para la migración

Asignar identificadores estables a cuentas, simulaciones, cuotas, apelaciones y eventos. No vincular datos por nombre, posición en la lista o correo mutable. Mantener la separación por propietario y las reglas de permisos acordadas.

Fuentes: PanelAdministrador.h, PanelPrincipal.cpp, HistorialSimulaciones.h, ComparadorSimulaciones.h, VentanaAmortizacion.h y ActividadAdministrativa.h. Este inventario refleja las muestras iniciales del código, no modificaciones temporales de una sesión abierta.
