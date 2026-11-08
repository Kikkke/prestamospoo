#pragma once
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QCoreApplication>
#include <QDir>

// Fondo decorativo independiente del formulario: no requiere imágenes externas.
class FondoAcceso : public QWidget
{
public:
    explicit FondoAcceso(QWidget *parent = nullptr) : QWidget(parent) {}
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(), QColor("#08090c"));
        p.scale(width() / 1180.0, height() / 820.0);
        QRadialGradient halo(420, 380, 540);
        halo.setColorAt(0, QColor(119, 53, 35, 65));
        halo.setColorAt(1, QColor(8, 9, 12, 0));
        p.fillRect(QRectF(0, 0, 1180, 820), halo);
        QPainterPath cinta;
        cinta.moveTo(300, -100);
        cinta.cubicTo(350, 250, 620, 420, 750, 920);
        cinta.lineTo(1040, 920);
        cinta.cubicTo(780, 410, 530, 180, 610, -100);
        cinta.closeSubpath();
        QLinearGradient luz(280, 200, 820, 500);
        luz.setColorAt(0, QColor(168, 68, 36, 15));
        luz.setColorAt(0.45, QColor(167, 72, 42, 65));
        luz.setColorAt(1, QColor(28, 18, 20, 0));
        p.fillPath(cinta, luz);
    }
};

class IlustracionAcceso : public QWidget
{
public:
    explicit IlustracionAcceso(QWidget *parent = nullptr) : QWidget(parent)
    {
        preparar("acceso.png");
    }
    explicit IlustracionAcceso(const QString &archivo, QWidget *parent = nullptr) : QWidget(parent)
    {
        preparar(archivo);
    }
private:
    void preparar(const QString &archivo)
    {
        setObjectName("assetIllustration");
        setMinimumHeight(180);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        // Primero permite una imagen junto al ejecutable; después busca en el proyecto.
        imagen.load(QCoreApplication::applicationDirPath() + "/assets/" + archivo);
        if (imagen.isNull()) imagen.load(QStringLiteral(ASSETS_DIR) + "/" + archivo);
        setAccessibleName("Ilustración: " + archivo);
        setToolTip(archivo);
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        if (!imagen.isNull()) {
            // Escalar a píxeles físicos evita perder detalle en pantallas HiDPI.
            const qreal densidad = devicePixelRatioF();
            auto ajustada = imagen.scaled(size() * densidad, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            ajustada.setDevicePixelRatio(densidad);
            const QSizeF logica = QSizeF(ajustada.size()) / densidad;
            p.drawPixmap(QPointF((width() - logica.width()) / 2,
                                (height() - logica.height()) / 2), ajustada);
            return;
        }
        // Composición geométrica provisional hasta que se añada acceso.png.
        p.translate(width() / 2.0, height() / 2.0);
        const qreal escala = qMin(width() / 390.0, height() / 280.0);
        p.scale(escala, escala);
        p.setPen(QPen(QColor("#5e4640"), 1));
        p.drawEllipse(QPointF(0, 0), 115, 115);
        p.drawEllipse(QPointF(0, 0), 138, 138);
        p.rotate(-9);
        p.setBrush(QColor("#161519"));
        p.setPen(QPen(QColor("#9a7b70"), 1.5));
        p.drawRoundedRect(QRectF(-140, -78, 280, 156), 16, 16);
        p.setPen(QColor("#c9bab4"));
        QFont letra("Segoe UI", 11); letra.setLetterSpacing(QFont::AbsoluteSpacing, 3);
        p.setFont(letra);
        p.drawText(QRectF(-115, -55, 230, 30), "TUS PRÓXIMAS METAS");
        p.setPen(QPen(QColor("#ed916f"), 3, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(-110, 46, -50, 15);
        p.drawLine(-50, 15, 8, 27);
        p.drawLine(8, 27, 100, -13);
        p.drawLine(100, -13, 82, -13);
        p.drawLine(100, -13, 95, 5);
    }
private:
    QPixmap imagen;
};
