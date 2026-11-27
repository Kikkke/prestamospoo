#pragma once
#include "Acceso.h"
#include "ConexionBD.h"
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QWidget>
#include <functional>
#include <QDialog>
#include <QCloseEvent>

// No permite cerrar el editor mientras se confirma una escritura.
class DialogoGuardado : public QDialog {
public:
    using QDialog::QDialog;
    void reject() override { if(isEnabled()) QDialog::reject(); }
protected:
    void closeEvent(QCloseEvent *e) override { if(!isEnabled()) e->ignore(); else QDialog::closeEvent(e); }
};

// PBKDF2 y SQL se ejecutan fuera del hilo gráfico. La conexión nace y muere allí.
inline void ejecutarAcceso(QWidget *pantalla,
    std::function<ResultadoAcceso(ServicioAcceso &)> operacion,
    std::function<void(ResultadoAcceso)> terminado) {
    pantalla->setEnabled(false);
    auto *observador=new QFutureWatcher<ResultadoAcceso>(pantalla);
    QObject::connect(observador,&QFutureWatcher<ResultadoAcceso>::finished,pantalla,[=]() {
        pantalla->setEnabled(true);
        const auto resultado=observador->result();
        observador->deleteLater();
        terminado(resultado);
    });
    observador->setFuture(QtConcurrent::run([operacion]() {
        ConexionBD conexion;
        QString error;
        if (!conexion.abrir(error)) { ResultadoAcceso r; r.error=error; return r; }
        ServicioAcceso servicio(conexion.base());
        return operacion(servicio);
    }));
}
