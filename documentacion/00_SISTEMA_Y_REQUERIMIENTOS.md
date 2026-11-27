# FinancionantiMex — guía del sistema y requisitos

Fecha de consolidación: 25/11/2026. Destinatarios: programador nuevo y asistente sin acceso al chat.
Este archivo es el punto de entrada. No es necesario releer todos los documentos para una tarea puntual.

## 1. Índice y autoridad

1. [Sistema y requisitos](00_SISTEMA_Y_REQUERIMIENTOS.md): alcance, recorrido, decisiones, estado y PDF fuente.
2. [Frontend](01_FRONTEND.md): diseño aprobado, mapa de ventanas y verificaciones visuales.
3. [Backend C++](02_BACKEND_CPP.md): reglas operativas, validaciones, permisos, cálculos, pruebas y plan de implementación.
4. [Base de datos](03_BASE_DE_DATOS.md): esquema real, conexión, consultas, transacciones y datos iniciales.
5. [Acuerdos de trabajo](04_ACUERDOS_DE_TRABAJO.md): preferencias, lecciones, dudas pendientes y forma de continuar.

Precedencia: nuevas instrucciones explícitas del propietario > decisiones definitivas de esta consolidación > requisitos del PDF no corregidos > implementación actual. El código demuestra lo existente, no define por sí solo lo deseado. Las propuestas no confirmadas están identificadas; no atribuirlas al usuario.
Estos cinco documentos sustituyen toda la documentación Markdown anterior del proyecto. No sustituyen el código ni convierten funcionalidades temporales en backend real.

## 2. Producto, entorno y alcance

Nombre exacto: **FinancionantiMex**. Eslogan: **Una forma impresionanti de obtener un préstamo**. La palabra «impresionanti» es intencional.
Aplicación de escritorio académica de Programación Orientada a Objetos, C++17 y Qt Widgets, ejecutada localmente en Windows. MySQL Server local administra los datos; MySQL Workbench es el cliente de administración, NO un servidor ni una API de conexión.
No crear web, API REST, servicios cloud, correo automático, chat o sistema de cobros sin un nuevo pedido. Cascaron es el boceto HTML histórico, no el frontend que se integrará.

Se simulan préstamos en MXN mediante cuota fija mensual sobre saldo insoluto (sistema francés). Se comparan simulaciones, consultan amortizaciones y exportan PDF. No se presta dinero, no hay aprobación de créditos reales, contratos, cobranza, CAT, seguros, comisiones ni impuestos calculados.
Una tasa 0 % es el caso sin intereses del MISMO método. Una tasa 1 %, 2 %, etc., no selecciona «interés simple». No crear una clase PrestamoInteresSimple ni selector de método ajeno al frontend.

Roles exclusivos: Cliente y Administrador. Los administradores tienen los mismos permisos; no hay superadministrador ni límite de cinco cuentas. Los cinco actuales son datos iniciales. El registro público crea únicamente clientes.

## 3. Directorios y ejecución

Raíz local conocida: `C:/Users/kikkk/OneDrive/Documentos/1UPIICSA/TERCER SEMESTRE/POO/Proyecto`.
- `aplicacion-cpp/`: aplicación activa, CMake y recursos.
- `aplicacion-cpp/bd/Financionanti.sql`: **único script SQL vigente** para esta integración.
- `base-datos/01_esquema_y_muestras.sql`: copia histórica desactualizada. No ejecutar, sincronizar ni usar como esquema vigente. No fue eliminada porque esta consolidación solo elimina otros .md.
- `Cascaron/`: ocho pantallas HTML antiguas, sin lógica conectada. No cambiarlo para arreglar Qt.
- `documentacion/`: los cinco documentos actuales.
- `tmp/`, carpetas de build: artefactos auxiliares, no fuente de verdad.
- Los DOCX/PDF de contexto anteriores en la raíz son históricos; las decisiones posteriores aquí prevalecen.

Qt conocido: 6.12.0 MinGW 64-bit, compilador MinGW 15.1. CMake requiere C++17, AUTOMOC/AUTORCC y los módulos Widgets, Sql, Network y Concurrent. Herramientas locales: `C:/Qt/Tools/CMake_64/bin/cmake.exe`, `C:/Qt/Tools/mingw1510_64/bin`, `C:/Qt/Tools/Ninja`, Qt en `C:/Qt/6.12.0/mingw_64`.
Abrir `aplicacion-cpp/CMakeLists.txt` en Qt Creator, elegir kit compatible, configurar y ejecutar Ctrl+R. No mezclar MinGW de otra versión ni DLL de otra instalación de Qt. Para compilar en terminal, anteponer MinGW y Ninja al PATH y ejecutar `cmake --build aplicacion-cpp/build-codex` si ese build ya está configurado. Un equipo nuevo debe configurar su propia carpeta de build.
No terminar el proceso de la aplicación del usuario sin avisar; para pruebas usar build separado. La compilación no equivale a validación visual ni a prueba de persistencia.

## 4. Estado real al entregar esta guía

| Área | Existe ahora | Pendiente para funcionamiento definitivo |
|---|---|---|
| Interfaz | Tema, navegación, formularios, tarjetas, filtros, fotos, ayuda | Mantener diseño y corregir solo defectos necesarios durante integración |
| Login | Autenticación MySQL/PBKDF2, folio de la cuenta, rol, suspensión y cierre de sesión por ID | Integrar autorización y persistencia de los módulos restantes |
| Registro | Validación compartida, ambos apellidos obligatorios, términos manuales y alta transaccional usuario + cliente | Sin pendientes de este flujo en el alcance acordado |
| Inicio | Validación, motor en centavos, tabla completa, recalcular tras cambios y guardado atómico en MySQL | Conectar historial, detalle/PDF y comparador con las simulaciones guardadas |
| Historial | 20 registros locales, búsqueda, iconos, borrado temporal | Consulta del propietario y eliminación lógica SQL |
| Amortización/PDF | Cálculo local de condiciones de la fila y PDF multipágina | Consultar cuotas guardadas coherentes y fechas reales |
| Comparador | Seis opciones y resultados precargados independientes | Usar historial del propietario y eliminar resultados hardcodeados |
| Mi cuenta/fotos | Perfil propio, contraseña y foto persistentes; validaciones y rechazo de cambios vacíos | Integrar las fichas de clientes de administración |
| Administración | Sesión real y folio contra BD; directorio de administradores desde SQL; gestión todavía temporal | Persistir cambios, clientes, apelaciones y auditoría |
| Actividad | 24 ejemplos locales y filtros/paginación | Registrar operaciones realizadas y leer SQL |
| Base de datos | QMYSQL verificado y script completo instalado en MySQL de pruebas | El propietario recreará su base con el script actualizado |
| Contraseñas | Login, registro, semillas y cambios de contraseña propios usan PBKDF2-HMAC-SHA-256 | Integrar edición administrativa de terceros |

El frontend está **aprobado visualmente**, no terminado funcionalmente. La memoria temporal, textos «demo», perfiles independientes y botones deshabilitados deben retirarse al sustituirlos por operaciones reales. No limitarse a ocultar etiquetas para declarar terminado el backend.

