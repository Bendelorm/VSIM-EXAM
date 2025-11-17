#ifndef WIDGETSQUAREIMAGE_H
#define WIDGETSQUAREIMAGE_H

#include <QLabel>
#include <QPixmap>

class WidgetSquareImage : public QLabel
{
    QPixmap defaultPixmap;

public:
    WidgetSquareImage(QWidget* parent = nullptr);

    void setPixmap(const QPixmap &pixmap);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateScaledPixmap();

};

#endif // WIDGETSQUAREIMAGE_H
