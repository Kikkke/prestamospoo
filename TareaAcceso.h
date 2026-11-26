#pragma once
#include "Acceso.h"
#include "ConexionBD.h"
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QWidget>
#include <functional>

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
