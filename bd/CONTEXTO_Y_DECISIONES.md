# Base de datos simplificada de FinancionantiMex

Actualizado: 23/11/2026. Script: Financionanti.sql.
Instalación en una base nueva; no modifica automáticamente una base existente.

## Las ocho tablas

- usuario: nombres, apellidos, correo, nacimiento, hash de contraseña, foto, fechas y última aceptación de términos.
- cliente: identifica qué usuarios son clientes.
- administrador: identifica administradores y conserva su folio y eliminación lógica.
- deshabilitacion: episodios de suspensión con motivo, cliente, fecha y reactivación. Permite conservar antecedentes sin sobrescribirlos.
- simulacion: propietario, monto, tasa, plazo, moneda, nombre y fechas.
- cuota: número de periodo, interés y abono a capital por simulación.
- apelacion: solicitud relacionada con una suspensión; mensaje, estado, respuesta y administrador que resuelve.
- actividad_administrativa: acciones realizadas y sus responsables, con autorización adicional cuando corresponda.

## Simplificaciones acordadas

Se quitaron version_calculo, terminos y aceptacion_terminos.
El método de cuota fija mensual se implementará en C++; editar monto, tasa o plazo no requiere una tabla de métodos.
El documento de términos permanece en TerminosRegistro.h. usuario.terminos_version y usuario.terminos_aceptados_en guardarán solo la aceptación más reciente, no el historial de todas las versiones.
Ambos campos comienzan NULL: no se atribuye consentimiento a las cuentas iniciales.
No se duplican datos personales ni se crean catálogos innecesarios. Simplificar no obliga a introducir datos repetidos.

## Datos y cálculos

Se conservan 12 usuarios, cinco administradores con sus folios originales, siete clientes, 36 simulaciones, 619 cuotas y una apelación pendiente vinculada a la suspensión de Ana.
Los clientes iniciales tienen nombres ficticios plausibles y correos reservados example.com. Para cuentas reales se requieren correos verificados. Los administradores conservan la información proporcionada por el usuario.
La actividad comienza vacía y solo debe registrar acciones realizadas.
password_hash NULL indica contraseña pendiente de establecer; el backend no debe permitir acceso con hash NULL.
foto_ruta NULL utiliza el avatar predeterminado.
Pago de cuota = interés + capital. Saldo inicial = monto menos capital anterior; saldo final = saldo inicial menos capital del periodo.
Intereses y pagos totales se suman desde cuota. La mensualidad regular se calcula en C++ con la misma regla de redondeo; la última cuota puede ajustar centavos.
Historial, comparador y PDF consultarán las mismas simulaciones y cuotas; se excluyen las eliminadas lógicamente.

## Reglas del backend

Validar mayoría de edad, correo y folio únicos, contraseña y confirmación, rol, permisos, límites y coherencia de amortización.
Guardar la aceptación explícita de términos al registrar la cuenta. Recorrer el documento no prueba comprensión.
Una deshabilitación sin reactivada_en impide acceso. No permitir dos episodios abiertos del mismo cliente. Aprobar apelación debe registrar resolución y reactivación conjuntamente.
Activo/inactivo se calcula por último acceso en seis meses y es distinto de deshabilitado.
Clientes solo se deshabilitan/reactivan. Administradores y simulaciones usan eliminación lógica para conservar relaciones.
Confirmar cambios con el folio de la sesión; revelar únicamente el folio propio con autorización de otro administrador. No guardar credenciales en actividad.
Los cinco folios se conservan inmutables aunque se editen nombres o nacimiento.
SQL sencillo por petición: sin UNIQUE, CHECK, índices auxiliares, DEFAULT, ENGINE ni COLLATE. Las reglas de negocio corresponden al backend.

## Verificación y pendientes

Carga de una adaptación sintáctica en SQLite en memoria con claves foráneas habilitadas: sin referencias rotas. Comprobados conteos, secuencias de cuotas y suma de capital igual al monto para todas las simulaciones.
No se ejecutó en MySQL. Falta probar allí, establecer contraseñas, conectar el backend y gestionar almacenamiento de fotos y protección de folios.
El frontend conserva su comportamiento actual: todavía trabaja con datos temporales y no está conectado a esta base.
