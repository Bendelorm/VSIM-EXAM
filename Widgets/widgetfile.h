#ifndef WIDGETFILE_H
#define WIDGETFILE_H

#include <QWidget>
#include "../Systems/filesystem.h"

namespace Ui {
class WidgetFile;
}

class WidgetFile : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetFile(QWidget *parent = nullptr);
    ~WidgetFile();

    void resizeEvent(QResizeEvent *event) override;

    void setIcon(const QPixmap& pixmap);
    void setTitle(const std::string& title);
    void setFileType(const gea::FileType& fileType);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

protected slots:
    void showContextMenu(const QPoint &pos);
    void renameFileDialog();

private:
    Ui::WidgetFile *ui;
};


#endif // WIDGETFILE_H