## 5. Recorridos finales

Cliente: login → validación → inicio/simulador → calcular → guardar → historial → detalle/amortización/PDF → comparador de dos registros propios. Puede consultar Acerca de, editar su perfil/foto y cerrar sesión.
Cliente deshabilitado: validar credenciales → pantalla restringida con motivo/fecha y apelaciones → enviar o consultar solicitud → cerrar sesión. No accede por atajos a módulos operativos.
Administrador: validar correo y contraseña → pedir folio de ESA cuenta → Clientes / Administradores / Apelaciones / Actividad. Antes de cambios, confirmar con el folio del administrador en sesión. Para revelar su propio folio, autoriza OTRO administrador; nadie puede solicitar revelar el folio ajeno.
Registro: completar datos → leer documento de términos hasta el final → volver → marcar casilla manualmente → validar y guardar cuenta Cliente. La interfaz actual abre Inicio; mantener ese recorrido solo después de establecer una sesión real y actualizar su último acceso.
Cerrar sesión destruye la sesión, limpia credenciales, autorizaciones y datos de la cuenta en las vistas, y regresa al login sin conservar acceso operativo.

## 6. Correcciones definitivas al PDF

| Identificador | Decisión vigente |
|---|---|
| RN-27 / RF-47 | **Eliminación lógica**, confirmada definitivamente por el usuario. Actualizar eliminada_en; conservar simulación y cuotas. Excluirlas de historial, comparador y listados normales. No papelera ni recuperación solicitadas. |
| RN-37 / RF-121 | Primeras tres letras normalizadas de los nombres + ddMMyy + dos letras A–Z + dos dígitos. No desplazar letras ni aplicar PAO/AOL/OLO/LOP. La unicidad es del folio completo; regenerar sufijo ante colisión. |
| RN-44 / RF-141 | Aprobar reactiva el acceso. Activo/inactivo sigue dependiendo de ultimo_acceso; no falsear esa fecha al aprobar. Se vuelve activo al iniciar sesión correctamente. |
| RF-107 | El administrador solo modifica su propia foto. Puede editar los otros datos de administradores con su folio de sesión; no revelar folio ajeno. |
| RN-51 / RF-103 | Consultar cualquier ficha administrativa, incluida la propia, requiere el folio de la sesión activa. |
| RN-13/14/15 | Rangos definitivos: monto 1.00–10,000,000.00 MXN; tasa 0.00–200.00 % con dos decimales; plazo entero 1–480 meses. Corregir límites antiguos del frontend y validarlos en backend. |
| RN-12 | Nombre simulación 1–50 caracteres: letras, acentos, números, espacios y guion bajo. Corregir nombres de semillas con paréntesis; no ampliar silenciosamente la regla. |
| RF-117 | Alta de administrador con Nombre(s), Apellido paterno y Apellido materno separados, además de correo, nacimiento, contraseña y confirmación. |
| RF-15/21 | Casilla manual; recorrer documento hasta el final antes de aceptar. Si se pulsa antes, advertencia y no cambiar su estado. Guardar versión y fecha; no prometer comprobar lectura/comprensión. |
| Regla complementaria | El administrador puede eliminar lógicamente simulaciones del cliente con autorización y registro; el PDF no desarrolla suficientemente esa acción. |
| Seguridad | Acuerdo: **PBKDF2 con SHA-256 mediante Qt**, no SHA-256 simple ni cifrado reversible de contraseñas; sin biblioteca criptográfica externa para esa función. |
| Despliegue | Uso local en este equipo; no arquitectura multiusuario remota ni servicio web. |

Los nombres completos de administradores y folios iniciales están en 03. Christian Jared conserva su folio JAR… histórico aunque su primer nombre sea Christian. Uziel Alexander conserva UZI…. No regenerarlos.
La política de eliminación lógica de administradores también se conserva; impide eliminar sesión propia o último administrador no eliminado.

## 7. Cobertura por módulo

| Sección | Requisitos PDF | Implementación futura principal |
|---|---|---|
| Login y sesión | RF-01–12; RN-01–06 | Autenticación, rol, estado, folio, último acceso |
| Registro | RF-13–24; RN-07–11 | Validación, aceptación y alta Cliente |
| Simulador | RF-25–38; RN-12–23 | Motor francés y guardado |
| Historial y PDF | RF-39–47; RN-24–27 | Propiedad, cuotas, PDF y eliminación lógica |
| Comparador | RF-48–60; RN-28–32 | Selecciones distintas, resultados y empates |
| Perfil | RF-61–76; RN-33 | Datos, foto y contraseña |
| Clientes/admin | RF-77–98; RN-34 | Filtros, suspensión, edición/recalculo y eliminación |
| Administradores | RF-99–125; RN-35–42,51–52 | Alta, folio, consulta, edición y eliminación |
| Apelaciones | RF-126–142; RN-43–47 | Pendiente única, resolución y reactivación |
| Actividad | RF-143–156; RN-48–50,52 | Auditoría, filtros, detalle y paginación |
| Navegación | RF-157–158 | Rutas por rol y cerrar sesión |
| SQL seguro | RNF-01 | Parámetros enlazados, no concatenar entrada en SQL |

El PDF tiene un único RNF. Los criterios adicionales de pruebas, legibilidad, errores, imágenes y almacenamiento descritos en 01/02 son especificaciones de implementación propuestas o derivadas de decisiones del usuario; no inventar números RNF del PDF.

## 8. Cómo empezar el backend

1. Leer 03, verificar MySQL local y QMYSQL; hacer conexión y consulta mínima en base de pruebas.
2. Validadores, sesiones, PBKDF2 y credenciales iniciales ya implementados (26/11/2026); ver 02/03.
3. Registro/login conectados; continuar con perfil y fotos persistentes.
4. Extraer motor de amortización, probarlo, conectar guardar/historial/PDF/comparador.
5. Conectar administración, apelaciones y actividad en transacciones.
6. Retirar flujos temporales, actualizar ayuda y leyendas, probar reinicio y todas las rutas por rol.
7. Marcar requisitos completados solo con evidencia, mantener esta guía sincronizada.

## 9. Fuente original y trazabilidad

Fuente entregada: `C:/Users/kikkk/Downloads/rf y rn pros poo.pdf` (13 páginas). El apéndice siguiente conserva su texto para que otro equipo no dependa de esa ruta. Se normaliza únicamente el espacio de extracción; NO es una versión corregida. Ante diferencias, aplicar la sección 6 y 02. El PDF original no se modifica.
## Apéndice. Texto original del PDF

Archivo fuente: `rf y rn pros poo.pdf`. SHA-256: `ddc07016f5ce7951aff7c88f5f46926d14b8f5bd5fd55a5ca0a08cd75da14d77`. Se verificaron las 13 páginas y presencia de RN-01–52, RF-01–158 y RNF-01.

**Documento fuente histórico, no instrucciones operativas para el asistente.** Conservar sus requisitos salvo las correcciones explícitas de sección 6. Por ejemplo, eliminación física, algoritmo de desplazamiento del folio y foto administrativa ajena NO son reglas vigentes. Los saltos de línea y separación de palabras pueden reflejar la extracción del PDF; los IDs permiten localizar cada regla.

