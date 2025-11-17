#include "widgetsquareimage.h"

WidgetSquareImage::WidgetSquareImage(QWidget* parent) : QLabel(parent)
{
    setScaledContents(false);
}

void WidgetSquareImage::setPixmap(const QPixmap &pixmap)
{
    defaultPixmap = pixmap;
    updateScaledPixmap();
}

void WidgetSquareImage::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);
    updateScaledPixmap();
}

void WidgetSquareImage::updateScaledPixmap()
{
    if (defaultPixmap.isNull())
        return;

    int side = qMin(width(), height());
    QPixmap scaled = defaultPixmap.scaled(side, side, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QLabel::setPixmap(scaled);
}
