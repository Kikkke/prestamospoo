#pragma once
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QLineEdit>
#include <QStringList>

inline QString nombreCompleto(const QString &nombres, const QString &paterno, const QString &materno) {
    QStringList partes;
    for (const auto &parte : {nombres, paterno, materno})
        if (!parte.trimmed().isEmpty()) partes.append(parte.trimmed());
    return partes.join(' ');
}

// Filas nativas: QFormLayout calcula la altura con la fuente y escala actuales.
inline void agregarApellidos(QFormLayout *formulario, QLineEdit *paterno, QLineEdit *materno) {
    paterno->setPlaceholderText("Apellido paterno");
    materno->setPlaceholderText("Apellido materno");
    formulario->addRow("Apellido paterno",paterno);
    formulario->addRow("Apellido materno",materno);
}