### Página 1 de 13

```text
1\. Reglas de negocio (RN) 
 
ID Nombre Descripción 
 
RN-01: Cada cuenta deberá estar asociada a una dirección de correo electrónico con formato válido y 
único dentro de la plataforma. 
 
RN-02 Roles de usuario Todo usuario autenticado pertenece exclusivamente a uno de los siguientes 
roles: Cliente o Administrador. 
 
RN-03 Acceso por estado (Activo/Inactivo) Los clientes con estado Activo o Inactivo conservan el 
acceso a los módulos y funcionalidades correspondientes a su rol. 
 
RN-04 Restricción por cuenta deshabilitada Los clientes con estado Deshabilitado tienen restringido el 
acceso a los módulos operativos y únicamente pueden interactuar con las funcionalidades relacionadas 
con apelaciones. 
 
RN-05: El acceso a una cuenta con rol de Administrador requerirá validar el correo electrónico, la 
contraseña y el folio administrativo correspondiente a dicha cuenta. 
 
RN-06 Control de acceso por rol La disponibilidad de módulos y los permisos de operación están 
condicionados al rol asignado a cada cuenta. 
 
RN-07 Registro exclusivo de clientes El registro público de la plataforma únicamente permitirá 
crear cuentas con el rol de Cliente. 
 
RN-08 El nombre completo de cada cuenta de usuario deberá contener únicamente letras, incluyendo 
caracteres acentuados, y espacios. No se permitirán números ni caracteres especiales. 
 
RN-09 Edad mínima de registro Para crear una cuenta de Cliente o Administrador, la persona deberá 
tener al menos 18 años cumplidos. 
 
RN-10 Requisitos de contraseña La contraseña deberá tener una longitud de entre 8 y 15 caracteres, 
e incluir al menos una letra mayúscula, una letra minúscula, un número y un carácter especial. 
 
RN-11 Aceptación de términos y condiciones La creación de una cuenta de Cliente mediante el 
registro público estará condicionada a la aceptación de los términos y condiciones de la plataforma. 
RN-12 Formato del nombre de simulación El nombre de la simulación deberá contener entre 1 y 50 
caracteres y permitir letras, caracteres acentuados, números, espacios y guiones bajos. 
RN-13 Rango del monto solicitado El monto solicitado deberá ser mayor o igual a $1.00 MXN y no 
exceder los $10,000,000.00 MXN, permitiendo hasta dos decimales. 
RN-14 Rango de tasa nominal anual La tasa nominal anual deberá encontrarse entre 0 % y 200 %, 
permitiendo hasta dos decimales. 
RN-15 Rango del plazo de pago El tiempo para pagar deberá expresarse en meses enteros, con un 
mínimo de 1 y un máximo de 480 meses. 
RN-16 Método de amortización Las simulaciones deberán calcularse mediante el sistema de 
amortización francés, utilizando una mensualidad constante durante el plazo del préstamo, salvo los 
ajustes necesarios en el último pago por redondeo.
```

### Página 2 de 13

```text
RN-17 Tratamiento de tasa cero Cuando la tasa nominal anual sea del 0 %, la mensualidad deberá 
calcularse dividiendo el monto solicitado entre el número de meses del plazo. 
RN-18 Composición de la mensualidad Cada mensualidad deberá estar compuesta por una parte 
correspondiente a intereses y otra destinada a reducir el capital pendiente. 
RN-19 Integridad de la amortización En cada periodo, el saldo final deberá corresponder al saldo inicial 
menos el capital amortizado. El saldo final de un periodo deberá coincidir con el saldo inicial del 
siguiente. Al finalizar el plazo, el saldo pendiente deberá ser cero, considerando los ajustes de redondeo. 
RN-20 Recuperación del capital La suma de las cantidades destinadas a capital durante todos los 
periodos deberá corresponder al monto originalmente solicitado. 
RN-21 Naturaleza informativa de la simulación Los resultados generados por el simulador tendrán 
carácter informativo y no representarán una oferta de crédito, contrato financiero ni compromiso de 
otorgamiento de préstamo. 
RN-22 Actualización de resultados Cuando el cliente modifique alguna condición del préstamo después 
de realizar un cálculo en el módulo Simulador, deberá ejecutar nuevamente la acción de calcular para 
obtener resultados actualizados antes de guardar la simulación. 
RN-23 Condición para guardar simulaciones Una simulación únicamente podrá guardarse cuando 
se haya realizado correctamente el cálculo de sus condiciones y resultados. 
RN-24: el cliente solo puede consultar sus propias simulaciones. 
RN-25 Consulta de simulaciones guardadas El historial estará compuesto por las simulaciones que 
hayan sido guardadas correctamente por el usuario. 
RN-26 — Conservación de simulaciones Las modificaciones administrativas de simulaciones están 
permitidas. Cada modificación deberá conservar y registrar los valores anteriores y posteriores, 
identificando la acción en el historial de actividad administrativa. 
RN-27 Eliminación de simulaciones propias El cliente podrá eliminar las simulaciones que le pertenecen. 
La eliminación requerirá confirmación y, una vez ejecutada, el registro será eliminado definitivamente 
de la base de datos. 
 
RN-28: el cliente solo puede comparar simulaciones que le pertenecen. 
 
RN-29 Selección de simulaciones para comparar La comparación deberá realizarse entre dos 
simulaciones guardadas, una asignada a la Opción A y otra a la Opción B. 
RN-30 Comparación de condiciones financieras Las simulaciones deberán compararse considerando 
el monto solicitado, tasa nominal anual, plazo, mensualidad, intereses totales y total por pagar. 
RN-31 Identificación de diferencias y empates Cuando los valores comparados sean iguales, el 
sistema deberá identificar los empates y evitar señalar una opción como mejor cuando no exista 
diferencia en el criterio evaluado. 
 
RN-32 Comparación de simulaciones diferentes Las opciones A y B deberán corresponder a 
simulaciones distintas. No se permitirá seleccionar la misma simulación en ambas opciones. 
 
RN-33 Actualización parcial del perfil El cliente podrá modificar uno o varios de sus datos 
personales sin estar obligado a actualizar todos los campos. Los datos que no sean modificados deberán 
conservar sus valores actuales. 
 
RN-34 Autorización de operaciones administrativas Las operaciones administrativas que 
impliquen modificaciones en la información o el estado de las cuentas deberán ser autorizadas mediante 
el folio del administrador que tenga la sesión iniciada.
```

### Página 3 de 13

