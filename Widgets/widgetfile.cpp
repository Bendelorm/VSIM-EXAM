#include "widgetfile.h"
#include "../UI/ui_widgetfile.h"
#include "../magic_enum.hpp"
#include <QResizeEvent>
#include <qdir.h>
#include <qmenu.h>
#include <QInputDialog>
#include <QMessageBox>

WidgetFile::WidgetFile(QWidget *parent): QWidget(parent), ui(new Ui::WidgetFile)
{
    ui->setupUi(this);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &WidgetFile::showContextMenu);
}

WidgetFile::~WidgetFile()
{
    delete ui;
}

void WidgetFile::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // Target aspect ratio: width / height = 4 / 5
    const float aspectRatio = 4.0f / 5.0f;

    int newWidth = event->size().width();
    int newHeight = event->size().height();

    // Adjust dimensions to maintain aspect ratio
    if (newWidth / static_cast<float>(newHeight) > aspectRatio) {
        newWidth = static_cast<int>(newHeight * aspectRatio);
    } else {
        newHeight = static_cast<int>(newWidth / aspectRatio);
    }

    resize(newWidth, newHeight);
}

void WidgetFile::setIcon(const QPixmap& pixmap)
{
    ui->fileIcon->setPixmap(pixmap);
}

void WidgetFile::setTitle(const std::string& title)
{
    ui->fileName->setText(QString::fromStdString(title));
}

void WidgetFile::setFileType(const gea::FileType& fileType)
{
    ui->fileType->setText(QString::fromStdString(std::string(magic_enum::enum_name(fileType))));
}

void WidgetFile::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {

    }

    if (event->buttons() & Qt::RightButton)
    {

    }
}

void WidgetFile::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {

    }
}

void WidgetFile::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Context Menu"), this);

    QAction renameAction("Rename", this);
    connect(&renameAction, &QAction::triggered, this, &WidgetFile::renameFileDialog);

    QAction deleteAction("Delete", this);
    connect(&deleteAction, &QAction::triggered, this, []()
    {

    });

    contextMenu.addAction(&renameAction);
    contextMenu.addAction(&deleteAction);

    contextMenu.exec(mapToGlobal(pos));
}

void WidgetFile::renameFileDialog()
{
    QString currentFileName = ui->fileName->text();
    QString newFileName = QInputDialog::getText(this, tr("Rename File"), tr("New name: "), QLineEdit::Normal, currentFileName);

    if (!newFileName.isEmpty() && newFileName != currentFileName)
    {
        QFile file(currentFileName);
        QString newPath = QFileInfo(file).absolutePath() + "/" + newFileName;

        if (file.rename(newPath))
        {
            QMessageBox::information(this, tr("Success"), tr("File renamed successfully!"));
        }
        else
        {
            QMessageBox::warning(this, tr("Error"), tr("Failed to rename file."));
        }
    }
}
