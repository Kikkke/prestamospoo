# Backend C++ — contrato de implementación y validación

Fecha: 25/11/2026. Alcance local, no API web. Referencias: [00](00_SISTEMA_Y_REQUERIMIENTOS.md), [frontend](01_FRONTEND.md), [SQL](03_BASE_DE_DATOS.md), [acuerdos](04_ACUERDOS_DE_TRABAJO.md).
Este archivo especifica trabajo pendiente. No afirmar que sus clases/servicios ya existen. Las correcciones aprobadas en 00 prevalecen sobre el texto original del PDF.

## 1. Organización de código propuesta

Mantener pantallas Qt existentes y extraer gradualmente la lógica de negocio de lambdas y headers. Separar:
- Modelos/datos: Usuario, Cliente, Administrador, Simulacion, Cuota, Deshabilitacion, Apelacion y ActividadAdministrativa. Usuario concentra datos comunes; Cliente y Administrador pueden heredar de Usuario para cumplir el enfoque POO. No inventar atributos exclusivos de Cliente: su identidad y sus relaciones ya lo distinguen.
- CalculadoraPrestamos: único motor de cuota fija; sin dependencia de widgets o SQL.
- Validaciones: reglas compartidas para alta y edición, usadas también fuera del formulario.
- ServicioAutenticacion/Sesion: credenciales, rol, estado, último acceso y autorización.
- Servicios de usuarios, simulaciones y administración: coordinan permisos, validación y transacciones.
- Repositorios Qt SQL: consultas preparadas. ConexionBD/configuración local no debe almacenar credenciales de clientes.
- ExportadorPDF o función extraída: recibe los mismos datos usados por la vista.

No es obligatorio crear una clase por cada nombre anterior. Elegir una organización clara para explicar en clase, sin frameworks adicionales, arquitectura distribuida ni abstracciones innecesarias. Métodos propuestos: registrarCliente, iniciarSesion, validarFolioSesion, calcular, guardarSimulacion, listarHistorial, eliminarSimulacion, editarSimulacion, resolverApelacion, actualizarPerfil y guardarFoto. Cada operación devuelve éxito/datos o error comprensible, nunca éxito tras fallar SQL.

### Guía para adaptar el diagrama de clases

Es una propuesta para el diagrama futuro, no clases ya implementadas con estos nombres. Los modelos no deben ejecutar SQL por cada getter ni encargarse de dibujar ventanas.

| Clase / componente | Datos propios principales | Responsabilidad o métodos propuestos |
|---|---|---|
| Usuario, base | id, nombre, apellidoPaterno, apellidoMaterno, correo, nacimiento, fotoRuta, creadoEn, ultimoAcceso | nombreCompleto(), nombresParaSaludo(); los datos de contraseña se reservan al servicio/repositorio, nunca al modelo que pinta tarjetas |
| Cliente : Usuario | Su identidad de cliente; relaciones con simulaciones y suspensiones | Identificar al propietario; estado calculado a partir de acceso/suspensión, no atributo fijo duplicado |
| Administrador : Usuario | folio inmutable, eliminadoEn | Identificar capacidades del rol; no getter público de folio usado por todas las vistas |
| Simulacion | id, clienteId, nombre, monto, tasaAnual, meses, moneda, fechas, cuotas | Representar condiciones/resultados y totales derivados |
| Cuota | numero, interes, abonoCapital; saldos/pago en vista derivada | Representar un periodo, no recibir un pago real |
| CalculadoraPrestamos | Sin sesión ni widgets | validarCondiciones(), calcular(), generarAmortizacion() |
| Historial / servicio de simulaciones | Repositorio + sesión | guardar(), buscar(), consultarDetalle(), eliminarLogicamente() |
| Comparador | Dos simulaciones vigentes diferentes | comparar(), detectarEmpates(); no propietario independiente de los préstamos |
| Deshabilitacion / Apelacion | IDs, fechas, motivo/mensaje, estado de resolución | Representar episodio y solicitud; servicio administrativo aplica cambios autorizados |
| ActividadAdministrativa | Actor, autorizador opcional, acción, objetivo, antes/después, fecha | Evento histórico sin métodos de edición/borrado para el usuario |
| ExportadorPDF | Datos de simulación y amortización consultados | exportar(); no requiere tabla Constancia ni PDF guardado en BD |

