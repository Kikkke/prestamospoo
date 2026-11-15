# Próxima etapa: MySQL y Workbench

Acuerdo del 14/11/2026: los datos de muestra actuales se trasladarán a la base de datos MySQL, administrada mediante MySQL Workbench. No iniciar esta migración hasta que el usuario solicite trabajar en ella.

- Conservar los cinco administradores actuales: Enrique, Pedro, Uziel, Alexander y Jared, con sus nombres, correos, fechas de nacimiento y folios exactos definidos en PanelAdministrador.h. No regenerar sus folios al migrar.
- Los cambios realizados durante la ejecución del prototipo son temporales; no asumir que están guardados ni que se recuperarán al reiniciar.
- Preparar datos iniciales y esquema para cuentas, fotos, simulaciones, amortización, apelaciones y actividad administrativa. Distinguir explícitamente los ejemplos de datos reales.
- El acceso actual por folio es una demostración. La integración final deberá validar credenciales primero, establecer la sesión y verificar el folio para las acciones acordadas.
- No migrar el hash temporal SHA-256 del prototipo como solución final para contraseñas. Implementar almacenamiento de contraseñas adecuado antes de habilitar autenticación real.
- Actualizar y conciliar el diagrama de clases con el frontend antes de cerrar el esquema SQL.

## Inventario complementario

Conservar también los clientes y simulaciones de muestra, amortizaciones y datos relacionados. El inventario detallado está en DATOS_MUESTRA_MIGRACION.md; distingue las relaciones actuales de las que aún deben definirse al integrar la base de datos.
