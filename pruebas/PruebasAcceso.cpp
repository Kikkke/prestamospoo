#include "Acceso.h"
#include "EditarPerfil.h"
#include "ConexionBD.h"
#include "PantallaLogin.h"
#include "PantallaRegistro.h"
#include "VentanaAcceso.h"
#include "Tema.h"
#include "SelectorDesplegable.h"
#include <QtTest>
#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QTextBrowser>
#include <QScrollBar>
#include <QDateEdit>
#include <QThreadPool>
#include <QLabel>
#include <QDialog>
#include <QInputDialog>
#include <memory>
#include <QStandardPaths>
#include <QFileInfo>

class PruebasAcceso : public QObject {
    Q_OBJECT
    ConexionBD conexion;
    QSqlDatabase bd;
    QJsonArray claves;
    QString correoNuevo;
    DatosRegistro datos() const {
        return {"José Luis","López","Muñoz",correoNuevo,"Ejemplo#29A","Ejemplo#29A",QDate(2000,2,29),true,true};
    }
    QVariant escalar(const QString &sql) {
        QSqlQuery q(bd); if(!q.exec(sql) || !q.next()) return {};
        return q.value(0);
    }
    QPushButton *boton(QWidget &w,const QString &texto) {
        for(auto *b:w.findChildren<QPushButton*>()) if(b->text()==texto)return b;
        return nullptr;
    }
private slots:
    void initTestCase() {
        QStandardPaths::setTestModeEnabled(true);
        qApp->setStyle(new EstiloSelectores);
        qApp->setStyleSheet(temaOscuro());
        qRegisterMetaType<Sesion>();
        QString error;
        QVERIFY2(conexion.abrir(error),qPrintable(error));
        bd=conexion.base();
        QCOMPARE(bd.databaseName(),QString("financionantimex_pruebas_acceso_20261126"));
        QFile f(QCoreApplication::applicationDirPath()+"/claves-prueba.local.json");
        QVERIFY(f.open(QIODevice::ReadOnly)); claves=QJsonDocument::fromJson(f.readAll()).array(); QCOMPARE(claves.size(),12);
        correoNuevo="registro"+QString::number(QDateTime::currentMSecsSinceEpoch())+"@example.com";
    }
    void esquemaYSemillas() {
        QCOMPARE(escalar("SELECT COUNT(*) FROM usuario").toInt(),12);
        QCOMPARE(escalar("SELECT COUNT(*) FROM cuota").toInt(),619);
        QCOMPARE(escalar("SELECT COUNT(*) FROM simulacion").toInt(),36);
        QCOMPARE(escalar("SELECT COUNT(*) FROM usuario WHERE terminos_version='1' AND terminos_aceptados_en IS NOT NULL").toInt(),12);
        QCOMPARE(escalar("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema=DATABASE() AND engine='InnoDB'").toInt(),8);
        QSqlQuery q(bd); QVERIFY(q.exec("SELECT id,password_hash FROM usuario ORDER BY id"));
        QSet<QString> hashes;
        while(q.next()) {
            const auto clave=claves[q.value(0).toInt()-1].toString();
            QVERIFY(Validaciones::clave(clave).isEmpty());
            QVERIFY(Claves::verificar(clave,q.value(1).toString()));
            hashes.insert(q.value(1).toString());
        }
        QCOMPARE(hashes.size(),12);
    }
    void validaciones() {
        auto d=datos(); QVERIFY(Validaciones::registro(d).isEmpty());
        d.paterno=" "; QVERIFY(!Validaciones::registro(d).isEmpty());
        d=datos(); d.materno=""; QVERIFY(!Validaciones::registro(d).isEmpty());
        d=datos(); d.nombre="Ana7"; QVERIFY(!Validaciones::registro(d).isEmpty());
        d=datos(); d.correo="a..b@example.com"; QVERIFY(!Validaciones::registro(d).isEmpty());
        d=datos(); d.nacimiento=QDate::currentDate().addYears(-18); QVERIFY(Validaciones::registro(d).isEmpty());
        d.nacimiento=d.nacimiento.addDays(1); QVERIFY(!Validaciones::registro(d).isEmpty());
        d=datos(); d.nacimiento=QDate::currentDate().addDays(1); QVERIFY(!Validaciones::registro(d).isEmpty());
        d=datos(); d.lecturaCompleta=false; QVERIFY(!Validaciones::registro(d).isEmpty());
        d=datos(); d.aceptaTerminos=false; QVERIFY(!Validaciones::registro(d).isEmpty());
        d=datos(); d.confirmacion="Otra#99A"; QVERIFY(!Validaciones::registro(d).isEmpty());
        for(const QString &invalida:QStringList{"Ab1#abc","Ab1#abcdefghijkl","abcdefgh1#","ABCDEFGH1#","Abcdefgh##","Abcdefgh12","Abcd 12!"})
            QVERIFY(!Validaciones::clave(invalida).isEmpty());
        QVERIFY(Validaciones::clave("Ab1#abcd").isEmpty());
        QVERIFY(Validaciones::clave("Ab1#abcdefghijk").isEmpty());
    }
    void hashing() {
        auto a=Claves::proteger("Ejemplo#29A"), b=Claves::proteger("Ejemplo#29A");
        QVERIFY(a!=b); QVERIFY(Claves::verificar("Ejemplo#29A",a));
        QVERIFY(!Claves::verificar("Incorrecta#1",a));
        QVERIFY(!Claves::verificar("",{}));
        QVERIFY(!Claves::verificar("Ejemplo#29A","pbkdf2-sha256$9999999999$x$y"));
        QVERIFY(!Claves::verificar("Ejemplo#29A",a+"!"));
    }
    void accesoSemillas() {
        ServicioAcceso servicio(bd);
        QVERIFY(!servicio.ingresar("x' OR 1=1 --","Cualquier#1").correcto());
        for(int id=1;id<=12;++id) {
            QSqlQuery q(bd); q.prepare("SELECT correo,ultimo_acceso FROM usuario WHERE id=:id");q.bindValue(":id",id);
            QVERIFY(q.exec()); QVERIFY(q.next()); const auto correo=q.value(0).toString();const auto antes=q.value(1);q.finish();
            auto resultado=servicio.ingresar(correo,claves[id-1].toString());
            if(id<=5) {
                QVERIFY(resultado.requiereFolio); QVERIFY(!resultado.sesion.valida());
                QCOMPARE(escalar("SELECT ultimo_acceso FROM usuario WHERE id="+QString::number(id)),antes);
                QVERIFY(!servicio.ingresar(correo,claves[id-1].toString(),"NO_CORRESPONDE").correcto());
                q.prepare("SELECT folio FROM administrador WHERE usuario_id=:id");q.bindValue(":id",id);QVERIFY(q.exec());QVERIFY(q.next());
                const auto folio=q.value(0).toString(); q.finish();
                resultado=servicio.ingresar(correo.toUpper(),claves[id-1].toString(),folio);
                QVERIFY(resultado.correcto()); QCOMPARE(resultado.sesion.rol,Rol::Administrador);
                QVERIFY(servicio.autorizarFolio(resultado.sesion,folio));
                QVERIFY(!servicio.autorizarFolio({},folio));
            } else {
                QVERIFY2(resultado.correcto(),qPrintable(resultado.error));
                QCOMPARE(resultado.sesion.rol,Rol::Cliente);
                QCOMPARE(resultado.sesion.restringida,id==6);
                if(id==6) { QCOMPARE(resultado.sesion.motivo,QString("Inactividad")); QCOMPARE(escalar("SELECT ultimo_acceso FROM usuario WHERE id=6"),antes); }
            }
            QCOMPARE(resultado.sesion.usuarioId,id);
        }
    }
    void registroYPersistencia() {
        ServicioAcceso servicio(bd);
        auto r=servicio.registrar(datos()); QVERIFY2(r.correcto(),qPrintable(r.error));
        QCOMPARE(r.sesion.nombre,QString("José Luis")); QCOMPARE(r.sesion.materno,QString("Muñoz"));
        const auto id=QString::number(r.sesion.usuarioId);
        QCOMPARE(escalar("SELECT COUNT(*) FROM cliente WHERE usuario_id="+id).toInt(),1);
        QCOMPARE(escalar("SELECT terminos_version FROM usuario WHERE id="+id).toString(),VERSION_TERMINOS);
        auto d=datos();d.correo=d.correo.toUpper();QVERIFY(!servicio.registrar(d).correcto());
        d=datos();d.correo="lopez.garfias.enrique@gmail.com"; QVERIFY(!servicio.registrar(d).correcto());
        ConexionBD otra;QString error;QVERIFY(otra.abrir(error));
        ServicioAcceso nuevaConexion(otra.base());
        auto entrada=nuevaConexion.ingresar(correoNuevo,datos().clave);QVERIFY(entrada.correcto());QCOMPARE(entrada.sesion.usuarioId,r.sesion.usuarioId);
    }
    void rollbackYRolesInvalidos() {
        ServicioAcceso servicio(bd);
        const int antes=escalar("SELECT COUNT(*) FROM usuario").toInt();
        QSqlQuery q(bd);
        QVERIFY(q.exec("CREATE TRIGGER prueba_fallo_cliente BEFORE INSERT ON cliente FOR EACH ROW SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT='Prueba de rollback'"));
        auto d=datos();d.correo="rollback_"+correoNuevo;
        const auto r=servicio.registrar(d);
        QVERIFY(q.exec("DROP TRIGGER prueba_fallo_cliente"));
        QVERIFY(!r.correcto()); QCOMPARE(escalar("SELECT COUNT(*) FROM usuario").toInt(),antes);
        const QString hash=escalar("SELECT password_hash FROM usuario WHERE id=7").toString();
        QVERIFY(q.exec("UPDATE usuario SET password_hash=NULL WHERE id=7"));
        QVERIFY(!servicio.ingresar("luis.perez@example.com",claves[6].toString()).correcto());
        q.prepare("UPDATE usuario SET password_hash=:hash WHERE id=7");q.bindValue(":hash",hash);QVERIFY(q.exec());
        QVERIFY(q.exec("INSERT INTO administrador(usuario_id,folio) VALUES(7,'LUI110496AB12')"));
        QVERIFY(!servicio.ingresar("luis.perez@example.com",claves[6].toString()).correcto());
        QVERIFY(q.exec("DELETE FROM administrador WHERE usuario_id=7"));
        QVERIFY(q.exec("UPDATE administrador SET eliminado_en=NOW() WHERE usuario_id=2"));
        QVERIFY(!servicio.ingresar("pescador.velazquez.pedro@gmail.com",claves[1].toString()).correcto());
        QVERIFY(q.exec("UPDATE administrador SET eliminado_en=NULL WHERE usuario_id=2"));
    }
    void formularioRegistro() {
        PantallaRegistro w;w.resize(430,800);w.show();
        QSignalSpy completado(&w,&PantallaRegistro::accesoCompletado);
        w.findChild<QLineEdit*>("nombreRegistro")->setText("Ana María");
        w.findChild<QLineEdit*>("paternoRegistro")->setText("López");
        w.findChild<QLineEdit*>("maternoRegistro")->setText("Pérez");
        w.findChild<QLineEdit*>("correoRegistro")->setText("ui_"+correoNuevo);
        w.findChild<QLineEdit*>("claveRegistro")->setText("Registro#25A");
        w.findChild<QLineEdit*>("confirmacionRegistro")->setText("Registro#25A");
        auto *casilla=w.findChild<QCheckBox*>("aceptarTerminos");
        QVERIFY(!casilla->isChecked()); QVERIFY(!boton(w,"Crear cuenta")->isEnabled());
        QTimer::singleShot(100,[](){if(auto *d=qobject_cast<QDialog*>(QApplication::activeModalWidget()))d->reject();});
        QTest::mouseClick(casilla,Qt::LeftButton); QVERIFY(!casilla->isChecked());
        QTimer::singleShot(200,[](){
            auto *d=qobject_cast<QDialog*>(QApplication::activeModalWidget());if(!d)return;
            auto *texto=d->findChild<QTextBrowser*>(); if(texto)texto->verticalScrollBar()->setValue(texto->verticalScrollBar()->maximum());
            for(auto *b:d->findChildren<QPushButton*>())if(b->text()=="Volver al registro" && b->isEnabled())b->click();
        });
        boton(w,"Leer términos y condiciones")->click();
        QVERIFY(!casilla->isChecked());
        QTest::mouseClick(casilla,Qt::LeftButton); QVERIFY(casilla->isChecked());
        boton(w,"Crear cuenta")->click();QVERIFY(!w.isEnabled());
        QTRY_COMPARE_WITH_TIMEOUT(completado.count(),1,30000);
        QVERIFY(!casilla->isChecked()); QVERIFY(w.findChild<QLineEdit*>("claveRegistro")->text().isEmpty());
    }
    void navegacionYSesion() {
        VentanaAcceso w(bd);w.show();QTest::qWait(200);
        w.grab().save(QCoreApplication::applicationDirPath()+"/login-verificado.png");
        auto *login=w.findChild<PantallaLogin*>();QVERIFY(login);
        login->findChild<QLineEdit*>("correoLogin")->setText(correoNuevo);
        login->findChild<QLineEdit*>("claveLogin")->setText(datos().clave);
        boton(*login,"Iniciar sesión")->click();
        QTRY_VERIFY_WITH_TIMEOUT(boton(w,"Cerrar sesión")!=nullptr,30000);
        bool identidad=false;for(auto *l:w.findChildren<QLabel*>())if(l->text()=="Bienvenido, José Luis")identidad=true;QVERIFY(identidad);
        boton(w,"Cerrar sesión")->click();QTest::qWait(100);
        QVERIFY(login->isVisible());QVERIFY(!boton(w,"Cerrar sesión"));
        QVERIFY(login->findChild<QLineEdit*>("claveLogin")->text().isEmpty());
        login->findChild<QLineEdit*>("correoLogin")->setText("ana.lopez@example.com");
        login->findChild<QLineEdit*>("claveLogin")->setText(claves[5].toString());
        boton(*login,"Iniciar sesión")->click();
        QTRY_VERIFY_WITH_TIMEOUT(boton(w,"Cerrar sesión")!=nullptr,30000);
        QVERIFY(!boton(w,"Inicio · Simulador"));
        w.grab().save(QCoreApplication::applicationDirPath()+"/restringida-verificada.png");
        boton(w,"Cerrar sesión")->click();QTest::qWait(100);
        QVERIFY(login->isVisible());
    }
    void folioCancelacionYAccesoAdmin() {
        VentanaAcceso w(bd);w.show();
        auto *login=w.findChild<PantallaLogin*>();
        const auto antes=escalar("SELECT ultimo_acceso FROM usuario WHERE id=2");
        QTimer respuesta;respuesta.setInterval(50);
        bool cancelar=true;QString folio="AJENO";
        connect(&respuesta,&QTimer::timeout,&w,[&]() {
            if(auto *d=qobject_cast<QInputDialog*>(QApplication::activeModalWidget())) {
                respuesta.stop();
                if(cancelar)d->reject();else {d->setTextValue(folio);d->accept();}
            }
        });
        auto enviar=[&]() {
            login->findChild<QLineEdit*>("correoLogin")->setText("pescador.velazquez.pedro@gmail.com");
            login->findChild<QLineEdit*>("claveLogin")->setText(claves[1].toString());
            respuesta.start();boton(*login,"Iniciar sesión")->click();
        };
        enviar();
        QTRY_VERIFY_WITH_TIMEOUT(!respuesta.isActive(),30000);
        QVERIFY(login->isVisible());QVERIFY(!boton(w,"Cerrar sesión"));
        QCOMPARE(escalar("SELECT ultimo_acceso FROM usuario WHERE id=2"),antes);
        cancelar=false;enviar();
        QTRY_VERIFY_WITH_TIMEOUT(!respuesta.isActive(),30000);
        QTRY_VERIFY_WITH_TIMEOUT(login->isEnabled(),30000);
        QVERIFY(login->isVisible());QVERIFY(!boton(w,"Cerrar sesión"));
        QCOMPARE(escalar("SELECT ultimo_acceso FROM usuario WHERE id=2"),antes);
        folio=escalar("SELECT folio FROM administrador WHERE usuario_id=2").toString();enviar();
        QTRY_VERIFY_WITH_TIMEOUT(boton(w,"Cerrar sesión")!=nullptr,30000);
        bool pedro=false;for(auto *l:w.findChildren<QLabel*>())if(l->text()=="Bienvenido, Pedro")pedro=true;QVERIFY(pedro);
        QVERIFY(!boton(w,"Inicio · Simulador"));
        w.grab().save(QCoreApplication::applicationDirPath()+"/admin-verificado.png");
        boton(w,"Cerrar sesión")->click();QTest::qWait(100);
        QVERIFY(login->isVisible());QVERIFY(!boton(w,"Cerrar sesión"));
        boton(*login,"Crear cuenta")->click();QTest::qWait(100);
        w.grab().save(QCoreApplication::applicationDirPath()+"/registro-verificado.png");
    }
    void perfilPersistente() {
        ServicioAcceso servicio(bd);
        auto d=datos();d.correo="perfil"+QString::number(QDateTime::currentMSecsSinceEpoch())+"@example.com";
        auto alta=servicio.registrar(d);QVERIFY2(alta.correcto(),qPrintable(alta.error));
        auto sesion=alta.sesion;const auto original=d;
        d.clave.clear();d.confirmacion.clear();
        QVERIFY(!servicio.actualizarPerfil(sesion,d,{}).correcto());
        d.nombre=QString(41,'A');QVERIFY(!servicio.actualizarPerfil(sesion,d,{}).correcto());
        d.nombre="José7";QVERIFY(!servicio.actualizarPerfil(sesion,d,{}).correcto());
        d.nombre="José";d.paterno=" ";QVERIFY(!servicio.actualizarPerfil(sesion,d,{}).correcto());
        d.paterno=original.paterno;d.correo="incorrecto";QVERIFY(!servicio.actualizarPerfil(sesion,d,{}).correcto());
        d.correo="lopez.garfias.enrique@gmail.com";QVERIFY(!servicio.actualizarPerfil(sesion,d,{}).correcto());
        d.correo=original.correo;d.nacimiento=QDate::currentDate();QVERIFY(!servicio.actualizarPerfil(sesion,d,{}).correcto());
        d.nacimiento=original.nacimiento;d.clave=original.clave;d.confirmacion=d.clave;
        QVERIFY(!servicio.actualizarPerfil(sesion,d,original.clave).correcto());
        d.clave="Nueva#34Ab";d.confirmacion=d.clave;
        QVERIFY(!servicio.actualizarPerfil(sesion,d,"Incorrecta1#").correcto());
        auto cambio=servicio.actualizarPerfil(sesion,d,original.clave);QVERIFY2(cambio.correcto(),qPrintable(cambio.error));
        QVERIFY(!servicio.ingresar(d.correo,original.clave).correcto());
        ConexionBD otra;QString error;QVERIFY(otra.abrir(error));ServicioAcceso lectura(otra.base());
        auto entrada=lectura.ingresar(d.correo,d.clave);QVERIFY(entrada.correcto());QCOMPARE(entrada.sesion.nombre,QString("José"));
        QImage foto(512,512,QImage::Format_ARGB32);foto.fill(Qt::red);
        auto imagen=servicio.guardarFoto(entrada.sesion,foto);QVERIFY2(imagen.correcto(),qPrintable(imagen.error));
        QVERIFY(QFileInfo::exists(imagen.sesion.fotoRuta));
        QCOMPARE(lectura.ingresar(d.correo,d.clave).sesion.fotoRuta,imagen.sesion.fotoRuta);
        QVERIFY(!servicio.guardarFoto(imagen.sesion,foto).correcto());
        QVERIFY(!servicio.guardarFoto(imagen.sesion,QImage(10,10,QImage::Format_RGB32)).correcto());
        auto quitar=servicio.guardarFoto(imagen.sesion,{});QVERIFY(quitar.correcto());QVERIFY(quitar.sesion.fotoRuta.isEmpty());
        QVERIFY(!QFileInfo::exists(imagen.sesion.fotoRuta));
        QVERIFY(lectura.ingresar(d.correo,d.clave).sesion.fotoRuta.isEmpty());
        auto invalida=sesion;invalida.rol=Rol::Administrador;
        QVERIFY(!servicio.guardarFoto(invalida,foto).correcto());
        invalida=sesion;invalida.restringida=true;
        QVERIFY(!servicio.actualizarPerfil(invalida,d,d.clave).correcto());
    }
    void editorPerfilYFotoAdmin() {
        ServicioAcceso servicio(bd);auto d=datos();
        d.correo="editor"+QString::number(QDateTime::currentMSecsSinceEpoch())+"@example.com";
        auto r=servicio.registrar(d);QVERIFY(r.correcto());auto sesion=r.sesion;
        bool completado=false;
        QTimer limite;
        limite.setSingleShot(true);
        connect(&limite,&QTimer::timeout,this,[]{if(auto *modal=qobject_cast<QDialog*>(QApplication::activeModalWidget()))modal->reject();});
        limite.start(15000);
        QTimer::singleShot(100,this,[&] {
            auto *modal=QApplication::activeModalWidget();if(!modal)return;
            auto *nombre=modal->findChild<QLineEdit*>("nombrePerfil");if(!nombre)return;
            QCOMPARE(nombre->maxLength(),40);
            nombre->setText("Nombre Nuevo");
            modal->grab().save(QCoreApplication::applicationDirPath()+"/editor-perfil-verificado.png");
            if(auto *guardar=boton(*modal,"Guardar cambios")){completado=true;guardar->click();}
        });
        QVERIFY(editarPerfil(nullptr,d.nombre,d.paterno,d.materno,d.correo,d.nacimiento,[](const QString &,const QString &){return true;},&sesion));
        limite.stop();QVERIFY(completado);QCOMPARE(d.nombre,QString("Nombre Nuevo"));
        QCOMPARE(servicio.ingresar(d.correo,d.clave).sesion.nombre,d.nombre);
        const auto folio=escalar("SELECT folio FROM administrador WHERE usuario_id=2").toString();
        const auto correo=escalar("SELECT correo FROM usuario WHERE id=2").toString();
        auto admin=servicio.ingresar(correo,claves[1].toString(),folio);QVERIFY(admin.correcto());
        const auto antes=escalar("SELECT COUNT(*) FROM actividad_administrativa WHERE administrador_id=2 AND accion='FOTO_PERFIL'").toInt();
        QImage foto(512,512,QImage::Format_ARGB32);foto.fill(Qt::blue);
        auto guardada=servicio.guardarFoto(admin.sesion,foto);QVERIFY2(guardada.correcto(),qPrintable(guardada.error));
        QVERIFY(servicio.guardarFoto(guardada.sesion,{}).correcto());
        QCOMPARE(escalar("SELECT COUNT(*) FROM actividad_administrativa WHERE administrador_id=2 AND accion='FOTO_PERFIL'").toInt(),antes+2);
    }
    void cleanupTestCase() {QThreadPool::globalInstance()->waitForDone(); bd=QSqlDatabase();}
};
QTEST_MAIN(PruebasAcceso)
#include "PruebasAcceso.moc"