```text
RN-35 Restricción de eliminación de cuenta propia Un administrador no podrá eliminar su propia cuenta 
mientras tenga una sesión iniciada en ella. 
RN-36 Conservación de administradores del sistema El sistema deberá garantizar que siempre 
exista al menos una cuenta con rol de Administrador, por lo que no permitirá eliminar al último 
administrador registrado. 
 
RN-37 Formato y generación del folio administrativo El folio estará compuesto por 13 caracteres. 
Las primeras tres letras se obtendrán de una secuencia de tres caracteres consecutivos del nombre del 
administrador. Se comenzará con los primeros tres caracteres y, si la combinación ya existe, se desplazará 
una posición hasta encontrar una combinación disponible. 
 
Ejemplo para el nombre Paolo: 
Intento Combinación 
1 PAO 
2 AOL 
3 OLO 
4 LOP 
 
Una vez obtenida una combinación disponible, se completa el folio con la fecha de nacimiento y los 
caracteres aleatorios establecidos. 
RN-38 Unicidad del folio administrativo Cada administrador deberá contar con un folio 
administrativo único dentro del sistema. 
RN-39 Inmutabilidad del folio administrativo El folio administrativo no podrá modificarse una vez 
generado y conservará su valor aunque se actualicen el nombre o la fecha de nacimiento del administrador. 
RN-40 Entrega del folio administrativo El folio deberá proporcionarse al administrador durante el 
proceso de creación de su cuenta. 
RN-41 Confidencialidad del folio administrativo El folio administrativo no  deberá mostrarse 
públicamente ni exponerse en las fichas de información de los administradores o en otras secciones del 
sistema. 
RN-42 Revelación autorizada del folio propio Un administrador podrá revelar su propio folio mediante 
la opción «Revelar mi folio». Esta acción abrirá una ventana en la que otro administrador deberá 
introducir su propio folio para autorizar la revelación. El adm inistrador que autoriza no podrá consultar 
el folio del otro administrador. 
 
RN-43 Una apelación pendiente por cliente Cada cliente podrá mantener como máximo una apelación 
en estado Pendiente al mismo tiempo. 
RN-44 Reactivación por apelación aprobada Cuando una apelación sea aprobada, la cuenta del 
cliente deberá cambiar al estado Activo. 
RN-45 Conservación del estado por apelación rechazada Cuando una apelación sea rechazada, la 
cuenta del cliente deberá permanecer en estado Deshabilitado. 
RN-46 Justificación de rechazo El rechazo de una apelación deberá incluir un motivo que justifique 
la decisión administrativa. 
RN-47 Nueva apelación después de un rechazo Un cliente podrá presentar una nueva apelación 
después de que su solicitud anterior haya sido rechazada. 
RN-48 Trazabilidad de acciones administrativas Las acciones realizadas por los administradores 
deberán conservar un registro que permita identificar al responsable, la operación ejecutada, el elemento 
afectado y el momento en que se realizó. 
RN-49 Integridad del historial administrativo Los registros de actividad administrativa deberán 
conservarse sin modificaciones que alteren la información original de las operaciones realizadas.
```

### Página 4 de 13

```text
RN-50 Registro de modificaciones administrativas Cuando una operación administrativa modifique 
información existente, el registro de actividad deberá conservar los valores anteriores y posteriores al 
cambio. 
 
RN-51 Autorización para consultar información administrativa La consulta de la ficha de un 
administrador requerirá la validación del folio correspondiente al administrador que tenga la sesión 
iniciada. 
RN-52 Protección de información confidencial en registros Los registros de actividad 
administrativa no deberán exponer contraseñas ni folios administrativos. 
 
 
 
2\. Requerimientos funcionales (RF) 
 
 
 
Módulo Iniciar sesión 
 
RF-01 Ingresar credenciales El sistema deberá permitir al usuario introducir su correo electrónico y 
contraseña para iniciar sesión. 
 
RF-02 Visualizar contraseña El sistema deberá permitir mostrar u ocultar la contraseña introducida 
mediante un control de visualización. 
 
RF-03 Validar cuenta de usuario El sistema deberá  verificar que el correo electrónico ingresado 
corresponda a una cuenta registrada en la plataforma. 
 
RF-04 Validar credenciales El sistema deberá comprobar que la contraseña ingresada corresponda a la 
cuenta identificada. 
 
RF-05 Identificar rol de usuario El sistema deberá identificar el rol asociado a la cuenta autenticada 
para determinar el flujo de acceso correspondiente. 
 
RF-06 Validar estado de cuenta El sistema deberá consultar el estado de la cuenta del cliente para 
determinar las restricciones de acceso aplicables. 
 
RF-07 Restringir acceso de clientes deshabilitados El sistema deberá impedir el acceso de clientes con 
cuentas deshabilitadas a los módulos operativos y dirigirlos al flujo de apelaciones. 
 
RF-08 Solicitar folio administrativo El sistema deberá solicitar el folio administrativo después de validar 
correctamente las credenciales de un administrador. 
 
RF-09 Validar folio administrativo El sistema deberá comprobar que el folio ingresado corresponda al 
administrador que intenta iniciar sesión. 
 
RF-10 Gestionar acceso administrativo El sistema deberá conceder el acceso a los módulos 
administrativos cuando las credenciales y el folio sean válidos.
```

### Página 5 de 13

```text
RF-11 Actualizar último inicio de sesión El sistema deberá registrar la fecha y hora del último inicio 
de sesión cuando la autenticación sea exitosa. 
 
RF-12 Acceder al registro El sistema deberá permitir navegar desde la pantalla de inicio de sesión 
hacia el módulo de registro. 
 
 
 
Módulo registro 
 
RF-13 Registrar datos personales El sistema deberá permitir al usuario introducir su nombre, apellido 
paterno, apellido materno, correo electrónico y fecha de nacimiento para crear una cuenta. 
 
RF-14 Registrar contraseña El sistema deberá permitir al usuario introducir una contraseña y su 
confirmación durante el proceso de registro. 
 
RF-15 Aceptar términos y condiciones El sistema deberá proporcionar un control que permita al 
usuario aceptar los términos y condiciones de la plataforma. 
 
RF-16 Validar nombre completo El sistema deberá verificar que el  nombre y apellidos contengan 
únicamente letras, incluyendo caracteres acentuados, y espacios. 
 
RF-17 Validar correo electrónico El sistema deberá verificar que el correo electrónico ingresado 
cumpla con un formato válido y no esté asociado a otra cuenta registrada. 
 
RF-18 Validar edad del usuario El sistema deberá verificar que el usuario tenga al menos 18 años 
cumplidos. 
 
RF-19 Validar contraseña El sistema deberá comprobar que la contraseña tenga entre 8 y 15 
caracteres e incluya al menos una mayúscula, una minúscula, un número y un carácter especial. 
 
RF-20 Confirmar contraseña El sistema deberá verificar que la contraseña ingresada y su confirmación 
coincidan exactamente. 
 
RF-21 Validar aceptación de términos El sistema deberá verificar que el usuario haya aceptado los 
términos y condiciones antes de continuar con el registro. 
 
RF-22 Proporcionar retroalimentación de validaciones El sistema deberá informar los errores 
detectados en los campos correspondientes e indicar las condiciones que deben cumplirse para corregirlos. 
 
RF-23 Crear cuenta de cliente El sistema deberá registrar una nueva cuenta con rol de Cliente 
cuando todos los datos y condiciones requeridos sean válidos. 
 
RF-24 Acceder al inicio de sesión El sistema deberá permitir al usuario regresar al módulo de inicio 
de sesión desde la pantalla de registro. 
 
 
 
Módulo simulador
```

