# Contexto de la base de datos FinancionantiMex

Fecha de preparación: 15/11/2026. Propuesta, sin ejecutar en MySQL.
El script 01_esquema_y_muestras.sql contiene CREATE TABLE e INSERT INTO completos.
Se propone MySQL 8.0.16 o posterior; Workbench es el cliente de administración, no el servidor.
Ejecutar una sola vez en una base nueva y revisar primero las decisiones de este documento.
No incluye DROP ni borra datos existentes. CREATE TABLE fallará si las tablas ya existen.
El DDL MySQL no se revierte con el ROLLBACK de los datos; verificar creación de tablas antes de insertar.
No ejecutar el script en una base con información real.

## Alcance actual

Aplicación Qt Widgets C++17. Registro, acceso, cliente y administración son mayormente prototipos con estado en memoria.
El historial genera amortizaciones reales para condiciones de muestra y exporta PDFs. Comparador usa un catálogo precargado independiente.
No existe autenticación de producción. La selección por folio identifica solo la sesión de demostración.
Los datos editados y las fotos elegidas en una ejecución no se recuperan al reiniciar.

## Modelo y relaciones

usuario guarda atributos comunes. cliente y administrador representan especializaciones (uno a uno).
Cada usuario debe pertenecer a exactamente una especialización; el servicio debe garantizarlo en una transacción.
El esquema propuesto NO impide por sí solo que se cree un usuario en ambas especializaciones.
cliente tiene muchas simulaciones; cada simulacion tiene muchas cuotas.
cliente tiene muchas apelaciones, con solo una pendiente por cliente mediante índice único.
administrador resuelve apelaciones y realiza acciones registradas en actividad_administrativa.
actividad_demo está separada: sus entradas no prueban hechos reales.
Entidad/entidad_id en auditoría son referencias descriptivas polimórficas, no claves foráneas.
Las relaciones reales entre usuarios y registros usan IDs, nunca nombres o índices de arreglos.

## Semillas y asignaciones propuestas

12 usuarios: 5 administradores y 7 clientes. Los 6 clientes originales mantienen sus dos simulaciones cada uno.
Propuesta explícita: la séptima cuenta (Cliente de muestra, cliente@demo.mx) recibe los 20 ejemplos del historial y 4 variantes adicionales del comparador.
Así hay 36 simulaciones y 619 cuotas. Los dos ejemplos Computadora comunes se reutilizan, no se duplican.
Curso de idiomas del comparador (sin interés, 10 meses) es distinto al del historial (8 %, 12 meses).
No existía esta propiedad en el frontend: es una decisión propuesta para los datos iniciales, no un hecho recuperado de una sesión.
La fecha de referencia para últimos accesos y apelación es 15/11/2026; las fechas de inserción de simulaciones las asigna MySQL al ejecutar.
No se inventan fechas históricas de guardado.
Los cinco folios y datos administrativos se preservan exactamente.
Uziel (21/12/2007) tiene 18 años el 21/12/2025: al 15/11/2026 YA ES MAYOR DE EDAD.
Todos los cinco administradores cumplen 18 años a la fecha de referencia.

## Contraseñas, folios y fotografías

password_hash permanece NULL en las semillas; significa credencial pendiente, jamás contraseña vacía aceptada.
El backend debe rechazar el acceso con hash NULL. Definir contraseñas mediante alta/activación y generar hashes con un algoritmo para contraseñas (no SHA-256 simple).
El folio de esta propuesta se guarda legible para preservar la muestra. No es una implementación de protección criptográfica.
Para producción, decidir cifrado recuperable con clave externa antes de migrar; debe poder revelarse solo por el flujo aprobado.
No enviar el folio en listados ni guardarlo en actividad o logs. Limitar intentos y verificar permisos en el servicio.
Nunca incluir contraseñas, hashes ni folios en antes/despues de auditoría.
Las fotos arrancan NULL (icono predeterminado). foto_ruta será una referencia a almacenamiento gestionado, no una ruta arbitraria del equipo del usuario.
Validar tipo, tamaño y propietario; reutilizar el encuadre cuadrado que ya genera el frontend.

