-- Esquema simplificado para FinancionantiMex. Instalación nueva; no migra una BD existente.
-- Validaciones de negocio a cargo del backend.
CREATE DATABASE financionantimex;
USE financionantimex;
CREATE TABLE usuario (
 id INT PRIMARY KEY AUTO_INCREMENT,
 nombre VARCHAR(120),
 apellido_paterno VARCHAR(80),
 apellido_materno VARCHAR(80),
 correo VARCHAR(254),
 nacimiento DATE,
 password_hash VARCHAR(255) NULL,
 foto_ruta VARCHAR(512) NULL,
 creado_en DATETIME,
 ultimo_acceso DATETIME NULL,
 terminos_version VARCHAR(32) NULL,
 terminos_aceptados_en DATETIME NULL
);
CREATE TABLE administrador (
 usuario_id INT PRIMARY KEY,
 folio CHAR(13),
 eliminado_en DATETIME NULL,
 FOREIGN KEY(usuario_id) REFERENCES usuario(id)
);
CREATE TABLE cliente (
 usuario_id INT PRIMARY KEY,
 FOREIGN KEY(usuario_id) REFERENCES usuario(id)
);
CREATE TABLE deshabilitacion (
 id INT PRIMARY KEY AUTO_INCREMENT,
 cliente_id INT,
 motivo TEXT,
 creada_en DATETIME,
 reactivada_en DATETIME NULL,
 FOREIGN KEY(cliente_id) REFERENCES cliente(usuario_id)
);
CREATE TABLE simulacion (
 id INT PRIMARY KEY AUTO_INCREMENT,
 cliente_id INT,
 nombre VARCHAR(160),
 monto DECIMAL(15,2),
 tasa_anual DECIMAL(7,4),
 meses INT,
 moneda CHAR(3),
 creada_en DATETIME,
 actualizada_en DATETIME,
 eliminada_en DATETIME NULL,
 FOREIGN KEY(cliente_id) REFERENCES cliente(usuario_id)
);
CREATE TABLE cuota (
 simulacion_id INT,
 numero INT,
 interes DECIMAL(17,2),
 abono_capital DECIMAL(15,2),
 PRIMARY KEY(simulacion_id,numero),
 FOREIGN KEY(simulacion_id) REFERENCES simulacion(id)
);
CREATE TABLE apelacion (
 id INT PRIMARY KEY AUTO_INCREMENT,
 deshabilitacion_id INT,
 mensaje TEXT,
 estado VARCHAR(20),
 respuesta TEXT NULL,
 creada_en DATETIME,
 resuelta_en DATETIME NULL,
 resuelta_por INT NULL,
 FOREIGN KEY(deshabilitacion_id) REFERENCES deshabilitacion(id),
 FOREIGN KEY(resuelta_por) REFERENCES administrador(usuario_id)
);
CREATE TABLE actividad_administrativa (
 id INT PRIMARY KEY AUTO_INCREMENT,
 administrador_id INT,
 autorizador_id INT NULL,
 accion VARCHAR(64),
 entidad VARCHAR(32),
 entidad_id INT NULL,
 descripcion TEXT,
 antes TEXT NULL,
 despues TEXT NULL,
 creada_en DATETIME,
 FOREIGN KEY(administrador_id) REFERENCES administrador(usuario_id),
 FOREIGN KEY(autorizador_id) REFERENCES administrador(usuario_id)
);
-- Contraseñas iniciales protegidas con PBKDF2-HMAC-SHA-256 (600000 iteraciones).
-- La última aceptación se guarda en usuario. El texto de términos está en la aplicación.
-- Por acuerdo del propietario, las 12 cuentas iniciales ya aceptaron la versión 1.
-- Su aceptación inicial usa la fecha de alta de la semilla; la actividad inicia vacía.