### Página 6 de 13

```text
RF-25 Registrar condiciones del préstamo El sistema deberá permitir al usuario introducir el nombre 
de la simulación, monto solicitado, tasa nominal anual y tiempo para pagar. 
RF-26 Validar nombre de simulación El sistema deberá verificar que el nombre de la simulación 
cumpla con el formato y longitud establecidos. 
RF-27 Validar monto solicitado El sistema deberá comprobar que el monto ingresado sea numérico, 
tenga una precisión máxima de dos decimales y se encuentre dentro del rango permitido. 
RF-28 Validar tasa nominal anual El sistema deberá comprobar que la tasa ingresada sea numérica, 
tenga una precisión máxima de dos decimales y se encuentre dentro del rango permitido. 
RF-29 Validar tiempo para pagar El sistema deberá verificar que el plazo ingresado sea un número 
entero comprendido dentro del rango permitido. 
RF-30 Validar condiciones del préstamo en el sistema El sistema deberá validar nuevamente las 
condiciones del préstamo antes de realizar los cálculos de la simulación, independientemente de las 
validaciones realizadas en la interfaz. 
RF-31 Calcular mensualidad El sistema deberá calcular la mensualidad correspondiente al monto, tasa 
nominal anual y plazo ingresados, aplicando el método de amortización francés. 
RF-32 Calcular intereses totales El sistema deberá calcular el importe total de intereses generado 
durante el plazo del préstamo. 
RF-33 Calcular total por pagar El sistema deberá calcular el importe total que deberá pagarse 
durante el plazo de la simulación, incluyendo capital e intereses. 
RF-34 Generar tabla de amortización El sistema deberá generar el desglose de los pagos 
correspondientes a cada mes, considerando saldo inicial, interés, capital, pago y saldo final. 
RF-35 Mostrar resultados de simulación El sistema deberá presentar la mensualidad, los intereses 
totales y el total por pagar calculados para el préstamo. 
RF-36 Ejecutar cálculo de préstamo El sistema deberá realizar los cálculos y actualizar los resultados y 
la tabla de amortización cuando el usuario active el botón «Calcular préstamo». 
RF-37 Consultar explicación de amortización El sistema deberá permitir al usuario acceder al 
apartado «Acerca de» para consultar la explicación sobre el  funcionamiento e interpretación de la tabla 
de amortización. 
RF-38 Guardar simulación El sistema deberá permitir al usuario guardar la simulación calculada para 
su posterior consulta desde el módulo Historial, impidiendo el guardado cuando no exista un cálculo 
válido y actualizado. 
 
 
 
 
 
Módulo historial 
RF-39 Consultar historial de simulaciones El sistema deberá permitir al usuario consultar las 
simulaciones que haya guardado previamente. 
RF-40 Filtrar simulaciones por nombre El sistema deberá permitir buscar simulaciones mediante su 
nombre. 
RF-41 Mostrar simulaciones registradas El sistema deberá presentar las simulaciones guardadas en 
una tabla que incluya nombre, monto, tasa, plazo y acciones disponibles. 
RF-42 Consultar detalle de simulación El sistema deberá permitir visualizar las condiciones del 
préstamo, los resultados calculados y la tabla de amortización completa de una simulación seleccionada. 
RF-43 Exportar simulación a PDF El sistema deberá generar un documento PDF con la fecha de 
emisión, monto solicitado, tasa nominal anual, número de pagos mensuales, pago regular, total de 
intereses y la tabla de amortización correspondiente.
```

### Página 7 de 13

```text
RF-44 Seleccionar ubicación de exportación El sistema deberá permitir al usuario seleccionar la 
ubicación donde desea guardar el archivo PDF generado. 
RF-45 Consultar explicación de amortización El sistema deberá permitir acceder al apartado 
«Acerca de» desde la ventana de detalle de una simulación para consultar la explicación sobre la 
interpretación de la tabla de amortización. 
RF-46 Cerrar detalle de simulación El sistema deberá permitir cerrar la ventana de detalle y regresar al 
historial de simulaciones. 
RF-47 Eliminar simulación propia El sistema deberá permitir al cliente eliminar una simulación de su 
historial mediante la opción correspondiente, solicitando confirmación antes de eliminar definitivamente 
el registro de la base de datos. 
 
 
 
Módulo comparador 
 
RF-48 Consultar simulaciones disponibles El sistema deberá mostrar al usuario la lista de simulaciones 
que haya guardado previamente para utilizarlas en la comparación. 
RF-49 Seleccionar simulación para opción A El sistema deberá permitir al usuario seleccionar una 
simulación de su historial para asignarla a la Opción A. 
RF-50 Seleccionar simulación para opción B El sistema deberá permitir al usuario seleccionar una 
simulación de su historial para asignarla a la Opción B. 
RF-51 Buscar simulaciones por nombre El sistema deberá permitir al usuario localizar simulaciones 
mediante la selección de la lista desplegable o la introducción manual de su nombre. 
RF-52 Validar búsqueda de simulaciones El sistema deberá verificar que el nombre introducido 
corresponda a una simulación registrada y proporcionar retroalimentación cuando no se encuentre o 
contenga caracteres no permitidos. 
RF-53 Comparar simulaciones automáticamente El sistema deberá realizar la comparación y 
actualizar los resultados automáticamente cuando se hayan seleccionado correctamente las simulaciones 
de ambas opciones, sin requerir una acción adicional del usuario. 
RF-54 Mostrar mensualidades comparadas El sistema deberá mostrar el importe de la mensualidad 
correspondiente a cada simulación seleccionada. 
RF-55 Identificar menor mensualidad El sistema deberá identificar la opción que presente la 
mensualidad más baja o indicar que existe un empate cuando ambas sean iguales. 
RF-56 Identificar menor total por pagar El sistema deberá identificar la opción que presente el 
menor importe total por pagar o indicar que existe un empate cuando ambos importes sean iguales. 
RF-57 Identificar menor plazo El sistema deberá identificar la opción que presente el menor 
tiempo para pagar o indicar que existe un empate cuando ambos plazos sean iguales. 
RF-58 Mostrar tabla comparativa El sistema deberá presentar una tabla con tres columnas: Concepto, 
Opción A y Opción B, que incluya monto solicitado, tasa nominal anual, plazo, mensualidad, intereses 
totales y total por pagar. 
RF-59 Mostrar resultados sin diferencias El sistema deberá mostrar una leyenda indicando que no 
existen diferencias entre las opciones cuando todos los valores de las condiciones y resultados financieros 
comparados sean iguales. 
 
RF-60 Validar selección de simulaciones diferentes El sistema deberá verificar que las simulaciones 
seleccionadas para las opciones A y B sean diferentes e impedir la comparación cuando ambas 
correspondan al mismo registro.
```

### Página 8 de 13

