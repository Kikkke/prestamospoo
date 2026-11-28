#include "ServicioHistorial.h"
#include "HistorialSimulaciones.h"
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
#include <QMessageBox>
#include <QThreadPool>

namespace {
struct TriggerPrueba {
    QSqlDatabase bd;bool activo=false;
    ~TriggerPrueba(){if(activo){QSqlQuery q(bd);q.exec("DROP TRIGGER prueba_fallo_historial");}}
};
struct ConfiguracionPrueba {
    QByteArray anterior=qgetenv("FINANCIONANTI_CONFIG");
    ~ConfiguracionPrueba(){qputenv("FINANCIONANTI_CONFIG",anterior);}
};
}
class PruebasHistorial : public QObject {
    Q_OBJECT
    ConexionBD conexion;
    QSqlDatabase bd;
    Sesion a,b,vacia;
    int a1=0,a2=0,a3=0,b1=0;
    QVariant dato(const QString &sql) {
        QSqlQuery q(bd);if(!q.exec(sql) || !q.next())return {};return q.value(0);
    }
    Sesion crearCuenta(const QString &sufijo) {
        DatosRegistro d{"Prueba","Historial","Cliente","historial"+QString::number(QDateTime::currentMSecsSinceEpoch())+sufijo+"@example.com","Ejemplo#29A","Ejemplo#29A",QDate(2000,1,1),true,true};
        return ServicioAcceso(bd).registrar(d).sesion;
    }
    int guardar(const Sesion &s,const QString &nombre,qint64 monto,int tasa,int meses) {
        return ServicioSimulaciones(bd).guardar(s,CondicionesPrestamo{nombre,monto,tasa,meses}).id;
    }
    QPushButton *boton(QWidget &w,const QString &texto) {
        for(auto *b:w.findChildren<QPushButton*>())if(b->text()==texto)return b;
        return nullptr;
    }
    QPushButton *eliminar(QTableWidget *t,int id) {
        for(auto *b:t->findChildren<QPushButton*>())if(b->objectName()=="deleteRow" && b->property("simulacionId").toInt()==id)return b;
        return nullptr;
    }
    QSet<int> ids(QTableWidget *t,bool soloVisibles=false) {
        QSet<int> r;for(int i=0;i<t->rowCount();++i)if(!soloVisibles || !t->isRowHidden(i))r.insert(t->item(i,0)->data(Qt::UserRole+1).toInt());return r;
    }
    bool responder(QPushButton *boton,const QString &respuesta) {
        if(!boton)return false;
        bool respondido=false;QTimer timer;
        connect(&timer,&QTimer::timeout,&timer,[&]{
            auto *d=qobject_cast<QMessageBox*>(QApplication::activeModalWidget());
            if(!d)return;
            timer.stop();
            for(auto *b:d->buttons())if(b->text()==respuesta){respondido=true;b->click();return;}
            d->reject();
        });
        timer.start(30);boton->click();timer.stop();return respondido;
    }
private slots:
    void initTestCase() {
        qApp->setStyle(new EstiloSelectores);qApp->setStyleSheet(temaOscuro());
        QString error;QVERIFY2(conexion.abrir(error),qPrintable(error));bd=conexion.base();
        QCOMPARE(bd.databaseName(),QString("financionantimex_pruebas_acceso_20261126"));
        a=crearCuenta("a");b=crearCuenta("b");vacia=crearCuenta("v");QVERIFY(a.valida() && b.valida() && vacia.valida());
        a1=guardar(a,"Educación",900,100,2);a2=guardar(a,"Educación",10000,20000,120);a3=guardar(a,"Árbol_3",1000000,1550,12);b1=guardar(b,"Privada B",5000,0,3);
        QVERIFY(a1>0 && a2>0 && a3>0 && b1>0);
    }
    void propiedadYEstado() {
        ServicioHistorial servicio(bd);const auto lista=servicio.listar(a);QVERIFY2(lista.correcto(),qPrintable(lista.error));QCOMPARE(lista.registros.size(),3);
        QSet<int> propios;for(const auto &r:lista.registros)propios.insert(r.id);QCOMPARE(propios,(QSet<int>{a1,a2,a3}));
        auto ajena=servicio.eliminar(a,b1);QVERIFY(!ajena.correcto());QVERIFY(dato("SELECT eliminada_en FROM simulacion WHERE id="+QString::number(b1)).isNull());
        QVERIFY(!servicio.eliminar(a,-1).correcto());QVERIFY(!servicio.eliminar(a,2147483647).correcto());
        const auto vacio=servicio.listar(vacia);QVERIFY(vacio.correcto());QVERIFY(vacio.registros.isEmpty());
        auto admin=a;admin.rol=Rol::Administrador;QVERIFY(servicio.listar(admin).accesoDenegado);
        admin=a;admin.usuarioId=1;QVERIFY(servicio.listar(admin).accesoDenegado);
        auto restringida=a;restringida.restringida=true;QVERIFY(servicio.listar(restringida).accesoDenegado);
        QSqlQuery q(bd);q.prepare("INSERT INTO deshabilitacion(cliente_id,motivo,creada_en) VALUES(:id,'Prueba de permisos',NOW())");q.bindValue(":id",a.usuarioId);QVERIFY(q.exec());
        const int suspension=q.lastInsertId().toInt();
        const auto denegada=servicio.listar(a);const auto borrado=servicio.eliminar(a,a1);
        q.prepare("UPDATE deshabilitacion SET reactivada_en=NOW() WHERE id=:id");q.bindValue(":id",suspension);QVERIFY(q.exec());
        QVERIFY(denegada.accesoDenegado);QVERIFY(borrado.accesoDenegado);QVERIFY(dato("SELECT eliminada_en FROM simulacion WHERE id="+QString::number(a1)).isNull());
        ConexionBD otra;QString error;QVERIFY(otra.abrir(error));otra.base().close();
        const auto fallo=ServicioHistorial(otra.base()).listar(a);QVERIFY(!fallo.correcto());QVERIFY(!fallo.error.isEmpty());
    }
    void eliminacionPersistente() {
        const int id=guardar(a,"Eliminación persistente",123450,1200,3);QVERIFY(id>0);
        const auto str=QString::number(id);
        QSqlQuery q(bd);QVERIFY(q.exec("UPDATE simulacion SET creada_en='2020-01-01 00:00:00',actualizada_en='2020-01-01 00:00:00' WHERE id="+str));
        ServicioHistorial servicio(bd);const auto r=servicio.eliminar(a,id);QVERIFY2(r.correcto(),qPrintable(r.error));QCOMPARE(r.eliminadoId,id);
        QCOMPARE(dato("SELECT COUNT(*) FROM cuota WHERE simulacion_id="+str).toInt(),3);
        QCOMPARE(dato("SELECT COUNT(*) FROM simulacion WHERE id="+str+" AND eliminada_en IS NOT NULL AND actualizada_en=eliminada_en AND creada_en='2020-01-01 00:00:00'").toInt(),1);
        const auto marca=dato("SELECT eliminada_en FROM simulacion WHERE id="+str);
        QVERIFY(!servicio.eliminar(a,id).correcto());QCOMPARE(dato("SELECT eliminada_en FROM simulacion WHERE id="+str),marca);
        ConexionBD otra;QString error;QVERIFY(otra.abrir(error));const auto recarga=ServicioHistorial(otra.base()).listar(a);QVERIFY(recarga.correcto());
        for(const auto &sim:recarga.registros)QVERIFY(sim.id!=id);
    }
    void interfazBusquedaYEliminacion() {
        PanelPrincipal w;w.sesionActual=a;w.cargarPerfilSesion(a.nombre,a.paterno,a.materno,a.correo,a.nacimiento);w.resize(1180,820);w.show();QVERIFY(QTest::qWaitForWindowExposed(&w));
        boton(w,"Historial")->click();auto *h=w.findChild<HistorialSimulaciones*>();QVERIFY(h);
        auto *t=w.findChild<QTableWidget*>("tablaHistorial");auto *buscar=w.findChild<QLineEdit*>("buscarHistorial");auto *actualizar=w.findChild<QPushButton*>("actualizarHistorial");auto *estado=w.findChild<QLabel*>("estadoHistorial");
        QTRY_VERIFY_WITH_TIMEOUT(actualizar->isEnabled(),10000);QCOMPARE(t->rowCount(),3);QCOMPARE(ids(t),(QSet<int>{a1,a2,a3}));
        buscar->setText("EDUCACION");QCOMPARE(ids(t,true),(QSet<int>{a1,a2}));
        for(int columna:{1,2,3}) {
            t->sortItems(columna,Qt::AscendingOrder);
            for(int i=1;i<t->rowCount();++i)QVERIFY(t->item(i-1,columna)->data(Qt::UserRole).toLongLong()<=t->item(i,columna)->data(Qt::UserRole).toLongLong());
            QCOMPARE(ids(t,true),(QSet<int>{a1,a2}));
        }
        buscar->setText("' OR 1=1 --");QVERIFY(ids(t,true).isEmpty());buscar->clear();
        for(auto *b:t->findChildren<QPushButton*>())if(b->objectName()=="verAmortizacion")QVERIFY(b->isEnabled());
        w.grab().save(QCoreApplication::applicationDirPath()+"/historial-verificado.png");
        w.resize(920,700);QTest::qWait(100);w.grab().save(QCoreApplication::applicationDirPath()+"/historial-reducido.png");
        QVERIFY(responder(eliminar(t,a1),"Cancelar"));QCOMPARE(t->rowCount(),3);
        QVERIFY(dato("SELECT eliminada_en FROM simulacion WHERE id="+QString::number(a1)).isNull());
        TriggerPrueba trigger{bd};QSqlQuery q(bd);
        QVERIFY(q.exec("CREATE TRIGGER prueba_fallo_historial BEFORE UPDATE ON simulacion FOR EACH ROW BEGIN IF OLD.id="+QString::number(a1)+" THEN SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT='Prueba de rollback'; END IF; END"));trigger.activo=true;
        QVERIFY(responder(eliminar(t,a1),"Eliminar"));
        QTRY_VERIFY_WITH_TIMEOUT(actualizar->isEnabled(),10000);QCOMPARE(t->rowCount(),3);QVERIFY(!estado->text().isEmpty());
        QVERIFY(dato("SELECT eliminada_en FROM simulacion WHERE id="+QString::number(a1)).isNull());
        QVERIFY(q.exec("DROP TRIGGER prueba_fallo_historial"));trigger.activo=false;
        buscar->setText("educación");t->sortItems(1,Qt::DescendingOrder);
        auto *quitar=eliminar(t,a1);QVERIFY(responder(quitar,"Eliminar"));quitar->click();
        QTRY_VERIFY_WITH_TIMEOUT(actualizar->isEnabled(),10000);QCOMPARE(t->rowCount(),2);QCOMPARE(ids(t,true),(QSet<int>{a2}));
        QVERIFY(estado->text().contains("eliminada"));QCOMPARE(dato("SELECT COUNT(*) FROM cuota WHERE simulacion_id="+QString::number(a1)).toInt(),2);
        QVERIFY(dato("SELECT eliminada_en FROM simulacion WHERE id="+QString::number(a2)).isNull());
        {
            ConfiguracionPrueba config;qputenv("FINANCIONANTI_CONFIG","configuracion_inexistente_para_prueba.ini");h->recargar(a);
            QTRY_VERIFY_WITH_TIMEOUT(actualizar->isEnabled(),10000);QCOMPARE(t->rowCount(),0);QVERIFY(!estado->text().isEmpty());QVERIFY(w.findChild<QLabel*>("vacioHistorial")->isHidden());
        }
        actualizar->click();QTRY_VERIFY_WITH_TIMEOUT(actualizar->isEnabled(),10000);QCOMPARE(t->rowCount(),2);QCOMPARE(buscar->text(),QString("educación"));
        h->recargar(a);h->recargar(b);QTRY_VERIFY_WITH_TIMEOUT(actualizar->isEnabled(),10000);QCOMPARE(ids(t),(QSet<int>{b1}));QVERIFY(buscar->text().isEmpty());
        h->recargar(vacia);QTRY_VERIFY_WITH_TIMEOUT(actualizar->isEnabled(),10000);QCOMPARE(t->rowCount(),0);QVERIFY(w.findChild<QLabel*>("vacioHistorial")->text().contains("Aún no tienes"));QVERIFY(estado->text().isEmpty());
    }
    void actualizarTrasGuardarYVolver() {
        PanelPrincipal w;w.sesionActual=b;w.resize(1180,820);w.show();QVERIFY(QTest::qWaitForWindowExposed(&w));
        boton(w,"Historial")->click();auto *t=w.findChild<QTableWidget*>("tablaHistorial");auto *actualizar=w.findChild<QPushButton*>("actualizarHistorial");QTRY_VERIFY_WITH_TIMEOUT(actualizar->isEnabled(),10000);QCOMPARE(t->rowCount(),1);
        boton(w,"Inicio · Simulador")->click();
        w.findChild<QLineEdit*>("nombreSimulacion")->setText("Nueva desde simulador");w.findChild<QLineEdit*>("montoSimulacion")->setText("12000");w.findChild<QLineEdit*>("tasaSimulacion")->setText("0");w.findChild<QLineEdit*>("mesesSimulacion")->setText("3");
        boton(w,"Calcular préstamo")->click();boton(w,"Guardar simulación")->click();
        QTRY_VERIFY_WITH_TIMEOUT(w.isEnabled(),10000);QVERIFY(w.findChild<QLabel*>("estadoSimulacion")->text().contains("guardada correctamente"));
        QTRY_COMPARE_WITH_TIMEOUT(t->rowCount(),2,10000); // Se actualiza incluso antes de navegar al historial.
        QVERIFY(guardar(b,"Cambio externo",80000,100,6)>0);
        boton(w,"Historial")->click();QTRY_VERIFY_WITH_TIMEOUT(actualizar->isEnabled(),10000);QCOMPARE(t->rowCount(),3);
    }
    void cleanupTestCase(){QThreadPool::globalInstance()->waitForDone();bd=QSqlDatabase();}
};
QTEST_MAIN(PruebasHistorial)
#include "PruebasHistorial.moc"
