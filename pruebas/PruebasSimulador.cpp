#include "Simulador.h"
#include "ConexionBD.h"
#include "PanelPrincipal.h"
#include "Tema.h"
#include "SelectorDesplegable.h"
#include <QtTest>
#include <QSqlQuery>
#include <QSqlError>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QThreadPool>
#include <QScrollArea>
#include <QScrollBar>
#include <limits>

class PruebasSimulador : public QObject {
    Q_OBJECT
    ConexionBD conexion;
    QSqlDatabase bd;
    Sesion sesion;
    QVariant dato(const QString &sql) {
        QSqlQuery q(bd);if(!q.exec(sql) || !q.next())return {};return q.value(0);
    }
    QPushButton *boton(QWidget &w,const QString &texto) {
        for(auto *b:w.findChildren<QPushButton*>())if(b->text()==texto)return b;
        return nullptr;
    }
private slots:
    void initTestCase() {
        qApp->setStyle(new EstiloSelectores);qApp->setStyleSheet(temaOscuro());
        QString error;QVERIFY2(conexion.abrir(error),qPrintable(error));bd=conexion.base();
        QCOMPARE(bd.databaseName(),QString("financionantimex_pruebas_acceso_20261126"));
        DatosRegistro d{"Prueba","Motor","Prestamo","simulador"+QString::number(QDateTime::currentMSecsSinceEpoch())+"@example.com","Ejemplo#29A","Ejemplo#29A",QDate(2000,1,1),true,true};
        auto r=ServicioAcceso(bd).registrar(d);QVERIFY2(r.correcto(),qPrintable(r.error));sesion=r.sesion;
    }
    void entradas() {
        const EntradaPrestamo base{"Estudios_2026","12000.00","12.00","12"};
        QVERIFY(MotorPrestamo::calcular(base).valido());
        for(const QString &nombre:QStringList{"", "   ",QString(51,'A'),"Plan-1","Plan!","Plan\nOtro"}) {
            auto e=base;e.nombre=nombre;QVERIFY2(!MotorPrestamo::calcular(e).valido(),qPrintable(nombre));
        }
        auto e=base;e.nombre=QString(50,'A');QVERIFY(MotorPrestamo::calcular(e).valido());
        e.nombre="Educación de José_2";QVERIFY(MotorPrestamo::calcular(e).valido());
        for(const QString &monto:QStringList{"","0","0.99","-1","10000000.01","1.234","NaN","inf","1e3","1,000.00","1,50","9999999999999999999999999"}) {
            e=base;e.monto=monto;QVERIFY2(!MotorPrestamo::calcular(e).valido(),qPrintable(monto));
        }
        for(const QString &tasa:QStringList{"","-0.01","200.01","12.345","NaN","infinity","1e2","12,5"}) {
            e=base;e.tasa=tasa;QVERIFY2(!MotorPrestamo::calcular(e).valido(),qPrintable(tasa));
        }
        for(const QString &plazo:QStringList{"","0","481","-1","3.5","3.0","1e2","4800"}) {
            e=base;e.meses=plazo;QVERIFY2(!MotorPrestamo::calcular(e).valido(),qPrintable(plazo));
        }
        auto d=MotorPrestamo::calcular(base).condiciones;
        d.monto=std::numeric_limits<qint64>::max();QVERIFY(!MotorPrestamo::calcular(d).valido());
        d.monto=100;d.tasa=20001;QVERIFY(!MotorPrestamo::calcular(d).valido());
        d.tasa=0;d.meses=-1;QVERIFY(!MotorPrestamo::calcular(d).valido());
    }
    void calculosYExtremos() {
        auto r=MotorPrestamo::calcular(EntradaPrestamo{"Computadora","12000","0","3"});
        QVERIFY(r.valido());QCOMPARE(r.regular,qint64(400000));QCOMPARE(r.total,qint64(1200000));QCOMPARE(r.intereses,qint64(0));
        r=MotorPrestamo::calcular(EntradaPrestamo{"Vivienda","150000","15.5","60"});
        QVERIFY(r.valido());QCOMPARE(r.regular,qint64(360798));QCOMPARE(r.cuotas.last().pago,qint64(360786));QCOMPARE(r.intereses,qint64(6647868));
        r=MotorPrestamo::calcular(EntradaPrestamo{"Pequeño","1","0","480"});
        QVERIFY(r.valido());QCOMPARE(r.regular,qint64(0));QCOMPARE(r.cuotas[478].pago,qint64(0));QCOMPARE(r.cuotas.last().pago,qint64(100));QVERIFY(r.aviso.contains("$0.00"));
        r=MotorPrestamo::calcular(EntradaPrestamo{"Centavos","1.01","0","2"});
        QCOMPARE(r.cuotas.first().pago,qint64(51));QCOMPARE(r.cuotas.last().pago,qint64(50));
        // Invariantes en todo el dominio, incluidos liquidación temprana y tasa mínima positiva.
        for(qint64 monto:QList<qint64>{100,101,999,1200000,15000000,1000000000})
            for(int tasa:{0,1,100,1200,1550,20000})for(int meses:{1,2,3,60,480}) {
                r=MotorPrestamo::calcular(CondicionesPrestamo{"Extremo",monto,tasa,meses});
                QVERIFY2(r.valido(),qPrintable(r.error));QCOMPARE(r.cuotas.size(),meses);
                qint64 capital=0,intereses=0,pagos=0,saldo=monto;
                for(int i=0;i<meses;++i) {
                    const auto &q=r.cuotas[i];QCOMPARE(q.numero,i+1);QCOMPARE(q.inicial,saldo);
                    QVERIFY(q.capital>=0);QVERIFY(q.interes>=0);QVERIFY(q.final>=0);
                    QCOMPARE(q.pago,q.interes+q.capital);QCOMPARE(q.final,q.inicial-q.capital);
                    saldo=q.final;capital+=q.capital;intereses+=q.interes;pagos+=q.pago;
                }
                QCOMPARE(saldo,qint64(0));QCOMPARE(capital,monto);QCOMPARE(intereses,r.intereses);QCOMPARE(pagos,r.total);QCOMPARE(pagos,monto+intereses);
            }
    }
    void guardarYRollback() {
        const auto c=MotorPrestamo::calcular(EntradaPrestamo{"Persistente","150000","15.5","60"});
        const QString ultimo=dato("SELECT ultimo_acceso FROM usuario WHERE id="+QString::number(sesion.usuarioId)).toString();
        ServicioSimulaciones servicio(bd);auto r=servicio.guardar(sesion,c.condiciones);QVERIFY2(r.correcto(),qPrintable(r.error));
        ConexionBD nueva;QString error;QVERIFY(nueva.abrir(error));
        QSqlQuery q(nueva.base());q.prepare("SELECT cliente_id,nombre,CAST(monto*100 AS SIGNED),tasa_anual,meses,moneda FROM simulacion WHERE id=:id");q.bindValue(":id",r.id);
        QVERIFY(q.exec());QVERIFY(q.next());QCOMPARE(q.value(0).toInt(),sesion.usuarioId);QCOMPARE(q.value(1).toString(),QString("Persistente"));QCOMPARE(q.value(2).toLongLong(),qint64(15000000));QCOMPARE(q.value(3).toDouble(),15.5);QCOMPARE(q.value(4).toInt(),60);QCOMPARE(q.value(5).toString(),QString("MXN"));
        q.prepare("SELECT numero,CAST(interes*100 AS SIGNED),CAST(abono_capital*100 AS SIGNED) FROM cuota WHERE simulacion_id=:id ORDER BY numero");q.bindValue(":id",r.id);QVERIFY(q.exec());
        int filas=0;while(q.next()) {const auto &cuota=c.cuotas[filas++];QCOMPARE(q.value(0).toInt(),cuota.numero);QCOMPARE(q.value(1).toLongLong(),cuota.interes);QCOMPARE(q.value(2).toLongLong(),cuota.capital);}QCOMPARE(filas,60);
        QCOMPARE(dato("SELECT ultimo_acceso FROM usuario WHERE id="+QString::number(sesion.usuarioId)).toString(),ultimo);
        auto sinPermiso=sesion;sinPermiso.rol=Rol::Administrador;QVERIFY(!servicio.guardar(sinPermiso,c.condiciones).correcto());
        sinPermiso=sesion;sinPermiso.usuarioId=6;QVERIFY(!servicio.guardar(sinPermiso,c.condiciones).correcto()); // Ana: suspensión real.
        sinPermiso=sesion;sinPermiso.usuarioId=1;QVERIFY(!servicio.guardar(sinPermiso,c.condiciones).correcto()); // Un admin no es cliente.
        const auto cantidad=dato("SELECT COUNT(*) FROM simulacion");const auto cuotas=dato("SELECT COUNT(*) FROM cuota");
        QSqlQuery trigger(bd);
        QVERIFY(trigger.exec("CREATE TRIGGER prueba_fallo_cuota BEFORE INSERT ON cuota FOR EACH ROW BEGIN IF NEW.numero=2 THEN SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT='Prueba de rollback'; END IF; END"));
        const auto fallo=servicio.guardar(sesion,c.condiciones);
        QVERIFY(trigger.exec("DROP TRIGGER prueba_fallo_cuota"));
        QVERIFY(!fallo.correcto());QCOMPARE(dato("SELECT COUNT(*) FROM simulacion"),cantidad);QCOMPARE(dato("SELECT COUNT(*) FROM cuota"),cuotas);
        QVERIFY(servicio.guardar(sesion,c.condiciones).correcto()); // Nombre repetido permitido.
    }
    void formulario() {
        PanelPrincipal w;w.sesionActual=sesion;w.cargarPerfilSesion(sesion.nombre,sesion.paterno,sesion.materno,sesion.correo,sesion.nacimiento);w.resize(1180,820);w.show();
        QVERIFY(QTest::qWaitForWindowExposed(&w));
        auto *nombre=w.findChild<QLineEdit*>("nombreSimulacion");auto *monto=w.findChild<QLineEdit*>("montoSimulacion");auto *tasa=w.findChild<QLineEdit*>("tasaSimulacion");auto *meses=w.findChild<QLineEdit*>("mesesSimulacion");
        auto *tabla=w.findChild<QTableWidget*>("amortizationTable");auto *guardar=boton(w,"Guardar simulación");auto *calcular=boton(w,"Calcular préstamo");auto *estado=w.findChild<QLabel*>("estadoSimulacion");
        QVERIFY(nombre && monto && tasa && meses && tabla && guardar && calcular && estado);QCOMPARE(nombre->maxLength(),50);QVERIFY(!guardar->isEnabled());QCOMPARE(tabla->rowCount(),0);
        nombre->setText("Prueba_UI");monto->setText("12000");tasa->setText("0");meses->setText("3.5");calcular->click();QCOMPARE(meses->text(),QString("3.5"));QVERIFY(!guardar->isEnabled());QVERIFY(estado->text().contains("entero"));
        meses->setText("3");calcular->click();QCOMPARE(tabla->rowCount(),3);QVERIFY(guardar->isEnabled());QCOMPARE(tabla->item(2,5)->text(),QString("0.00"));
        monto->setText("12001");QVERIFY(!guardar->isEnabled());monto->setText("12000");QVERIFY(!guardar->isEnabled());calcular->click();
        const auto antes=dato("SELECT COUNT(*) FROM simulacion WHERE cliente_id="+QString::number(sesion.usuarioId)).toInt();
        guardar->click();guardar->click();QTRY_VERIFY_WITH_TIMEOUT(w.isEnabled(),10000);QVERIFY2(estado->text().contains("guardada correctamente"),qPrintable(estado->text()));QVERIFY(!guardar->isEnabled());
        QCOMPARE(dato("SELECT COUNT(*) FROM simulacion WHERE cliente_id="+QString::number(sesion.usuarioId)).toInt(),antes+1);
        calcular->click();QVERIFY(!guardar->isEnabled()); // Recalcular no vuelve a guardar el mismo resultado.
        monto->setText("150000");tasa->setText("15.5");meses->setText("60");calcular->click();QVERIFY(guardar->isEnabled());QCOMPARE(tabla->rowCount(),60);
        w.grab().save(QCoreApplication::applicationDirPath()+"/simulador-verificado.png");
        for(auto *scroll:w.findChildren<QScrollArea*>())if(scroll->isVisible())scroll->verticalScrollBar()->setValue(scroll->verticalScrollBar()->maximum());
        QTest::qWait(100);w.grab().save(QCoreApplication::applicationDirPath()+"/simulador-tabla-verificada.png");
        w.resize(920,700);monto->setText("10000000");tasa->setText("200");meses->setText("480");calcular->click();QCOMPARE(tabla->rowCount(),480);
        QTest::qWait(100);w.grab().save(QCoreApplication::applicationDirPath()+"/simulador-extremo-verificado.png");
    }
    void cleanupTestCase() {QThreadPool::globalInstance()->waitForDone();bd=QSqlDatabase();}
};
QTEST_MAIN(PruebasSimulador)
#include "PruebasSimulador.moc"