```text
Módulo Perfil 
RF-61 Consultar perfil El sistema deberá permitir al cliente consultar la información asociada a su 
cuenta, incluyendo nombre, correo electrónico y fecha de nacimiento, además de mostrar un indicador 
visual de que existe una contraseña configurada, sin revelar su valor. 
RF-62 Mostrar foto de perfil El sistema deberá mostrar la fotografía de perfil asociada a la cuenta del 
cliente o el ícono predeterminado cuando no exista una fotografía personalizada. 
RF-63 Acceder al editor de foto de perfil El sistema deberá permitir abrir una ventana para modificar 
la fotografía de perfil mediante el control de cámara ubicado sobre la imagen actual. 
RF-64 Seleccionar imagen de perfil El sistema deberá permitir al cliente seleccionar una imagen desde 
el explorador de archivos de su dispositivo para utilizarla como fotografía de perfil. 
RF-65 Ajustar acercamiento de imagen El sistema deberá permitir modificar el acercamiento de la 
imagen mediante un parámetro comprendido entre el 100 % y el 300 %. 
RF-66 Ajustar posición horizontal de imagen El sistema deberá permitir modificar la posición 
horizontal de la imagen mediante un parámetro comprendido entre 0 y 100, donde los valores inferiores 
a 50 representan un desplazamiento hacia la izquierda y los superiores a 50 representan un 
desplazamiento hacia la derecha. 
RF-67 Ajustar posición vertical de imagen El sistema deberá permitir modificar la posición vertical de 
la imagen mediante un parámetro comprendido entre 0 y 100, donde los valores inferiores a 50 
representan un desplazamiento hacia arriba y los superiores a 50 representan un desplazamiento hacia 
abajo. 
RF-68 Restablecer foto predeterminada El sistema deberá permitir al cliente sustituir la imagen 
actual por el ícono predeterminado en la vista previa de edición, sin guardar el cambio hasta que se 
confirme la operación. 
RF-69 Cancelar edición de foto El sistema deberá permitir cancelar la edición de la fotografía y 
cerrar la ventana sin guardar los cambios realizados. 
RF-70 Guardar foto de perfil El sistema deberá permitir guardar la imagen seleccionada o el ícono 
predeterminado, junto con los ajustes realizados, al presionar el botón «Guardar foto». 
RF-71 Acceder a la edición del perfil El sistema deberá permitir abrir una ventana de edición de 
datos personales mediante el control correspondiente ubicado en el apartado Perfil. 
RF-72 Modificar datos personales El sistema deberá permitir al cliente modificar su nombre completo, 
correo electrónico y fecha de nacimiento. 
RF-73 Modificar contraseña El sistema deberá permitir al cliente introducir una nueva contraseña y su 
confirmación como parte de la actualización de su perfil. 
RF-74 Validar datos del perfil El sistema deberá aplicar las validaciones correspondientes al 
nombre completo, correo electrónico y edad, así como las validaciones de contraseña y confirmación 
cuando el cliente solicite modificarla, conforme a los criterios establecidos en RF-16 a RF-20. 
RF-75 Guardar cambios del perfil El sistema deberá permitir guardar las modificaciones realizadas en 
los datos personales del cliente cuando las validaciones correspondientes sean satisfactorias. 
RF-76 Cancelar edición del perfil El sistema deberá permitir cancelar la edición de los datos 
personales y cerrar la ventana sin guardar las modificaciones realizadas. 
 
 
 
Módulo clientes 
 
 
 
ID Nombre Descripción
```

### Página 9 de 13

```text
RF-77 Consultar clientes registrados El sistema deberá permitir al administrador consultar la lista 
de clientes registrados en la plataforma. 
RF-78 Buscar clientes por nombre El sistema deberá permitir localizar clientes mediante la 
introducción de su nombre. 
RF-79 Filtrar clientes por estado El sistema deberá permitir filtrar los clientes de acuerdo con su 
estado: Activo, Inactivo o Deshabilitado. 
RF-80 Ordenar clientes El sistema deberá permitir ordenar la lista de clientes alfabéticamente de A 
a Z, por acceso más reciente, por acceso más antiguo, por mayor número de simulaciones o por menor 
número de simulaciones. 
RF-81 Filtrar clientes por simulaciones El sistema deberá permitir filtrar los clientes de acuerdo con 
la existencia de simulaciones guardadas, mediante las opciones Todas, Con simulaciones y Sin 
simulaciones. 
RF-82 Limpiar filtros de búsqueda El sistema deberá permitir restablecer los filtros aplicados para 
volver a consultar la lista de clientes sin esos criterios. 
RF-83 Mostrar información de clientes El sistema deberá presentar en cada registro de la lista el 
nombre, correo electrónico, último acceso y estado del cliente. 
RF-84 Acceder a la ficha del cliente El sistema deberá permitir al administrador abrir la ficha de un 
cliente mediante la opción «Mostrar información». 
ID Nombre Descripción 
RF-85 Consultar información del cliente El sistema deberá mostrar en la ficha del cliente su nombre, 
fotografía de perfil, correo electrónico, último acceso, fecha de nacimiento, estado de la cuenta y número 
de simulaciones guardadas. 
RF-86 Consultar simulaciones del cliente El sistema deberá permitir al administrador consultar las 
simulaciones guardadas por el cliente, mostrando para cada una el monto solicitado, la tasa nominal anual 
y el plazo. 
RF-87 Acceder a la deshabilitación de cuenta El sistema deberá permitir al administrador iniciar el 
proceso de deshabilitación de la cuenta de un cliente mediante la opción «Deshabilitar cuenta». 
RF-88 Seleccionar motivo de deshabilitación El sistema deberá permitir al administrador 
seleccionar uno de los motivos predefinidos para deshabilitar una cuenta: Inactividad o Uso inapropiado. 
RF-89 Solicitar autorización para deshabilitar cuenta El sistema deberá solicitar el folio 
administrativo como confirmación antes de ejecutar la deshabilitación de la cuenta. 
RF-90 Validar folio para deshabilitar cuenta El sistema deberá verificar que el folio ingresado 
corresponda al administrador que tiene la sesión iniciada antes de autorizar la deshabilitación de la cuenta. 
RF-91 Acceder a la edición de simulación El sistema deberá permitir al administrador abrir la ventana 
de edición de las condiciones de una simulación seleccionada. 
RF-92 Modificar condiciones de simulación El sistema deberá permitir al administrador 
modificar el nombre, monto solicitado, tasa nominal anual y plazo de una simulación. 
RF-93 Guardar cambios de simulación El sistema deberá permitir guardar las modificaciones 
realizadas en las condiciones de una simulación, previa autorización administrativa. 
RF-94 Solicitar autorización para editar simulación El sistema deberá solicitar el folio administrativo 
antes de guardar las modificaciones realizadas en una simulación. 
RF-95 Validar folio para editar simulación El sistema deberá verificar que el folio ingresado 
corresponda al administrador que tiene la sesión iniciada antes de guardar los cambios. 
RF-96 Cancelar edición de simulación El sistema deberá permitir cancelar la edición de una 
simulación y descartar las modificaciones realizadas. 
RF-97 Cerrar ficha del cliente El sistema deberá permitir al administrador cerrar la ficha del 
cliente y regresar a la lista de clientes.
```

