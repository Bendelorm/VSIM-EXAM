#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qlistwidget.h>
#include "../Systems/System.h"
#include "../Systems/FileSystem.h"
#include "../engineinit.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void start();

    void UpdateFileView();

    //TODO: MainWindow.h - MOVE SOMEWHERE SMARTER
    gea::FileSystem fileSystem{};

private:
    Ui::MainWindow *ui;
    Renderer* renderSurface;
    //QWidget* mRenderWindowContainer{nullptr};   //Qt container for the RenderWindow
    //class Renderer* mVulkanWindow{nullptr};
     std::unique_ptr<gea::EngineInit> mEngine;

    int fileGridColumnMaxCount = 5;

    void setupEntitiesDock();
    void refreshEntitiesList();

    void updateInspector(uint32_t entityID);
    uint32_t currentSelectedEntityID = 0;
private slots:
    void on_actionPhysicsToggle_triggered(bool checked);
    void on_EntitiesList_itemClicked(QListWidgetItem *item);
    void onTransformValueChanged();
    void on_actionPlay_triggered();
    void on_actionStopPlay_triggered();
    void on_actionToogle_Console_Log_triggered();
    void on_actionPlaySound_triggered();
    void on_actionAdd_FIle_triggered();
};
#endif // MAINWINDOW_H