Herencia: flecha con triángulo hueco desde Cliente/Administrador apuntando a Usuario. Asociación: Cliente tiene muchas Simulacion. Composición: Simulacion contiene sus Cuota (rombo lleno junto a Simulacion); borrar lógicamente no destruye físicamente esos objetos persistidos. Dependencia: Calculadora/Comparador/Exportador usan simulaciones y cuotas, no heredan de ellas. Historial es una vista/servicio del conjunto del propietario, no otra copia de todos sus registros.
En UML, `+` público es parte de la interfaz invocable; `-` privado queda dentro de la clase; `#` protegido puede usarse por la clase y sus derivadas. Preferir datos privados y métodos públicos controlados; no hacer todos los atributos protegidos solo por existir herencia. Si se destruyen derivados a través de Usuario, diseñar destructor virtual. No inventar una jerarquía de tipos de interés para justificar polimorfismo.

## 2. Sesión y autorización transversal

Sesión mínima: usuarioId, rol, autenticación completada, modo operativo o restringido por suspensión. No usar nombre, correo, posición de una lista o folio como ID interno estable. No guardar contraseña ni folio de sesión en texto para compararlos eternamente.

- Antes de cada operación de negocio, comprobar sesión, rol y estado vigente. Ocultar un botón no es autorización.
- Cliente: consultar, comparar, exportar y eliminar solo registros cuyo cliente_id coincida con usuarioId y eliminada_en sea NULL.
- Administrador no eliminado: consultar clientes y gestionar sus simulaciones/cuentas; gestionar administradores; resolver apelaciones y ver toda la actividad.
- Revalidar permisos/estado antes de escribir; una ventana abierta no autoriza para siempre.
- Folio de sesión para operaciones administrativas modificadoras y consulta de fichas de administradores. Cancelar/folio inválido no produce cambios ni evento de éxito.
- Revelar propio folio exige folio de un administrador distinto, no eliminado. El autorizador no puede usar esta operación para consultar folio ajeno. Si solo queda un administrador, rechazar con explicación; no crear bypass.
- La foto administrativa se cambia solo por el propietario activo; la foto de un cliente solo por ese cliente.
- Al salir, invalidar sesión y datos visibles de la cuenta anterior; no reutilizar modelos del usuario previo. Reabrir o volver atrás no recupera autorización.
- Uso local significa una aplicación/BD en el equipo; no da protección contra quien controla Windows o accede como root. El folio es una comprobación adicional acordada, no autenticación multifactor independiente.

## 3. Contraseñas: acuerdo definitivo

**PBKDF2-HMAC-SHA-256 mediante QPasswordDigestor::deriveKeyPbkdf2**, incluido en Qt Network. No requiere internet ni una biblioteca externa de hashing. Agregar Network en CMake junto a Widgets y Sql al implementar. No usar PBKDF1, SHA-256 simple ni cifrado reversible para contraseñas. El SHA-256 de las ediciones temporales actuales debe reemplazarse.

Formato propuesto autocontenido en password_hash (VARCHAR(255)):
`pbkdf2-sha256$600000$SAL_BASE64$RESULTADO_BASE64`.
Parámetros de partida: sal aleatoria criptográfica de 16 bytes, 600000 iteraciones, salida 32 bytes. Generar sal nueva en cada alta/cambio, incluso si dos usuarios usan la misma contraseña. QRandomGenerator::system permite obtener bytes del generador del sistema; no usar rand, fecha, nombre ni folio como sal.