### Página 10 de 13

```text
RF-98 Recalcular simulación modificada El sistema deberá recalcular la mensualidad, los intereses 
totales, el total por pagar y la tabla de amortización cuando el administrador guarde modificaciones  en 
las condiciones de una simulación. 
 
 
 
 
 
Módulo Administradores 
 
 
 
RF-99 Consultar administradores registrados El sistema deberá permitir al administrador consultar 
las cuentas que tienen asignado el rol de Administrador. 
RF-100 Buscar administradores El sistema deberá permitir localizar administradores mediante una 
barra de búsqueda. 
RF-101 Mostrar tarjetas de administradores El sistema deberá presentar cada administrador mediante 
una tarjeta que incluya su fotografía de perfil, correo electrónico y una  etiqueta que identifique su rol 
como Administrador. 
RF-102 Acceder a la información de un administrador El sistema deberá permitir iniciar el proceso 
de consulta de la información completa de un administrador mediante la opción «Ver información». 
 
RF-103 Solicitar folio para consultar administrador El sistema deberá solicitar el folio del administrador 
que tiene la sesión iniciada antes de permitir consultar la ficha de otro administrador. 
RF-104 Validar folio para consultar administrador El sistema deberá  verificar que el folio ingresado 
corresponda al administrador que tiene la sesión iniciada y rechazar cualquier otro folio. 
RF-105 Consultar ficha de administrador El sistema deberá mostrar la fotografía de perfil, nombre, 
correo electrónico y fecha de nacimiento del administrador seleccionado, sin revelar su contraseña ni su 
folio administrativo. 
 
RF-106 Acceder a la edición de administrador El sistema deberá permitir abrir la ventana de 
edición de los datos del administrador seleccionado. 
RF-107 Modificar datos de administrador El sistema deberá permitir modificar los datos personales 
del administrador, incluyendo fotografía de perfil, nombre, correo electrónico, fecha de nacimiento y 
contraseña, sin permitir modificar su folio administrativo. 
RF-108 Solicitar autorización para editar administrador El sistema deberá solicitar el folio del 
administrador que tiene la sesión iniciada antes de guardar los cambios realizados. 
RF-109 Validar folio para editar administrador El sistema deberá verificar que  el folio ingresado 
corresponda al administrador que tiene la sesión iniciada antes de autorizar la modificación. 
RF-110 Guardar cambios de administrador El sistema deberá guardar las modificaciones realizadas en 
los datos del administrador cuando la autorización sea válida y los datos cumplan las validaciones 
correspondientes. 
 
RF-111 Solicitar autorización para eliminar administrador El sistema deberá solicitar el folio del 
administrador que tiene la sesión iniciada antes de ejecutar la eliminación de otra cuenta administrativa. 
RF-112 Validar folio para eliminar administrador El sistema deberá verificar que el folio ingresado 
corresponda al administrador que tiene la sesión iniciada antes de autorizar la eliminación.
```

### Página 11 de 13

```text
RF-113 Eliminar cuenta de administrador El sistema deberá permitir eliminar una cuenta 
administrativa cuando la autorización sea válida y se cumplan las restricciones de eliminación 
establecidas. 
RF-114 Restringir eliminación de cuenta propia El sistema deberá impedir que el administrador 
elimine la cuenta correspondiente a su sesión activa. 
RF-115 Restringir eliminación del último administrador El sistema deberá impedir la eliminación de 
una cuenta cuando esta sea la última cuenta con rol de Administrador registrada en la plataforma. 
 
RF-116 Acceder al registro de administrador El sistema deberá permitir al administrador iniciar el 
proceso de creación de una nueva cuenta con rol de Administrador mediante la opción «Agregar 
administrador». 
 
RF-117 Registrar datos del administrador El sistema  deberá permitir introducir el nombre, correo 
electrónico, fecha de nacimiento, contraseña y confirmación de contraseña para crear una nueva cuenta 
administrativa. 
RF-118 Validar datos del administrador El sistema deberá verificar que los datos introducidos 
cumplan las condiciones de formato, unicidad, edad y seguridad establecidas para las cuentas de usuario. 
RF-119 Solicitar autorización para crear administrador El sistema deberá solicitar el folio del 
administrador que tiene la sesión iniciada antes de ejecutar la creación de una nueva cuenta administrativa. 
RF-120 Validar folio para crear administrador El sistema deberá verificar que el folio ingresado 
corresponda al administrador que tiene la sesión iniciada antes de autorizar la creación de la nueva cuenta. 
RF-121 Generar folio administrativo El sistema deberá generar automáticamente un folio único de 13 
caracteres para la nueva cuenta administrativa, conforme al formato establecido en RN-37. 
RF-122 Crear cuenta de administrador El sistema deberá registrar una nueva cuenta con rol de 
Administrador cuando los datos proporcionados sean válidos y la autorización administrativa sea 
satisfactoria. 
RF-123 Proporcionar folio administrativo El sistema deberá proporcionar al nuevo administrador el 
folio generado durante la creación de su cuenta. 
RF-124 Solicitar revelación del folio propio El sistema deberá permitir al administrador iniciar la 
revelación de su propio folio mediante la opción «Revelar mi folio». 
RF-125 Autorizar revelación del folio propio El sistema deberá solicitar el folio de otro 
administrador y validar que corresponda a dicho administrador antes de revelar el folio del administrador 
solicitante, sin mostrar el folio de quien autoriza. 
 
 
 
Módulo Apelaciones del cliente 
 
RF-126 Consultar información de deshabilitación El sistema deberá mostrar al cliente deshabilitado su 
nombre, el motivo de desactivación de la cuenta y la fecha en que fue deshabilitada. 
RF-127 Presentar apelación de reactivación El sistema deberá permitir al cliente deshabilitado introducir 
una explicación en el campo «Explica por qué solicitas la reactivación» y enviar la solicitud de apelación. 
RF-128 Consultar estado de apelación El sistema deberá permitir al cliente consultar el estado y el 
resultado de las apelaciones que haya presentado. 
 
Módulo Apelaciones 
 
RF-129 Consultar cantidades de apelaciones El sistema deberá mostrar la cantidad de apelaciones 
que se encuentran en estado Pendiente, Aprobada y Rechazada.
```

### Página 12 de 13