## Reglas de negocio a implementar

- Registro de cliente: fecha válida, edad mínima 18 años según fecha local de México, nombre y correo válidos, correo único y confirmación de contraseña.
- Edad se valida en el servicio; no usar una columna de edad fija que quede obsoleta.
- Activo/inactivo se deriva de ultimo_acceso: no más de seis meses es activo. Deshabilitado es un estado distinto que impide acceso.
- Solo deshabilitar/reactivar clientes: no eliminar sus cuentas.
- Administradores con permisos iguales pueden consultar/editar cuentas y simulaciones y resolver apelaciones.
- Confirmar cambios con el folio de quien tiene la sesión activa.
- Cada administrador modifica únicamente su propia foto y revela únicamente su propio folio, autorizado por el folio de otro administrador.
- Folio generado una sola vez: 3 letras mayúsculas normalizadas + ddmmaa + 2 letras aleatorias + 2 dígitos. Nunca se modifica al cambiar nombre o nacimiento.
- Restricción UNIQUE y reintento ante colisión. Validar inmutabilidad en el servicio o con trigger antes de producción.
- No eliminar al administrador de la sesión ni al último administrador habilitado.
- Propuesta: eliminación lógica de administradores y simulaciones para conservar relaciones y auditoría.
- No incluir eliminados en login, listados, historial ni comparador.
- Apelación: motivo no vacío; aprobación y reactivación en una transacción. Rechazo conserva cuenta deshabilitada.
- Todos los administradores ven actividad; no pueden editarla ni borrarla mediante la aplicación.
- Tabla actividad real inicia vacía; 24 muestras se insertan solo en actividad_demo.
- Monto, tasa y plazo conservan límites actuales del frontend: monto hasta 999999999.99, tasa 0–1000 %, plazo 1–600 meses. Son límites técnicos actuales, no límites comerciales definitivos.
- La BD tiene CHECK básicos, pero todavía corresponde imponer el máximo del monto, unicidad de rol, inmutabilidad de folio, permisos y coherencia agregada en el servicio.
- Guardar simulación y todas sus cuotas en una transacción. Al editar condiciones, recalcular resultados y reemplazar cuotas en la misma transacción; nunca mezclar versiones.
- Al eliminar una simulación usar eliminación lógica; excluirla del comparador.
- Comparador lee las simulaciones del propietario activo y no requiere una tabla separada.
- PDF se genera desde la misma versión de simulación/cuotas. No necesita guardarse como BLOB por ahora.

## Cálculo de las muestras

Se reproduce el algoritmo C++ de cuota fija, tasa mensual nominal anual / 1200 y redondeo a centavos.
La última cuota liquida el saldo residual. Se guardan valores decimales exactos en SQL.
Se verificaron en el generador el capital amortizado y saldo final de las 36 tablas.
El script incorpora una consulta para comparar número de cuotas y totales contra las simulaciones.
Antes de producción, adoptar una política decimal de cálculo común y versionada, con pruebas para tasa cero, plazos límite y redondeo.
Las cifras precargadas del comparador pueden diferir por redondeo; el inventario antiguo las conserva, pero las semillas nuevas usan cuotas coherentes calculadas.
No se modelan desembolsos ni pagos reales: cuota representa un periodo proyectado de amortización, no un cobro recibido.

## Integración y pendientes

1. Confirmar este modelo con el diagrama de clases y las asignaciones de muestras.
2. Verificar versión del servidor MySQL y ejecutar en una base vacía de pruebas.
3. Revisar conteos, claves, consultas de verificación y totales.
4. Implementar acceso a datos con parámetros y transacciones.
5. Conectar autenticación/sesión, perfiles, historial, comparador, apelaciones y auditoría.
6. Configurar usuario SQL con privilegios mínimos; no usar root ni distribuir credenciales administrativas.
7. Definir almacenamiento protegido de credenciales/folios y fotos antes de usar datos reales.

Pruebas realizadas: generación y validación aritmética local de las semillas.
Pendiente: ejecutar DDL/INSERT y pruebas de restricciones contra un servidor MySQL. No afirmar que el script ya fue ejecutado o validado en Workbench.