INSERT INTO usuario(id,nombre,apellido_paterno,apellido_materno,correo,nacimiento,ultimo_acceso,creado_en,password_hash,terminos_version,terminos_aceptados_en) VALUES
(1,'Enrique','López','Garfias','lopez.garfias.enrique@gmail.com','2007-03-31',NULL,'2026-11-15 12:00:00','pbkdf2-sha256$600000$wDMGM7AqtPiuYzXhie4Eyw==$r5sRPdSP/N4u8TRvmRAutZ6aajQXkdEoo2I+bFxuBss=','1','2026-11-15 12:00:00'),
(2,'Pedro','Pescador','Velázquez','pescador.velazquez.pedro@gmail.com','2007-05-09',NULL,'2026-11-15 12:00:00','pbkdf2-sha256$600000$dpswzJ5G37JmylP/g/5LwA==$dxreyMEvRcAPYp+9RuKPRB6DFD+5gSGX+SbiweJWVvU=','1','2026-11-15 12:00:00'),
(3,'Uziel Alexander','Pimentel','Ramírez','uzielpime@gmail.com','2007-12-21',NULL,'2026-11-15 12:00:00','pbkdf2-sha256$600000$AAjeUxIkbHv1EM4oJyvtrQ==$AQyC9ly8LuQ56tSKRrvZBbCdS29Qrr3ZBLodzjR8rDQ=','1','2026-11-15 12:00:00'),
(4,'Alexander','Mendoza','Jiménez','alexxaanderjimenez@gmail.com','2005-09-22',NULL,'2026-11-15 12:00:00','pbkdf2-sha256$600000$r/Meovxm92dJGMVQ4cmQDg==$ufXkxn3m/+tvawSsoehxddjk5nngjlzbC3UIQ64TQAA=','1','2026-11-15 12:00:00'),
(5,'Christian Jared','Castillo','Hernández','tcrid67@gmail.com','2007-09-27',NULL,'2026-11-15 12:00:00','pbkdf2-sha256$600000$GTQlOdNL/Ui3Gx69zHTLFg==$2UnyIrpxW+J+BIM9cP+X7HYsuV3X5FuoV1IHH34+XfU=','1','2026-11-15 12:00:00'),
(6,'Ana','López','Martínez','ana.lopez@example.com','1995-03-10','2026-11-15 12:00:00','2026-11-15 12:00:00','pbkdf2-sha256$600000$XHowfKmn3aG0cML7+GCU8w==$QjDO4dUTsIviXdk4tGQ5wd5I655tuhLpM4oaHiyARxk=','1','2026-11-15 12:00:00'),
(7,'Luis','Pérez','Sánchez','luis.perez@example.com','1996-04-11','2026-09-15 12:00:00','2026-11-15 12:00:00','pbkdf2-sha256$600000$wTONo65xtncNpuPCIhoOGg==$IjGPAH2GzxwCyVLNXF4plWAx7Euu/YaTYCfIaT5cxFE=','1','2026-11-15 12:00:00'),
(8,'María','Torres','Flores','maria.torres@example.com','1997-05-12','2026-07-15 12:00:00','2026-11-15 12:00:00','pbkdf2-sha256$600000$oyzHF2TyjgoUeFlXoxTw3A==$1vbSN1piuyOR4QqPiNsqHjzVJibCs0cQh+/mvJhwKew=','1','2026-11-15 12:00:00'),
(9,'Carlos','Ruiz','Morales','carlos.ruiz@example.com','1998-06-13','2026-05-15 12:00:00','2026-11-15 12:00:00','pbkdf2-sha256$600000$GrpOLcG58C1PhKIcPy1l6g==$BmnF09Ld2OUQ43mQDjSUYB9EAvaPl4ynIVPJcIXHT/U=','1','2026-11-15 12:00:00'),
(10,'Sofía','García','Vargas','sofia.garcia@example.com','1999-07-14','2026-03-15 12:00:00','2026-11-15 12:00:00','pbkdf2-sha256$600000$KMbgjWnjXzHZ3JkYdNBMqA==$VprPbjb8Wed0oIpg5PktH2CCvdiqZ9ksnMrBJk5Bnh4=','1','2026-11-15 12:00:00'),
(11,'Diego','Hernández','Castro','diego.hernandez@example.com','2000-08-15','2026-01-15 12:00:00','2026-11-15 12:00:00','pbkdf2-sha256$600000$KkqWTNT/AWF8nko/jnMVmw==$nqmQvEu34jXkTPg5S4jqi/0YlltkVmooxNGdOusrk7M=','1','2026-11-15 12:00:00'),
(12,'Valeria','Ramírez','Mendoza','valeria.ramirez@example.com','2000-01-01',NULL,'2026-11-15 12:00:00','pbkdf2-sha256$600000$v/WcE2V98TeZ806YvY1I3w==$BErbcSvFSQpr0YJeTSTL1hBEu6nat1WxNOVAug12SdY=','1','2026-11-15 12:00:00');
INSERT INTO administrador(usuario_id,folio) VALUES
(1,'ENR310307GB34'),
(2,'PED090507XJ87'),
(3,'UZI211207CY79'),
(4,'ALE220905FQ64'),
(5,'JAR270907WW20');
INSERT INTO cliente(usuario_id) VALUES (6),(7),(8),(9),(10),(11),(12);
INSERT INTO deshabilitacion(id,cliente_id,motivo,creada_en) VALUES (1,6,'Inactividad','2026-11-15 12:00:00');
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(1,6,'Computadora',12000.00,0.0000,3,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(1,1,0.00,4000.00),
(1,2,0.00,4000.00),
(1,3,0.00,4000.00);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(2,6,'Estudios',24000.00,12.0000,12,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(2,1,240.00,1892.37),
(2,2,221.08,1911.29),
(2,3,201.96,1930.41),
(2,4,182.66,1949.71),
(2,5,163.16,1969.21),
(2,6,143.47,1988.90),
(2,7,123.58,2008.79),
(2,8,103.49,2028.88),
(2,9,83.20,2049.17),
(2,10,62.71,2069.66),
(2,11,42.02,2090.35),
(2,12,21.11,2111.26);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(3,7,'Computadora',12000.00,0.0000,3,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(3,1,0.00,4000.00),
(3,2,0.00,4000.00),
(3,3,0.00,4000.00);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(4,7,'Estudios',24000.00,12.0000,12,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(4,1,240.00,1892.37),
(4,2,221.08,1911.29),
(4,3,201.96,1930.41),
(4,4,182.66,1949.71),
(4,5,163.16,1969.21),
(4,6,143.47,1988.90),
(4,7,123.58,2008.79),
(4,8,103.49,2028.88),
(4,9,83.20,2049.17),
(4,10,62.71,2069.66),
(4,11,42.02,2090.35),
(4,12,21.11,2111.26);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(5,8,'Computadora',12000.00,0.0000,3,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(5,1,0.00,4000.00),
(5,2,0.00,4000.00),
(5,3,0.00,4000.00);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(6,8,'Estudios',24000.00,12.0000,12,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(6,1,240.00,1892.37),
(6,2,221.08,1911.29),
(6,3,201.96,1930.41),
(6,4,182.66,1949.71),
(6,5,163.16,1969.21),
(6,6,143.47,1988.90),
(6,7,123.58,2008.79),
(6,8,103.49,2028.88),
(6,9,83.20,2049.17),
(6,10,62.71,2069.66),
(6,11,42.02,2090.35),
(6,12,21.11,2111.26);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(7,9,'Computadora',12000.00,0.0000,3,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(7,1,0.00,4000.00),
(7,2,0.00,4000.00),
(7,3,0.00,4000.00);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(8,9,'Estudios',24000.00,12.0000,12,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(8,1,240.00,1892.37),
(8,2,221.08,1911.29),
(8,3,201.96,1930.41),
(8,4,182.66,1949.71),
(8,5,163.16,1969.21),
(8,6,143.47,1988.90),
(8,7,123.58,2008.79),
(8,8,103.49,2028.88),
(8,9,83.20,2049.17),
(8,10,62.71,2069.66),
(8,11,42.02,2090.35),
(8,12,21.11,2111.26);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(9,10,'Computadora',12000.00,0.0000,3,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(9,1,0.00,4000.00),
(9,2,0.00,4000.00),
(9,3,0.00,4000.00);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(10,10,'Estudios',24000.00,12.0000,12,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(10,1,240.00,1892.37),
(10,2,221.08,1911.29),
(10,3,201.96,1930.41),
(10,4,182.66,1949.71),
(10,5,163.16,1969.21),
(10,6,143.47,1988.90),
(10,7,123.58,2008.79),
(10,8,103.49,2028.88),
(10,9,83.20,2049.17),
(10,10,62.71,2069.66),
(10,11,42.02,2090.35),
(10,12,21.11,2111.26);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(11,11,'Computadora',12000.00,0.0000,3,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(11,1,0.00,4000.00),
(11,2,0.00,4000.00),
(11,3,0.00,4000.00);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(12,11,'Estudios',24000.00,12.0000,12,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(12,1,240.00,1892.37),
(12,2,221.08,1911.29),
(12,3,201.96,1930.41),
(12,4,182.66,1949.71),
(12,5,163.16,1969.21),
(12,6,143.47,1988.90),
(12,7,123.58,2008.79),
(12,8,103.49,2028.88),
(12,9,83.20,2049.17),
(12,10,62.71,2069.66),
(12,11,42.02,2090.35),
(12,12,21.11,2111.26);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(13,12,'Computadora',12000.00,0.0000,3,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(13,1,0.00,4000.00),
(13,2,0.00,4000.00),
(13,3,0.00,4000.00);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(14,12,'Computadora a 6 meses',12000.00,0.0000,6,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(14,1,0.00,2000.00),
(14,2,0.00,2000.00),
(14,3,0.00,2000.00),
(14,4,0.00,2000.00),
(14,5,0.00,2000.00),
(14,6,0.00,2000.00);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(15,12,'Automóvil',180000.00,14.5000,48,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(15,1,2175.00,2789.03),
(15,2,2141.30,2822.73),
(15,3,2107.19,2856.84),
(15,4,2072.67,2891.36),
(15,5,2037.73,2926.30),
(15,6,2002.37,2961.66),
(15,7,1966.59,2997.44),
(15,8,1930.37,3033.66),
(15,9,1893.71,3070.32),
(15,10,1856.61,3107.42),
(15,11,1819.06,3144.97),
(15,12,1781.06,3182.97),
(15,13,1742.60,3221.43),
(15,14,1703.68,3260.35),
(15,15,1664.28,3299.75),
(15,16,1624.41,3339.62),
(15,17,1584.05,3379.98),
(15,18,1543.21,3420.82),
(15,19,1501.88,3462.15),
(15,20,1460.04,3503.99),
(15,21,1417.70,3546.33),
(15,22,1374.85,3589.18),
(15,23,1331.48,3632.55),
(15,24,1287.59,3676.44),
(15,25,1243.17,3720.86),
(15,26,1198.21,3765.82),
(15,27,1152.70,3811.33),
(15,28,1106.65,3857.38),
(15,29,1060.04,3903.99),
(15,30,1012.87,3951.16),
(15,31,965.12,3998.91),
(15,32,916.80,4047.23),
(15,33,867.90,4096.13),
(15,34,818.40,4145.63),
(15,35,768.31,4195.72),
(15,36,717.61,4246.42),
(15,37,666.30,4297.73),
(15,38,614.37,4349.66),
(15,39,561.81,4402.22),
(15,40,508.62,4455.41),
(15,41,454.78,4509.25),
(15,42,400.29,4563.74),
(15,43,345.15,4618.88),
(15,44,289.34,4674.69),
(15,45,232.85,4731.18),
(15,46,175.68,4788.35),
(15,47,117.82,4846.21),
(15,48,59.27,4904.81);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(16,12,'Estudios universitarios',45000.00,10.0000,24,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(16,1,375.00,1701.52),
(16,2,360.82,1715.70),
(16,3,346.52,1730.00),
(16,4,332.11,1744.41),
(16,5,317.57,1758.95),
(16,6,302.91,1773.61),
(16,7,288.13,1788.39),
(16,8,273.23,1803.29),
(16,9,258.20,1818.32),
(16,10,243.05,1833.47),
(16,11,227.77,1848.75),
(16,12,212.36,1864.16),
(16,13,196.83,1879.69),
(16,14,181.16,1895.36),
(16,15,165.37,1911.15),
(16,16,149.44,1927.08),
(16,17,133.38,1943.14),
(16,18,117.19,1959.33),
(16,19,100.86,1975.66),
(16,20,84.40,1992.12),
(16,21,67.80,2008.72),
(16,22,51.06,2025.46),
(16,23,34.18,2042.34),
(16,24,17.16,2059.38);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(17,12,'Remodelación de cocina',65000.00,16.0000,36,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(17,1,866.67,1418.54),
(17,2,847.75,1437.46),
(17,3,828.59,1456.62),
(17,4,809.17,1476.04),
(17,5,789.48,1495.73),
(17,6,769.54,1515.67),
(17,7,749.33,1535.88),
(17,8,728.85,1556.36),
(17,9,708.10,1577.11),
(17,10,687.07,1598.14),
(17,11,665.77,1619.44),
(17,12,644.17,1641.04),
(17,13,622.29,1662.92),
(17,14,600.12,1685.09),
(17,15,577.65,1707.56),
(17,16,554.89,1730.32),
(17,17,531.81,1753.40),
(17,18,508.44,1776.77),
(17,19,484.75,1800.46),
(17,20,460.74,1824.47),
(17,21,436.41,1848.80),
(17,22,411.76,1873.45),
(17,23,386.78,1898.43),
(17,24,361.47,1923.74),
(17,25,335.82,1949.39),
(17,26,309.83,1975.38),
(17,27,283.49,2001.72),
(17,28,256.80,2028.41),
(17,29,229.76,2055.45),
(17,30,202.35,2082.86),
(17,31,174.58,2110.63),
(17,32,146.44,2138.77),
(17,33,117.92,2167.29),
(17,34,89.02,2196.19),
(17,35,59.74,2225.47),
(17,36,30.07,2255.00);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(18,12,'Motocicleta',38000.00,18.0000,24,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(18,1,570.00,1327.12),
(18,2,550.09,1347.03),
(18,3,529.89,1367.23),
(18,4,509.38,1387.74),
(18,5,488.56,1408.56),
(18,6,467.43,1429.69),
(18,7,445.99,1451.13),
(18,8,424.22,1472.90),
(18,9,402.13,1494.99),
(18,10,379.70,1517.42),
(18,11,356.94,1540.18),
(18,12,333.84,1563.28),
(18,13,310.39,1586.73),
(18,14,286.59,1610.53),
(18,15,262.43,1634.69),
(18,16,237.91,1659.21),
(18,17,213.02,1684.10),
(18,18,187.76,1709.36),
(18,19,162.12,1735.00),
(18,20,136.10,1761.02),
(18,21,109.68,1787.44),
(18,22,82.87,1814.25),
(18,23,55.66,1841.46),
(18,24,28.03,1868.94);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(19,12,'Viaje familiar',25000.00,12.0000,12,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(19,1,250.00,1971.22),
(19,2,230.29,1990.93),
(19,3,210.38,2010.84),
(19,4,190.27,2030.95),
(19,5,169.96,2051.26),
(19,6,149.45,2071.77),
(19,7,128.73,2092.49),
(19,8,107.81,2113.41),
(19,9,86.67,2134.55),
(19,10,65.33,2155.89),
(19,11,43.77,2177.45),
(19,12,21.99,2199.24);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(20,12,'Equipo de fotografía',32000.00,15.0000,18,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(20,1,400.00,1596.31),
(20,2,380.05,1616.26),
(20,3,359.84,1636.47),
(20,4,339.39,1656.92),
(20,5,318.68,1677.63),
(20,6,297.71,1698.60),
(20,7,276.47,1719.84),
(20,8,254.97,1741.34),
(20,9,233.21,1763.10),
(20,10,211.17,1785.14),
(20,11,188.85,1807.46),
(20,12,166.26,1830.05),
(20,13,143.39,1852.92),
(20,14,120.22,1876.09),
(20,15,96.77,1899.54),
(20,16,73.03,1923.28),
(20,17,48.99,1947.32),
(20,18,24.65,1971.73);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(21,12,'Muebles para el hogar',28000.00,9.5000,12,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(21,1,221.67,2233.47),
(21,2,203.99,2251.15),
(21,3,186.16,2268.98),
(21,4,168.20,2286.94),
(21,5,150.10,2305.04),
(21,6,131.85,2323.29),
(21,7,113.45,2341.69),
(21,8,94.92,2360.22),
(21,9,76.23,2378.91),
(21,10,57.40,2397.74),
(21,11,38.42,2416.72),
(21,12,19.28,2435.85);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(22,12,'Paneles solares',85000.00,11.0000,36,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(22,1,779.17,2003.62),
(22,2,760.80,2021.99),
(22,3,742.27,2040.52),
(22,4,723.56,2059.23),
(22,5,704.68,2078.11),
(22,6,685.63,2097.16),
(22,7,666.41,2116.38),
(22,8,647.01,2135.78),
(22,9,627.43,2155.36),
(22,10,607.68,2175.11),
(22,11,587.74,2195.05),
(22,12,567.62,2215.17),
(22,13,547.31,2235.48),
(22,14,526.82,2255.97),
(22,15,506.14,2276.65),
(22,16,485.27,2297.52),
(22,17,464.21,2318.58),
(22,18,442.95,2339.84),
(22,19,421.51,2361.28),
(22,20,399.86,2382.93),
(22,21,378.02,2404.77),
(22,22,355.97,2426.82),
(22,23,333.73,2449.06),
(22,24,311.28,2471.51),
(22,25,288.62,2494.17),
(22,26,265.76,2517.03),
(22,27,242.69,2540.10),
(22,28,219.40,2563.39),
(22,29,195.90,2586.89),
(22,30,172.19,2610.60),
(22,31,148.26,2634.53),
(22,32,124.11,2658.68),
(22,33,99.74,2683.05),
(22,34,75.15,2707.64),
(22,35,50.33,2732.46),
(22,36,25.28,2757.57);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(23,12,'Herramientas de trabajo',18500.00,13.0000,18,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(23,1,200.42,936.36),
(23,2,190.27,946.51),
(23,3,180.02,956.76),
(23,4,169.65,967.13),
(23,5,159.18,977.60),
(23,6,148.59,988.19),
(23,7,137.88,998.90),
(23,8,127.06,1009.72),
(23,9,116.12,1020.66),
(23,10,105.06,1031.72),
(23,11,93.89,1042.89),
(23,12,82.59,1054.19),
(23,13,71.17,1065.61),
(23,14,59.62,1077.16),
(23,15,47.95,1088.83),
(23,16,36.16,1100.62),
(23,17,24.24,1112.54),
(23,18,12.18,1124.61);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(24,12,'Curso de idiomas',15000.00,8.0000,12,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(24,1,100.00,1204.83),
(24,2,91.97,1212.86),
(24,3,83.88,1220.95),
(24,4,75.74,1229.09),
(24,5,67.55,1237.28),
(24,6,59.30,1245.53),
(24,7,51.00,1253.83),
(24,8,42.64,1262.19),
(24,9,34.22,1270.61),
(24,10,25.75,1279.08),
(24,11,17.23,1287.60),
(24,12,8.64,1296.15);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(25,12,'Consultorio dental',120000.00,17.0000,48,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(25,1,1700.00,1762.61),
(25,2,1675.03,1787.58),
(25,3,1649.71,1812.90),
(25,4,1624.02,1838.59),
(25,5,1597.98,1864.63),
(25,6,1571.56,1891.05),
(25,7,1544.77,1917.84),
(25,8,1517.60,1945.01),
(25,9,1490.05,1972.56),
(25,10,1462.10,2000.51),
(25,11,1433.76,2028.85),
(25,12,1405.02,2057.59),
(25,13,1375.87,2086.74),
(25,14,1346.31,2116.30),
(25,15,1316.33,2146.28),
(25,16,1285.92,2176.69),
(25,17,1255.09,2207.52),
(25,18,1223.81,2238.80),
(25,19,1192.10,2270.51),
(25,20,1159.93,2302.68),
(25,21,1127.31,2335.30),
(25,22,1094.23,2368.38),
(25,23,1060.67,2401.94),
(25,24,1026.65,2435.96),
(25,25,992.14,2470.47),
(25,26,957.14,2505.47),
(25,27,921.64,2540.97),
(25,28,885.65,2576.96),
(25,29,849.14,2613.47),
(25,30,812.12,2650.49),
(25,31,774.57,2688.04),
(25,32,736.49,2726.12),
(25,33,697.87,2764.74),
(25,34,658.70,2803.91),
(25,35,618.98,2843.63),
(25,36,578.69,2883.92),
(25,37,537.84,2924.77),
(25,38,496.40,2966.21),
(25,39,454.38,3008.23),
(25,40,411.77,3050.84),
(25,41,368.54,3094.07),
(25,42,324.71,3137.90),
(25,43,280.26,3182.35),
(25,44,235.18,3227.43),
(25,45,189.45,3273.16),
(25,46,143.08,3319.53),
(25,47,96.06,3366.55),
(25,48,48.36,3413.95);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(26,12,'Negocio de repostería',55000.00,14.0000,24,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(26,1,641.67,1999.04),
(26,2,618.34,2022.37),
(26,3,594.75,2045.96),
(26,4,570.88,2069.83),
(26,5,546.73,2093.98),
(26,6,522.30,2118.41),
(26,7,497.59,2143.12),
(26,8,472.59,2168.12),
(26,9,447.29,2193.42),
(26,10,421.70,2219.01),
(26,11,395.81,2244.90),
(26,12,369.62,2271.09),
(26,13,343.13,2297.58),
(26,14,316.32,2324.39),
(26,15,289.20,2351.51),
(26,16,261.77,2378.94),
(26,17,234.01,2406.70),
(26,18,205.94,2434.77),
(26,19,177.53,2463.18),
(26,20,148.79,2491.92),
(26,21,119.72,2520.99),
(26,22,90.31,2550.40),
(26,23,60.55,2580.16),
(26,24,30.45,2610.21);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(27,12,'Reparación de automóvil',22000.00,19.0000,12,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(27,1,348.33,1679.11),
(27,2,321.75,1705.69),
(27,3,294.74,1732.70),
(27,4,267.31,1760.13),
(27,5,239.44,1788.00),
(27,6,211.13,1816.31),
(27,7,182.37,1845.07),
(27,8,153.16,1874.28),
(27,9,123.48,1903.96),
(27,10,93.33,1934.11),
(27,11,62.71,1964.73),
(27,12,31.60,1995.91);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(28,12,'Equipo de sonido',16500.00,12.5000,18,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(28,1,171.88,838.16),
(28,2,163.14,846.90),
(28,3,154.32,855.72),
(28,4,145.41,864.63),
(28,5,136.40,873.64),
(28,6,127.30,882.74),
(28,7,118.11,891.93),
(28,8,108.82,901.22),
(28,9,99.43,910.61),
(28,10,89.94,920.10),
(28,11,80.36,929.68),
(28,12,70.67,939.37),
(28,13,60.89,949.15),
(28,14,51.00,959.04),
(28,15,41.01,969.03),
(28,16,30.92,979.12),
(28,17,20.72,989.32),
(28,18,10.41,999.64);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(29,12,'Bicicleta eléctrica',27000.00,10.5000,24,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(29,1,236.25,1015.90),
(29,2,227.36,1024.79),
(29,3,218.39,1033.76),
(29,4,209.35,1042.80),
(29,5,200.22,1051.93),
(29,6,191.02,1061.13),
(29,7,181.73,1070.42),
(29,8,172.37,1079.78),
(29,9,162.92,1089.23),
(29,10,153.39,1098.76),
(29,11,143.78,1108.37),
(29,12,134.08,1118.07),
(29,13,124.29,1127.86),
(29,14,114.43,1137.72),
(29,15,104.47,1147.68),
(29,16,94.43,1157.72),
(29,17,84.30,1167.85),
(29,18,74.08,1178.07),
(29,19,63.77,1188.38),
(29,20,53.37,1198.78),
(29,21,42.88,1209.27),
(29,22,32.30,1219.85),
(29,23,21.63,1230.52),
(29,24,10.86,1241.36);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(30,12,'Gastos médicos',40000.00,9.0000,24,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(30,1,300.00,1527.39),
(30,2,288.54,1538.85),
(30,3,277.00,1550.39),
(30,4,265.38,1562.01),
(30,5,253.66,1573.73),
(30,6,241.86,1585.53),
(30,7,229.97,1597.42),
(30,8,217.99,1609.40),
(30,9,205.91,1621.48),
(30,10,193.75,1633.64),
(30,11,181.50,1645.89),
(30,12,169.16,1658.23),
(30,13,156.72,1670.67),
(30,14,144.19,1683.20),
(30,15,131.57,1695.82),
(30,16,118.85,1708.54),
(30,17,106.03,1721.36),
(30,18,93.12,1734.27),
(30,19,80.12,1747.27),
(30,20,67.01,1760.38),
(30,21,53.81,1773.58),
(30,22,40.51,1786.88),
(30,23,27.11,1800.28),
(30,24,13.60,1813.79);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(31,12,'Teléfono móvil',14000.00,0.0000,12,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(31,1,0.00,1166.67),
(31,2,0.00,1166.67),
(31,3,0.00,1166.67),
(31,4,0.00,1166.67),
(31,5,0.00,1166.67),
(31,6,0.00,1166.67),
(31,7,0.00,1166.67),
(31,8,0.00,1166.67),
(31,9,0.00,1166.67),
(31,10,0.00,1166.67),
(31,11,0.00,1166.67),
(31,12,0.00,1166.63);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(32,12,'Ampliación de vivienda',150000.00,15.5000,60,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(32,1,1937.50,1670.48),
(32,2,1915.92,1692.06),
(32,3,1894.07,1713.91),
(32,4,1871.93,1736.05),
(32,5,1849.51,1758.47),
(32,6,1826.79,1781.19),
(32,7,1803.78,1804.20),
(32,8,1780.48,1827.50),
(32,9,1756.88,1851.10),
(32,10,1732.97,1875.01),
(32,11,1708.75,1899.23),
(32,12,1684.21,1923.77),
(32,13,1659.37,1948.61),
(32,14,1634.20,1973.78),
(32,15,1608.70,1999.28),
(32,16,1582.88,2025.10),
(32,17,1556.72,2051.26),
(32,18,1530.22,2077.76),
(32,19,1503.39,2104.59),
(32,20,1476.20,2131.78),
(32,21,1448.67,2159.31),
(32,22,1420.78,2187.20),
(32,23,1392.52,2215.46),
(32,24,1363.91,2244.07),
(32,25,1334.92,2273.06),
(32,26,1305.56,2302.42),
(32,27,1275.82,2332.16),
(32,28,1245.70,2362.28),
(32,29,1215.19,2392.79),
(32,30,1184.28,2423.70),
(32,31,1152.97,2455.01),
(32,32,1121.26,2486.72),
(32,33,1089.14,2518.84),
(32,34,1056.61,2551.37),
(32,35,1023.65,2584.33),
(32,36,990.27,2617.71),
(32,37,956.46,2651.52),
(32,38,922.21,2685.77),
(32,39,887.52,2720.46),
(32,40,852.38,2755.60),
(32,41,816.79,2791.19),
(32,42,780.73,2827.25),
(32,43,744.22,2863.76),
(32,44,707.22,2900.76),
(32,45,669.76,2938.22),
(32,46,631.80,2976.18),
(32,47,593.36,3014.62),
(32,48,554.42,3053.56),
(32,49,514.98,3093.00),
(32,50,475.03,3132.95),
(32,51,434.56,3173.42),
(32,52,393.57,3214.41),
(32,53,352.05,3255.93),
(32,54,310.00,3297.98),
(32,55,267.40,3340.58),
(32,56,224.25,3383.73),
(32,57,180.54,3427.44),
(32,58,136.27,3471.71),
(32,59,91.43,3516.55),
(32,60,46.01,3561.85);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(33,12,'Computadora a 12 meses',12000.00,12.0000,12,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(33,1,120.00,946.19),
(33,2,110.54,955.65),
(33,3,100.98,965.21),
(33,4,91.33,974.86),
(33,5,81.58,984.61),
(33,6,71.73,994.46),
(33,7,61.79,1004.40),
(33,8,51.75,1014.44),
(33,9,41.60,1024.59),
(33,10,31.36,1034.83),
(33,11,21.01,1045.18),
(33,12,10.56,1055.58);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(34,12,'Muebles a 12 meses',24000.00,0.0000,12,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(34,1,0.00,2000.00),
(34,2,0.00,2000.00),
(34,3,0.00,2000.00),
(34,4,0.00,2000.00),
(34,5,0.00,2000.00),
(34,6,0.00,2000.00),
(34,7,0.00,2000.00),
(34,8,0.00,2000.00),
(34,9,0.00,2000.00),
(34,10,0.00,2000.00),
(34,11,0.00,2000.00),
(34,12,0.00,2000.00);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(35,12,'Muebles a 24 meses',24000.00,0.0000,24,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(35,1,0.00,1000.00),
(35,2,0.00,1000.00),
(35,3,0.00,1000.00),
(35,4,0.00,1000.00),
(35,5,0.00,1000.00),
(35,6,0.00,1000.00),
(35,7,0.00,1000.00),
(35,8,0.00,1000.00),
(35,9,0.00,1000.00),
(35,10,0.00,1000.00),
(35,11,0.00,1000.00),
(35,12,0.00,1000.00),
(35,13,0.00,1000.00),
(35,14,0.00,1000.00),
(35,15,0.00,1000.00),
(35,16,0.00,1000.00),
(35,17,0.00,1000.00),
(35,18,0.00,1000.00),
(35,19,0.00,1000.00),
(35,20,0.00,1000.00),
(35,21,0.00,1000.00),
(35,22,0.00,1000.00),
(35,23,0.00,1000.00),
(35,24,0.00,1000.00);
INSERT INTO simulacion(id,cliente_id,nombre,monto,tasa_anual,meses,moneda,creada_en,actualizada_en) VALUES
(36,12,'Curso de idiomas (10 meses sin interés)',15000.00,0.0000,10,'MXN','2026-11-15 12:00:00','2026-11-15 12:00:00');
INSERT INTO cuota(simulacion_id,numero,interes,abono_capital) VALUES
(36,1,0.00,1500.00),
(36,2,0.00,1500.00),
(36,3,0.00,1500.00),
(36,4,0.00,1500.00),
(36,5,0.00,1500.00),
(36,6,0.00,1500.00),
(36,7,0.00,1500.00),
(36,8,0.00,1500.00),
(36,9,0.00,1500.00),
(36,10,0.00,1500.00);
INSERT INTO apelacion(deshabilitacion_id,mensaje,creada_en,estado) VALUES (1,'Deseo retomar mis simulaciones y solicito reactivar mi cuenta.','2026-11-15 12:00:00','PENDIENTE');