```text
RF-130 Consultar apelaciones registradas El sistema deberá permitir al administrador consultar 
las solicitudes de apelación registradas en la plataforma. 
RF-131 Buscar apelaciones por nombre El sistema deberá permitir localizar apelaciones mediante el 
nombre del cliente que presentó la solicitud. 
RF-132 Filtrar apelaciones por estado El sistema deberá permitir filtrar las solicitudes mediante las 
opciones Todas, Pendientes, Aprobadas y Rechazadas. 
RF-133 Mostrar información de apelación El sistema deberá presentar en cada ficha el nombre del 
cliente, correo electrónico, fecha de solicitud, motivo, estado de la apelación y mensaje proporcionado 
por el cliente. 
RF-134 Acceder al rechazo de apelación El sistema deberá permitir al administrador iniciar el 
proceso de rechazo de una apelación pendiente. 
RF-135 Registrar motivo de rechazo El sistema deberá permitir al administrador introducir el 
motivo por el cual se rechaza una apelación. 
RF-136 Solicitar autorización para rechazar apelación El sistema deberá solicitar el folio del 
administrador que tiene la sesión iniciada antes de confirmar el rechazo. 
RF-137 Validar folio para rechazar apelación El sistema deberá verificar que el folio ingresado 
corresponda al administrador que tiene la sesión iniciada. 
RF-138 Rechazar apelación El sistema deberá cambiar el estado de la apelación a Rechazada 
cuando el motivo y la autorización administrativa sean válidos. 
RF-139 Solicitar autorización para aprobar apelación El sistema deberá solicitar el folio del 
administrador que tiene la sesión iniciada antes de confirmar la aprobación de una apelación. 
RF-140 Validar folio para aprobar apelación El sistema deberá verificar que el folio ingresado 
corresponda al administrador que tiene la sesión iniciada. 
RF-141 Aprobar apelación y reactivar cuentaEl sistema deberá cambiar el estado de la apelación 
a Aprobada y reactivar la cuenta del cliente, asignándole el estado Activo, cuando la autorización sea 
válida. 
RF-142 Actualizar estado de apelación El sistema deberá actualizar el estado mostrado de la 
solicitud después de que sea aprobada o rechazada. 
 
 
 
Módulo Actividad Administrativa 
 
RF-143 Consultar actividad administrativa El sistema deberá permitir al administrador consultar 
el historial de acciones realizadas dentro de la plataforma por las cuentas administrativas. 
RF-144 Mostrar indicadores de actividad El sistema deberá mostrar la cantidad de acciones 
correspondientes a los filtros aplicados, el total de participaciones administrativas y las apelaciones 
resueltas. 
RF-145 Buscar actividad administrativa El sistema deberá permitir localizar registros 
mediante el nombre del administrador, nombre del cliente, nombre de la simulación o descripción de la 
acción. 
RF-146 Filtrar actividad por administrador El sistema deberá permitir filtrar los registros de 
actividad seleccionando uno de los administradores registrados en la plataforma. 
RF-147 Filtrar actividad por acción El sistema deberá permitir filtrar los registros según el tipo 
de acción administrativa realizada, incluyendo las operaciones disponibles en el sistema. 
RF-148 Filtrar actividad por periodo El sistema deberá permitir delimitar los registros de 
actividad mediante una fecha inicial y una fecha final. 
RF-149 Limpiar filtros de actividad El sistema deberá permitir restablecer los criterios de 
búsqueda y filtrado aplicados para consultar nuevamente los registros sin dichos criterios.
```

### Página 13 de 13

```text
RF-150 Mostrar registros de actividad El sistema deberá presentar los resultados en una 
tabla con las columnas fecha y hora, responsable, acción, elemento afectado y detalle. 
RF-151 Consultar detalle de actividad El sistema deberá permitir abrir el detalle de un 
registro de actividad mediante la opción «Ver detalle». 
RF-152 Mostrar información detallada de actividad El sistema deberá presentar la acción 
ejecutada, el administrador responsable, la fecha y hora, el motivo o descripción y, cuando corresponda, 
los valores anteriores y posteriores a la modificación. 
RF-153 Cerrar detalle de actividad El sistema deberá permitir cerrar la ventana de detalle y 
regresar a la consulta de actividad administrativa. 
RF-154 Mostrar cantidad de resultados El sistema deberá mostrar el número total de 
registros que coincidan con los criterios de búsqueda y filtrado aplicados. 
RF-155 Paginar resultados de actividad El sistema deberá dividir los resultados en páginas y 
permitir consultar la cantidad de páginas disponibles. 
RF-156 Navegar entre páginas de actividad El sistema deberá permitir desplazarse entre las 
páginas de resultados mediante los controles «Anterior» y «Siguiente». 
 
Módulo Navegación global 
RF-157 Cerrar sesión El sistema deberá permitir al usuario cerrar su sesión mediante el botón 
correspondiente disponible en el navegador global. 
 
RF- 158 Navegación El sistema deberá permitir navehar al usuario entre las diferentes ventanas mediante 
un panel global. 
 
3. Requerimientos no funcionales (RNF) 
 
RNF-01 Seguridad en operaciones de base de datos El sistema deberá utilizar mecanismos seguros para 
procesar los datos proporcionados por los usuarios en las operaciones con la base de datos, evitando la 
ejecución de instrucciones SQL no autorizadas.
```

## Acuerdo adicional: limpieza completa del prototipo

La integración incluye retirar todas las referencias visibles y comportamientos provisionales, incluso mensajes pequeños, tooltips, diálogos y PDF. Aplicar el alcance y los criterios de la sección «Retirada completa del prototipo al integrar el backend» de [01_FRONTEND.md](01_FRONTEND.md). Conservar los avisos informativos propios del simulador y el diseño aprobado. No declarar completada una función únicamente por quitar sus leyendas temporales.

### Avance 25/11/2026: conexión local
ConexionBD y Qt SQL ya abren MySQL y consultan usuario al arrancar.
QMYSQL compilado y prueba --comprobar-bd satisfactoria (salida 0).
Detalles de configuración y dependencias locales en 03. Los módulos de interfaz
siguen usando datos temporales; autenticación y persistencia funcional pendientes.


## Entrega de acceso — 26/11/2026
Implementado el registro, login y cierre de sesión. El administrador aporta correo,
contraseña y después su propio folio. Cancelar o fallar no establece sesión ni
actualiza último acceso. Cliente suspendido consulta únicamente su motivo, fecha
y apelaciones existentes; el envío/resolución se implementará con administración.

Las 12 cuentas iniciales se consideran con términos aceptados por instrucción
expresa del propietario. Los cinco administradores usan las claves que proporcionó;
los siete clientes usan las claves propuestas y aceptadas en el chat. Solo se
incluyen hashes con sal en los INSERT. El propietario ejecutará el SQL completo
tras eliminar su base anterior; el programa no la borra ni reinstala al arrancar.
No confundir esta entrega con la integración del simulador o de la gestión.

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

## Simulador implementado — 26/11/2026
El alcance aprobado de esta etapa es calcular, mostrar y guardar. Motor y servicio en
Simulador.h/.cpp; interfaz en PanelPrincipal.cpp; pruebas en pruebas/PruebasSimulador.cpp.
No se modificó ni reinstaló el esquema. Historial, PDF y comparador permanecen para
la siguiente etapa: sus muestras no representan todavía las simulaciones del usuario.

Nota del entorno local: Windows impidió sobrescribir el ejecutable anterior de la
carpeta Debug. Se configuró CMAKE_RUNTIME_OUTPUT_DIRECTORY en su caché local para
usar build/Desktop_Qt_6_12_0_MinGW_64_bit_Debug/bin, donde compiló correctamente.
Qt Creator utiliza el target CalculadorPrestamos: volver a ejecutar CMake para
refrescar la ruta antes de Ctrl+R. No se cambió esta ruta en el CMakeLists compartido.
