#include "Amortizacion.h"
#include "ServicioHistorial.h"
#include "VentanaAmortizacion.h"
#include "PanelPrincipal.h"
#include "Tema.h"
#include "SelectorDesplegable.h"
#include <QtTest>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QTimer>
#include <QThreadPool>

class PruebasAmortizacion : public QObject {
    Q_OBJECT
    ConexionBD conexion;QSqlDatabase bd;Sesion a,b;QString salida;
    Sesion cuenta(const QString &sufijo) {
        DatosRegistro d{"Prueba","Detalle","PDF","pdf"+QString::number(QDateTime::currentMSecsSinceEpoch())+sufijo+"@example.com","Ejemplo#29A","Ejemplo#29A",QDate(2000,1,1),true,true};return ServicioAcceso(bd).registrar(d).sesion;
    }
    int guardar(int meses,const QString &nombre="Educación_Árbol",qint64 monto=15000000,int tasa=1550) {
        return ServicioSimulaciones(bd).guardar(a,CondicionesPrestamo{nombre,monto,tasa,meses}).id;
    }
    QPushButton *boton(QWidget &w,const QString &nombre) {for(auto *b:w.findChildren<QPushButton*>())if(b->text()==nombre)return b;return nullptr;}
    QByteArray leer(const QString &ruta){QFile f(ruta);if(!f.open(QIODevice::ReadOnly))return {};return f.readAll();}
private slots:
    void initTestCase() {
        QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
        qApp->setStyle(new EstiloSelectores);qApp->setStyleSheet(temaOscuro());
        QString error;QVERIFY2(conexion.abrir(error),qPrintable(error));bd=conexion.base();
        QCOMPARE(bd.databaseName(),QString("financionantimex_pruebas_acceso_20261126"));
        a=cuenta("a");b=cuenta("b");QVERIFY(a.valida() && b.valida());
        salida=QCoreApplication::applicationDirPath()+"/pdfs-prueba";QVERIFY(QDir().mkpath(salida));
    }
    void lecturaSinRecalcularEIntegridad() {
        const int id=guardar(3,"Cuotas guardadas",1200000,1200);QVERIFY(id>0);
        ServicioAmortizacion servicio(bd);auto r=servicio.consultar(a,id);QVERIFY2(r.correcto(),qPrintable(r.error));
        QCOMPARE(r.detalle.cuotas.size(),3);QCOMPARE(r.detalle.total,qint64(1224080));QCOMPARE(r.detalle.cuotas.last().pago,qint64(408026));
        QSqlQuery q(bd);QVERIFY(q.exec("UPDATE cuota SET interes=interes+0.01 WHERE simulacion_id="+QString::number(id)+" AND numero=1"));
        r=servicio.consultar(a,id);QVERIFY(r.correcto());QCOMPARE(r.detalle.total,qint64(1224081));QCOMPARE(r.detalle.cuotas.first().interes,qint64(12001)); // Lee el dato, no recalcula el motor.
        QVERIFY(q.exec("UPDATE cuota SET numero=4 WHERE simulacion_id="+QString::number(id)+" AND numero=3"));
        auto corrupto=servicio.consultar(a,id);QVERIFY(!corrupto.correcto());
        const QString destino=salida+"/inconsistente.pdf";QFile::remove(destino);QVERIFY(!servicio.exportar(a,id,destino).correcto());QVERIFY(!QFileInfo::exists(destino));
        QVERIFY(q.exec("UPDATE cuota SET numero=3 WHERE simulacion_id="+QString::number(id)+" AND numero=4"));
        QVERIFY(q.exec("UPDATE simulacion SET meses=4 WHERE id="+QString::number(id)));QVERIFY(!servicio.consultar(a,id).correcto());
        QVERIFY(q.exec("UPDATE simulacion SET meses=3 WHERE id="+QString::number(id)));QVERIFY(servicio.consultar(a,id).correcto());
        // También comprueba que se pueden consultar los datos históricos sin aplicar reglas nuevas al nombre.
        QVERIFY(q.exec("SELECT id,cliente_id FROM simulacion WHERE id<=36 AND cliente_id<>6 AND eliminada_en IS NULL"));
        int muestras=0;while(q.next()){auto cliente=a;cliente.usuarioId=q.value(1).toInt();auto historico=servicio.consultar(cliente,q.value(0).toInt());QVERIFY2(historico.correcto(),qPrintable(historico.error));++muestras;}QVERIFY(muestras>0);
    }
    void permisosYEscrituraAtomica() {
        const int id=guardar(3);QVERIFY(id>0);ServicioAmortizacion servicio(bd);
        const QString destino=salida+"/protegido.pdf";
        {QFile f(destino);QVERIFY(f.open(QIODevice::WriteOnly));QCOMPARE(f.write("ORIGINAL"),qint64(8));}
        QVERIFY(!servicio.consultar(b,id).correcto());QVERIFY(!servicio.exportar(b,id,destino).correcto());QCOMPARE(leer(destino),QByteArray("ORIGINAL"));
        auto admin=a;admin.rol=Rol::Administrador;QVERIFY(!servicio.consultar(admin,id).correcto());
        QVERIFY(servicio.exportar(a,id,{}).cancelado);
        QSqlQuery q(bd);q.prepare("INSERT INTO deshabilitacion(cliente_id,motivo,creada_en) VALUES(:id,'Prueba PDF',NOW())");q.bindValue(":id",a.usuarioId);QVERIFY(q.exec());const int suspension=q.lastInsertId().toInt();
        const auto consulta=servicio.consultar(a,id);const auto exportacion=servicio.exportar(a,id,destino);
        q.prepare("UPDATE deshabilitacion SET reactivada_en=NOW() WHERE id=:id");q.bindValue(":id",suspension);QVERIFY(q.exec());
        QVERIFY(!consulta.correcto());QVERIFY(!exportacion.correcto());QCOMPARE(leer(destino),QByteArray("ORIGINAL"));
        const QString carpeta=salida+"/directorio.pdf";QVERIFY(QDir().mkpath(carpeta));
        QVERIFY(!servicio.exportar(a,id,carpeta).correcto());QVERIFY(QFileInfo(carpeta).isDir());
        QVERIFY(!servicio.exportar(a,id,salida+"/carpeta_inexistente/archivo.pdf").correcto());
        QVERIFY(ServicioHistorial(bd).eliminar(a,id).correcto());QVERIFY(!servicio.exportar(a,id,destino).correcto());QCOMPARE(leer(destino),QByteArray("ORIGINAL"));
    }
    void pdfsCompletos() {
        ServicioAmortizacion servicio(bd);
        for(int meses:{1,22,23,44,60,480}) {
            const int id=guardar(meses,meses==23?QString(50,'W'):"Educación_Árbol",meses==480?1000000000:15000000,meses==480?20000:1550);QVERIFY(id>0);
            QSqlQuery q(bd);QVERIFY(q.exec("UPDATE simulacion SET creada_en='2020-01-02 03:04:05',actualizada_en='2021-02-03 04:05:06' WHERE id="+QString::number(id)));
            const QString ruta=salida+"/amortizacion_"+QString::number(meses);auto r=servicio.exportar(a,id,ruta);QVERIFY2(r.correcto(),qPrintable(r.error));QCOMPARE(r.paginas,(meses+21)/22);
            QVERIFY(leer(ruta+".pdf").startsWith("%PDF"));QVERIFY(leer(ruta+".pdf").trimmed().endsWith("%%EOF"));
        }
        const int id=guardar(480,"Pagos cero",100,0);QVERIFY(id>0);
        QVERIFY(servicio.exportar(a,id,salida+"/cero_480.PDF").correcto());
    }
    void ventanaCancelacionYRegistroEliminado() {
        const int id=guardar(60);QVERIFY(id>0);VentanaAmortizacion d(a,id);d.show();QVERIFY(QTest::qWaitForWindowExposed(&d));
        auto *exportar=boton(d,"Exportar PDF");auto *tabla=d.findChild<QTableWidget*>("detalleAmortizacion");auto *estado=d.findChild<QLabel*>("estadoAmortizacion");
        QTRY_VERIFY_WITH_TIMEOUT(exportar->isEnabled(),10000);QCOMPARE(tabla->rowCount(),61);QCOMPARE(tabla->item(59,5)->text(),QString("0.00"));QCOMPARE(tabla->item(60,1)->text(),QString("—"));
        d.grab().save(QCoreApplication::applicationDirPath()+"/amortizacion-verificada.png");d.resize(680,600);QTest::qWait(100);d.grab().save(QCoreApplication::applicationDirPath()+"/amortizacion-reducida.png");d.resize(1000,760);
        bool cancelado=false;QTimer cancelar;
        connect(&cancelar,&QTimer::timeout,&cancelar,[&]{if(auto *f=qobject_cast<QFileDialog*>(QApplication::activeModalWidget())){cancelado=true;f->reject();cancelar.stop();}});
        cancelar.start(30);exportar->click();cancelar.stop();QVERIFY(cancelado);QVERIFY(exportar->isEnabled());
        const QString ruta=salida+"/desde_ventana.pdf";QFile::remove(ruta);bool elegido=false;QTimer elegir;
        connect(&elegir,&QTimer::timeout,&elegir,[&]{if(auto *f=qobject_cast<QFileDialog*>(QApplication::activeModalWidget())){auto *nombre=f->findChild<QLineEdit*>("fileNameEdit");if(!nombre){f->reject();return;}elegido=true;nombre->setText(QDir::toNativeSeparators(ruta));QMetaObject::invokeMethod(f,"accept");elegir.stop();}});
        elegir.start(30);exportar->click();elegir.stop();QVERIFY(elegido);
        QTRY_VERIFY_WITH_TIMEOUT(d.isEnabled(),15000);QVERIFY2(estado->text().contains("guardado correctamente"),qPrintable(estado->text()));QVERIFY(QFileInfo::exists(ruta));
        const QByteArray original=leer(ruta);bool reemplazoCancelado=false;QTimer reemplazar;
        connect(&reemplazar,&QTimer::timeout,&reemplazar,[&]{
            if(auto *f=qobject_cast<QFileDialog*>(QApplication::activeModalWidget())){auto *nombre=f->findChild<QLineEdit*>("fileNameEdit");if(!nombre){f->reject();return;}nombre->setText(QDir::toNativeSeparators(ruta));QMetaObject::invokeMethod(f,"accept");}
            else if(auto *mensaje=qobject_cast<QMessageBox*>(QApplication::activeModalWidget())){reemplazoCancelado=true;mensaje->button(QMessageBox::No)->click();reemplazar.stop();}
        });
        reemplazar.start(30);exportar->click();reemplazar.stop();QVERIFY(reemplazoCancelado);QCOMPARE(leer(ruta),original);QVERIFY(exportar->isEnabled());
        QVERIFY(ServicioHistorial(bd).eliminar(a,id).correcto());
        QTRY_COMPARE_WITH_TIMEOUT(tabla->rowCount(),0,10000);QVERIFY(!exportar->isEnabled());QVERIFY(estado->text().contains("disponible"));
    }
    void abrirDesdeHistorialOrdenado() {
        const int id=guardar(3,"Mismo nombre",1200000,0);QVERIFY(id>0);QVERIFY(guardar(6,"Mismo nombre",2400000,0)>0);
        PanelPrincipal w;w.sesionActual=a;w.resize(1180,820);w.show();boton(w,"Historial")->click();
        auto *actualizar=w.findChild<QPushButton*>("actualizarHistorial");QTRY_VERIFY_WITH_TIMEOUT(actualizar->isEnabled(),10000);
        auto *tabla=w.findChild<QTableWidget*>("tablaHistorial");tabla->sortItems(1,Qt::DescendingOrder);
        QPushButton *ver=nullptr;for(auto *b:tabla->findChildren<QPushButton*>())if(b->objectName()=="verAmortizacion" && b->property("simulacionId").toInt()==id)ver=b;QVERIFY(ver);
        bool correcto=false;QTimer cerrar,limite;limite.setSingleShot(true);
        connect(&limite,&QTimer::timeout,&limite,[]{if(auto *d=qobject_cast<QDialog*>(QApplication::activeModalWidget()))d->reject();});limite.start(10000);
        connect(&cerrar,&QTimer::timeout,&cerrar,[&]{auto *d=qobject_cast<VentanaAmortizacion*>(QApplication::activeModalWidget());if(!d || !boton(*d,"Exportar PDF")->isEnabled())return;auto *t=d->findChild<QTableWidget*>("detalleAmortizacion");correcto=t->rowCount()==4 && t->item(0,1)->text()=="12,000.00";cerrar.stop();d->reject();});
        cerrar.start(30);ver->click();cerrar.stop();limite.stop();QVERIFY(correcto);
        QTRY_VERIFY_WITH_TIMEOUT(actualizar->isEnabled(),10000);
    }
    void cleanupTestCase(){QThreadPool::globalInstance()->waitForDone();bd=QSqlDatabase();}
};
QTEST_MAIN(PruebasAmortizacion)
#include "PruebasAmortizacion.moc"