Registro/cambio: validar contraseña y confirmación → generar sal → derivar con Qt → serializar algoritmo/parámetros/sal/hash → guardar. Login: parsear con límites, validar formato/longitudes, derivar con la sal y parámetros almacenados y comparar el resultado. Contraseña nunca se recupera; solo se verifica. No eliminar espacios ni cambiar mayúsculas de la contraseña antes de derivarla.
Ejemplo orientativo de llamada: `QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256, clave.toUtf8(), sal, 600000, 32)`; no es todo el flujo de autenticación.
Hash NULL, vacío o formato inválido: cuenta sin credencial válida, jamás aceptar entrada vacía ni asumir clave predeterminada. No convertir un hash SHA-256 viejo a PBKDF2 como si fuera la contraseña; establecer una clave válida nuevamente.
Comparación del resultado: utilizar una rutina revisada que no revele el primer byte diferente; no improvisar algoritmos criptográficos. Evitar registrar contraseña, hash o sal junto a credenciales en logs. Medir coste para que no congele la interfaz; si se usa worker, Qt SQL mantiene conexiones por hilo, no compartir QSqlDatabase entre ellos.
No debilitar iteraciones solo por parecer muchas: Qt hace el trabajo internamente. Si se cambia parámetro, documentar y conservarlo en cada hash para verificar los existentes.
Fuentes técnicas: [Qt QPasswordDigestor](https://doc.qt.io/qt-6/qpassworddigestor.html) y [OWASP Password Storage](https://cheatsheetseries.owasp.org/cheatsheets/Password_Storage_Cheat_Sheet.html). Esta elección reemplaza la propuesta anterior de Argon2id por simplicidad de integración y explicación académica.

## 4. Validaciones comunes de cuentas

| Dato | Regla y tratamiento |
|---|---|
| Nombre(s)/apellidos | Letras Unicode y espacios; admitir acentos y ñ. Rechazar números, guiones y signos conforme RN-08. Recortar espacios externos y normalizar espacios internos; no convertir a ASCII el nombre visible. |
| Longitudes | No sobrepasar SQL: nombres 120, cada apellido 80, correo 254. El PDF no fija mínimo 3 para nombres: no generalizar la validación temporal de EditarPerfil. |
| Campos obligatorios | Nombre y apellido paterno deben existir. El PDF no aclara casos sin apellido materno: propuesta permitirlo vacío/NULL para personas que no lo tienen; confirmar antes de imponer obligatoriedad a todos. Las semillas actuales tienen apellidos completos. |
| Correo | Formato válido, sin espacios exteriores, no duplicado entre clientes y administradores. Política propuesta comparar sin distinguir mayúsculas, también al editar excluyendo el ID propio. Una regex no verifica propiedad o entrega; no agregar verificación por correo no solicitada. |
| Nacimiento | Fecha válida, no futura, al menos 18 años cumplidos usando fecha local; aplicar en alta y edición. Documentar y probar cumpleaños y 29 de febrero. No guardar edad. |
| Contraseña | 8–15 caracteres, al menos mayúscula, minúscula, número y carácter especial (RN-10/RF-19). No recortar ni alterar. Confirmación exacta. La política de qué espacios/signos cuentan como especial no está definida: propuesta especial no alfanumérico y no espacio, y rechazar saltos de línea/caracteres de control. |
| Edición sin clave | Ambos campos de clave vacíos conserva hash actual. Uno lleno o distintos → error. Nunca borrar el hash por dejar campos vacíos. |
| Unicidad | Backend comprueba correo/folio; no confiar en índices UNIQUE porque usuario pidió SQL sin ellos. Resolver dentro de operación serializada/transacción en la aplicación local. No prometer unicidad frente a escritores SQL externos. |

Normalización de búsqueda es distinta a almacenamiento: buscar «lopez» puede encontrar López, pero no quitar acentos del dato guardado. Definir de forma consistente conteo de caracteres Unicode; los límites son caracteres, no bytes UTF-8.

## 5. Login, registro y términos (RF-01–24)

### Login
1. Leer correo y clave; si faltan, mostrar error sin ir al panel.
2. Buscar usuario con parámetros; validar hash. Mensaje uniforme «Correo o contraseña incorrectos» para credenciales inválidas.
3. Determinar exactamente un rol. Rechazar rol ambiguo/inexistente o administrador eliminado.
4. Cliente: si suspensión abierta, sesión limitada a sus apelaciones; no usar clientes[0]. Si habilitado, establecer sesión operativa y actualizar ultimo_acceso.
5. Administrador: después de contraseña válida, pedir SU folio; uno de otro administrador no sirve aunque sea válido. Cancelar mantiene login. Solo al superar ambos establecer sesión y último acceso.
6. Cargar foto/nombres/datos por ID. Credenciales de una persona no deben abrir modelos estáticos de otra.

Último acceso representa sesión operativa exitosa; propuesta no actualizarlo por un intento denegado ni por una sesión restringida de apelaciones, para no convertir actividad restringida en reactivación. Esta precisión completa RF-11 y debe quedar consistente en pruebas.

### Registro público
Validar todos los campos anteriores y aceptación. Crear usuario + cliente en una transacción. Guardar PBKDF2, versión actual de términos y fecha/hora real de aceptación. La confirmación de contraseña no se almacena. No crear Administrador desde este flujo.
Tras éxito, mantener la navegación aprobada al inicio con sesión real (alta fallida no emite inicioSolicitado). Actualizar ultimo_acceso si se inicia sesión automáticamente. Repetir clic mientras se guarda no crea dos cuentas. Errores preservan los campos no sensibles para corregirlos.

### Aceptación
Fuente de texto actual TerminosRegistro.h, v1 de 17/11/2026. Una constante compartida debe identificar la misma versión en UI y BD. Abrir modal no cuenta como aceptación; llegar al final habilita retorno, pero la casilla la marca el usuario. Pulsarla antes debe advertir sin marcar y también funcionar con barra espaciadora. Crear cuenta comprueba flag de recorrido + casilla, además de controles visibles.
Al cerrar antes del final no habilitar. Si el contenido cabe entero, está visible completo y se puede habilitar. No usar temporizador que marque aceptación sin acción. Reiniciar flags para nueva alta/cambio de versión. El almacenamiento solo conserva la última versión y fecha: no hay tabla histórica de aceptaciones.
Las semillas tienen aceptación NULL; no atribuir una fecha inventada. Propuesta pendiente: al iniciar por primera vez una cuenta precargada, pedir aceptación vigente antes de habilitar módulos, sin obligar a recrear la cuenta.

Contenido aprobado que debe preservarse al integrar: naturaleza informativa; cuenta personal y mayoría de edad; datos veraces; prohibición de contenido obsceno, amenazas, discriminación, suplantación y usos ilícitos; acceso administrativo a datos/simulaciones; edición/eliminación administrativa justificada y registrada; suspensión y apelación; inactividad sin bloqueo automático; privacidad; PDF sin carácter contractual; actividad accesible a todos los administradores y actualización de términos. No agregar un chat de soporte. Los PDF ya descargados no desaparecen al eliminar lógicamente el registro. El texto existente menciona un aviso de privacidad independiente: dicho aviso no está implementado ni redactado aquí y no debe darse por cubierto con la casilla de términos.

## 6. Simulador y motor (RF-25–38, RN-12–23)

### Entradas definitivas
- Nombre 1–50 caracteres tras trim; letras Unicode/acento, números, espacio y `_`. Sin saltos de línea ni solo espacios. Se permiten nombres repetidos: identificar siempre por ID.
- Monto numérico finito de 1.00 a 10000000.00 MXN, hasta dos decimales.
- Tasa nominal anual de 0.00 a 200.00 %, hasta dos decimales. 12 significa 12 %, no 0.12. No CAT ni tasa efectiva.
- Plazo entero entre 1 y 480 meses. No aceptar decimales redondeándolos silenciosamente.
- Validar también llamadas directas al servicio, no solo spinboxes. Rechazar NaN/infinito/desbordamientos, campos vacíos y formato ambiguo. Definir parseo español mexicano y evitar convertir entrada inválida a cero.
- Límites actuales mayores en PanelPrincipal y editor administrativo deben cambiar conjuntamente. SQL DECIMAL más amplio no autoriza saltarse la regla.

### Cálculo único
P = monto; n = meses; r = tasa anual en porcentaje / 1200.
Si r = 0: cuota teórica = P / n.
Si r > 0: cuota teórica = P*r / (1-(1+r)^(-n)).
Para tasas pequeñas utilizar una expresión numéricamente estable o probar cancelación con long double; no aplicar la rama de tasa cero a tasas positivas arbitrariamente.
Redondear cuota regular a centavos. Cada periodo: interés = redondeo(saldo inicial*r); capital = cuota regular - interés, limitado al saldo restante; pago = capital + interés. En el último periodo, capital = saldo restante y pago = interés + capital, para cerrar exactamente.
Guardar importes definitivos en centavos enteros o una representación decimal acordada; no acumular dinero en double sin una política de redondeo central. El código existente usa std::round, útil como referencia, no prueba de robustez en todo el dominio.
Cuota guarda interés y capital. Reconstruir saldo inicial desde P menos capital anterior, saldo final y pago; obtener totales sumando. No almacenar otras copias para cada pantalla. Mismo resultado para simulador, edición admin, historial, comparador y PDF.
Invariantes: n cuotas consecutivas; pago = interés+capital; saldo final = inicial-capital; siguiente inicial = anterior final; sum(capital)=P; último saldo=0; sum(pagos)=P+sum(intereses). No sumar saldos como totales.
Para montos diminutos/plazos largos (p.ej. 1 MXN/480, tasa 0), el redondeo puede dar cuotas regulares 0 y liquidación final distinta. No dividir por cero ni rechazar silenciosamente un extremo permitido: acordar y probar esa política antes de entregar; explicar ajuste final. También verificar que redondeo no produzca capital negativo para tasa alta/plazo largo.

### Calcular y guardar
Calcular valida y construye un resultado asociado a una copia exacta de entradas. Cambiar nombre/monto/tasa/plazo invalida resultado para guardar; mostrar «Vuelve a calcular antes de guardar». Calcular otra vez actualiza resumen y todas las filas, no solo etiquetas.
Guardar requiere cliente habilitado y resultado válido actualizado. Transacción: insertar simulacion con propietario de sesión, condiciones y timestamps reales; insertar todas las cuotas; validar invariantes; commit. Fallo → rollback, no simulaciones sin cuotas ni duplicados por doble clic. Después actualizar historial y selectores del comparador desde repositorio.
Los valores fijos y botones Próximamente se retiran solo al conectar este flujo.

## 7. Historial, detalle, PDF y comparador (RF-39–60)

Historial: cargar solo propietario, eliminada_en NULL. Buscar nombre sin diferenciar acentos/mayúsculas en UI o SQL según política; orden numérico real, conteo coherente. Tabla/acciones transportan ID estable, no índice de fila que cambie al ordenar.
Detalle: volver a comprobar propiedad/estado y consultar condiciones + cuotas persistidas. No generar nuevas cuotas cada vez ocultando inconsistencias guardadas. Mostrar fecha de guardado real y fecha de emisión separadas.
PDF: diálogo de ruta, cancelar sin archivo; validar escritura, extensión .pdf, error de permisos/espacio y fallo parcial; no mostrar éxito sin terminar. Multipágina con cabecera y número; tabla completa hasta 480 periodos, totales solo al final y no sumar saldos. No limitar pagos a una página. Mantener mismo resultado de centavos. No necesita tabla PDF ni BLOB SQL.
Eliminar cliente: confirmar nombre, validar propietario, hacer UPDATE eliminada_en + actualizada_en. Conservar cuotas; ocultar en todas las listas, búsqueda y comparador. Repetir petición sobre eliminado no produce otro borrado. Ventanas de detalle ya abiertas deben detectar que fue eliminado o cerrarse; no exportar desde un acceso normal obsoleto. No papelera/restauración.

Comparador lee exactamente la fuente del historial. Opciones por ID distinto (no por nombre distinto), con al menos dos registros vivos. Nombre escrito es búsqueda, no una nueva simulación: seleccionar coincidencia real; texto sin selección no habilita comparación. Si hay nombres duplicados, distinguir con monto/plazo/ID o fecha en lista, sin exponer datos ajenos.
Conforme RF-52, informar cuando no hay coincidencias o el texto contiene caracteres no permitidos por RN-12; no convertir un texto arbitrario en una selección válida ni generar un préstamo nuevo a partir de una búsqueda.
Comparar al seleccionar ambas opciones; no botón adicional. Filas: monto, tasa, meses, pago regular, intereses y total. Calcular menor mensualidad, menor total y menor plazo separadamente; empates exactos según centavos, sin ganador arbitrario. Si todos iguales, mensaje explícito. Diferentes montos generan advertencia de contexto, no prohibición nueva. Al borrar/editar un registro recargar opciones y resultados, sin usar catálogo precargado.

## 8. Mi cuenta y fotos (RF-61–76)

Leer perfil por sesión. Actualización parcial conserva campos no cambiados. Aplicar mismas validaciones de alta salvo contraseña vacía conservada y unicidad de correo excluyendo ID propio. Actualizar nombres visibles, saludo solo Nombre(s), correo y fecha en todas las vistas relevantes. No cambiar usuarioId ni rol desde perfil. Cancelar no modifica modelo ni archivos.

Foto: seleccionar local con QImageReader, comprobar que es decodificable, formatos soportados PNG/JPG/JPEG/WEBP/BMP, dimensión actual máxima 12000 por lado. Reducir original de trabajo a 2048, recorte cuadrado final 512; conservar el algoritmo de preview del frontend. No confiar solo en extensión; no aceptar rutas como SQL sin verificar propiedad. Propuesta adicional pendiente: limitar archivo a 10 MiB y establecer máximo de píxeles antes de decodificar.
Guardar únicamente al confirmar; restaurar default implica foto_ruta NULL tras guardar. Usar ruta gestionada bajo datos de la aplicación (QStandardPaths::AppDataLocation), nunca depender de Descargas o ruta original. ID/nombre generado para archivo, no texto introducido. Escribir nuevo archivo temporal y validar antes de actualizar BD; si falla SQL retirar el nuevo archivo, si commit retirar el anterior de forma controlada. No borrar un archivo fuera del directorio de fotos.
No hay transacción única SQL+filesystem: prever compensación. Foto circular es máscara de visualización, guardar PNG cuadrado. Con esquema actual se conserva recorte final, no original ni posiciones editables; RF-70 se satisface en el resultado. Reabrir editor puede partir del recorte final. Conservar ajustes exactos/original requeriría ampliar esquema: no implementarlo sin necesidad.

## 9. Clientes y ficha administrativa (RF-77–98)

Listar clientes con foto, nombre completo, correo, nacimiento, último acceso, estado y count de simulaciones NO eliminadas. Buscar por nombre/correo; ordenar A–Z/Z–A, último acceso reciente/antiguo y cantidad asc/desc. Estado Todos/Activo/Inactivo/Deshabilitado; simulaciones Todas/Con/Sin. Limpiar restablece búsqueda vacía, todos y A–Z. Orden estable con ID para desempatar; fecha NULL como «Sin acceso», propuesta ubicarla al final del orden de acceso.
Estado: suspensión abierta prevalece. Si no la hay, ultimo_acceso en últimos seis meses inclusive → Activo; anterior → Inactivo. Usuario sin acceso: propuesta Inactivo con «Sin acceso». No calcular seis meses como 180 días; usar meses calendario y regla uniforme.
Ficha muestra datos y simulaciones por ID; admin no puede cambiar foto del cliente ni eliminar su cuenta.
Deshabilitar: motivo predefinido Inactividad/Uso inapropiado; confirmar folio sesión; comprobar que no exista episodio abierto; transacción insertar episodio + actividad. No inventar bloqueo automático por seis meses: solo clasificación hasta decisión administrativa. Cuenta ya deshabilitada → mensaje claro/acción inhabilitada.
Editar simulación: mismas reglas 1–50/monto/tasa/plazo; folio sesión; consultar anterior, recalcular motor único, actualizar condiciones, reemplazar cuotas y registrar antes/después dentro de una transacción. No guardar cuotas viejas con condiciones nuevas. Antes/después incluye condiciones y resultados relevantes, sin credenciales. Conservar simulacion.id y cliente_id.
Eliminar simulación: folio propio + confirmación, UPDATE eliminada_en y evento en transacción. No DELETE físico. No registrar la operación si cancela o falla. Puede operar sobre cuentas deshabilitadas en su función administrativa, sin habilitarlas implícitamente.
Moderación: las condiciones de uso permiten revisar contenido ofensivo/ilícito. No crear un filtro automático arbitrario de palabras ni inferir ilegalidad por una palabra. El admin toma la decisión y queda motivo/descripcion. Incorporar campo de motivo cuando se necesite justificar; no confundirlo con RN-12 de caracteres permitidos.

## 10. Administradores (RF-99–125)

Listados excluyen eliminado_en no NULL; mostrar sesión propia y rol igual para todos. Buscar nombre/correo, IDs estables, sin folios en tarjetas.
Consultar ficha propia o ajena pide folio activo, no el del objetivo. Muestra datos, contraseña protegida y folio reservado. Prohibir revelar otro folio mediante cambio de ID.
Alta: nombre(s), dos apellidos, correo, nacimiento, clave y confirmación. Validadores comunes + autorización del actor. Transacción usuario + administrador + actividad. Generar folio solo una vez y entregarlo después de commit; copia seleccionable, no publicación general. Dar a conocer datos de acceso no significa guardar contraseña legible.
Algoritmo folio: normalizar Nombre(s) a letras A–Z (quitar diacríticos, espacios para prefijo), tomar primeras tres; + fecha ddMMyy + dos letras A–Z aleatorias + dos dígitos 0–9, incluidos ceros iniciales. Longitud 13; patrón ^[A-Z]{3}[0-9]{6}[A-Z]{2}[0-9]{2}$. Verificar folio completo, reintentar sufijo con límite/error claro si no consigue uno. Un prefijo repetido NO es colisión. RN-37 corregida; no desplazar prefijos.
Nombre normalizado de menos de tres letras no genera prefijo: propuesta pendiente de confirmar, mostrar error al alta administrativa. Nunca rellenarlo inventando letras. No aplicar esta limitación automáticamente a un cliente.
Edición propia o ajena: folio de la sesión para cambios. Mismas validaciones; confirmar nueva contraseña, ambos vacíos conserva. Folio nunca editable ni regenerado; foto exclusivamente propia. Registrar cambio de contraseña como hecho, SIN valores de clave/hash; folio tampoco va a auditoría.
Eliminación lógica: no eliminar a sí mismo ni al último administrador no eliminado. Revalidar conteo dentro de la operación, marcar eliminado_en y registrar evento. Conservar usuario y relaciones; no permitir nuevo login/revelación/autorización con administrador eliminado.
Revelar propio folio: segundo administrador diferente y no eliminado autoriza con su folio. Actor=propietario activo, autorizador_id=tercero; registro sin ambos folios. No agregar un botón para ver folios de terceros. No guardar confirmación como permiso reutilizable para futuras acciones.
Con un único admin superviviente no hay tercero para revelar: explicar que debe conservar su folio; no inventar recuperación por correo o excepción.

## 11. Apelaciones (RF-126–142)

Cliente restringido se identifica con credenciales válidas; muestra nombre, motivo y fecha real de episodio abierto, su historial de solicitudes y respuestas. Texto claro indicando la suspensión y botón de apelar. No hay chat ni canal externo de contacto implementado.
Enviar: mensaje no vacío tras trim; propuesta límite 2000 caracteres (no fijado en PDF, confirmar). Una pendiente por cliente en total, no por pantalla; verificar episodio actual, no resuelto y propiedad. Insertar en transacción, impedir doble envío; error si otra pendiente ya existe. Tras rechazo puede enviar una nueva: no sobrescribir anterior. Estados canónicos sugeridos PENDIENTE/APROBADA/RECHAZADA, traducidos en UI.
Administración: contadores por estado, buscar nombre/correo, filtros, ordenar fechas y tarjetas con mensaje/respuesta ajustados. Solo actuar sobre pendiente y episodio abierto vigente.
Rechazo: motivo obligatorio (misma propuesta límite 2000), folio sesión; guardar estado, respuesta, resuelta_en, resuelta_por y auditoría juntos. Suspensión queda abierta.
Aprobación: folio sesión; guardar estado aprobado/resolución, cerrar episodio con reactivada_en y registrar auditoría juntos. No falsear ultimo_acceso. Cuenta recupera acceso y puede seguir Inactiva hasta login válido. Evitar dos resoluciones por doble clic o ventana obsoleta.
Cancelar motivo/folio no cambia estado. Respuesta legible para el cliente en siguiente consulta/login restringido. Refrescar contadores y fichas después de commit.

## 12. Actividad administrativa (RF-143–156)

Evento real de cada alta/edición/eliminación administrativa, suspensión, aprobación/rechazo, cambio de contraseña/foto y revelación autorizada. Según RN-51/48, propuesta registrar también consultas protegidas de fichas; no registrar cada tecla o navegación.
Actor y autorizador IDs, acción, entidad, entidad_id, descripcion y timestamp. antes/despues como JSON serializado dentro de TEXT o formato estable legible; son instantáneas históricas, no acceso al estado actual. Sin contraseñas, hashes, folios o datos confidenciales innecesarios.
Actividad debe persistir en la MISMA transacción de los cambios que describe; si el evento falla, no declarar el cambio auditado. No interfaz de edición/borrado de actividad. Todos los administradores no eliminados tienen acceso.
Un actor eliminado sigue siendo identificable por su usuario conservado. Nombres pueden consultarse por JOIN; si se requiere nombre histórico exacto, incluirlo en la instantánea del evento, no confundir con nombre actual.
Búsqueda por actor, cliente, simulación o descripción. Conservar identificadores/descripciones del objetivo aun si fue ocultado lógicamente. No truncar injustificadamente textos en detalle.
Filtros administrador, acción y fechas: desde <= hasta. Incluir todo el último día (>= inicio y < día siguiente a hasta), no hasta 00:00 del día final. Limpiar vuelve a 30 días y todos. Las métricas usan el mismo conjunto filtrado, no solo la página: acciones, administradores distintos, apelaciones resueltas. PDF «participaciones» se interpreta como administradores participantes según frontend; no confundir con total de eventos.
Paginación actual 8 filas: ORDER BY creada_en DESC,id DESC, LIMIT y OFFSET validados. Reiniciar página al cambiar filtros; ajustar si disminuyen resultados. Cero resultados muestra ceros, sin página negativa ni siguiente activo.

## 13. SQL, atomicidad y errores (RNF-01)

Usar QSqlQuery::prepare y bindValue; nunca concatenar nombre, correo, folio, mensaje o contraseña en SQL. ORDER BY solo selecciona de una lista fija de expresiones autorizadas; no enlazar directamente una cadena de usuario como identificador. Parámetros también en filtros; escapar comodines si la búsqueda se define literal.
Conexión local configurada fuera del código/versionado; errores comprensibles, sin mostrar contraseña de root ni consultas con datos sensibles. Si falla conectar, no caer silenciosamente al modo demo.
Transacciones para alta, cambio de perfil+auditoría cuando aplica, guardar/recalcular simulación+cuotas, suspensión+evento, resolución+reactivación+evento y borrados lógicos+evento. Verificar cada exec, commit y rollback. Deshabilitar botón mientras procesa para no duplicar acciones; rollback también por excepción.
El usuario pidió esquema sin UNIQUE/CHECK/DEFAULT/índices auxiliares. Por tanto comprobar duplicados en backend y serializar escrituras. Conexiones externas a la aplicación pueden saltarse esas reglas; documentar esta limitación del alcance local, no afirmar garantías que SQL no impone.

## 14. Plan de entrega y pruebas

Fase 1: conectar QMYSQL en base nueva de pruebas; validar esquema, FK e inserts; ninguna cuenta accede con hash NULL.
Fase 2: PBKDF2, validadores, bootstrap de contraseñas, registro/login/rol/folio/estado/salir; aceptar términos sin falsear datos iniciales.
Fase 3: perfiles y fotos persistentes; nombres compuestos y actualizaciones parciales.
Fase 4: motor, cálculo e invalidación, guardar, historial, PDF, comparador único.
Fase 5: servicios administrativos, episodios y apelaciones, auditoría real.
Fase 6: retirar constantes de prototipo, leyendas, catálogos paralelos y accesos demo; actualizar Acerca de; revisar UI y reiniciar aplicación para comprobar persistencia.

| Prueba | Resultado esperado |
|---|---|
| Correo repetido entre roles o edición ajena | Rechazado, sin cambios parciales |
| 17 años / cumple 18 hoy / fecha futura / fecha inválida | Solo mayoría de edad y fecha válida se admite |
| Claves 7/8/15/16, falta cada categoría, confirmación distinta | Límites PDF; campos vacíos en edición conservan hash |
| Misma clave en dos cuentas | Hash serializado diferente por sal, ambos verifican |
| Hash NULL/corrupto, contraseña errónea | Login denegado; no navegación directa |
| Folio ajeno durante login o modificación | Denegado, sin eventos de éxito |
| Términos arriba/abajo/cerrar/clic prematuro/espacio | Advertencia o habilitación correctas, nunca autocheck |
| Cambiar entradas tras calcular | Guardar bloqueado hasta recalcular |
| Monto 0.99/1/10 millones/exceso; tasa negativa/0/200/exceso; meses 0/1/480/481/fracción | Límites iguales en servicio y UI |
| 12000,0 %,3 meses | 4000 por pago; capital total 12000; interés0; saldo0 |
| 150000,15.5 %,60 meses | Referencia semilla: cuota3607.98, última3607.86, interés66478.68, total216478.68 |
| Monto pequeño, tasa alta, plazo máximo, tasa casi0 | Sin negativos/NaN; invariantes y redondeo explicado |
| Guardado falla en una cuota o auditoría | Rollback total |
| Dos clientes, ID de simulación manipulado | No acceder/comparar/eliminar/exportar registro ajeno |
| Borrado lógico | Sigue en BD y cuotas; desaparece de consultas normales incluso tras reiniciar |
| Opciones mismo ID / nombres iguales distinto ID / empate / monto distinto | Regla por ID; empate/aviso coherentes |
| PDF 1/22/23/60/480 pagos; cancelar/error escritura | Completo, paginado, sin éxito falso ni recortes |
| Suspender, una apelación, segundo envío, rechazar, nueva, aprobar | Restricciones y episodios consistentes |
| Editar foto ajena / revelar folio ajeno / eliminar sesión o último admin | Denegado |
| Registro de acciones con cambio clave/folio | No filtra valores sensibles |
| Salir y entrar como otro usuario | Ningún perfil/dato/autorización heredados |
| Inyección en búsquedas y textos SQL | Tratada como dato; sin ejecutar instrucciones |
| MySQL caído y reabrir aplicación | Error claro; no pérdida silenciosa ni retorno a muestras |

Los límites de archivos/textos propuestos y comportamiento de cuentas semilla sin términos necesitan confirmación cuando se implemente; no bloquean la documentación. No afirmar que estas pruebas ya se ejecutaron: son criterios de aceptación del backend futuro.

## Acuerdo adicional: limpieza completa del prototipo

La integración incluye retirar todas las referencias visibles y comportamientos provisionales, incluso mensajes pequeños, tooltips, diálogos y PDF. Aplicar el alcance y los criterios de la sección «Retirada completa del prototipo al integrar el backend» de [01_FRONTEND.md](01_FRONTEND.md). Conservar los avisos informativos propios del simulador y el diseño aprobado. No declarar completada una función únicamente por quitar sus leyendas temporales.
