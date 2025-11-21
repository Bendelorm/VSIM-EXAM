#include "../core.h"

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "../Renderer.h"
#include <QKeyEvent>
#include <QtConcurrent/QtConcurrent>
#include "../SoundTester.h"
#include <QFileDialog>
#include <QImageReader>
#include <QMimeDatabase>
#include <qlistwidget.h>
#include "../Widgets/widgetfile.h"
#include <QDoubleSpinBox>
#include "../Systems/PhysicsSystem.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //MainWindow size:
    resize(1300, 850);

    // Trying to show data about files imported to the engine
    UpdateFileView();

    //Entities dock
    setupEntitiesDock();

    setWindowTitle("INNgine Omega Ultra Insane Version");  //Main app title

    //Create window, ready to receive renderer
    renderSurface =new Renderer();
    renderSurface->setSurfaceType(QSurface::VulkanSurface);
    renderSurface->resize(1100,700);

    // Wrap VulkanRenderer (QWindow) into a QWidget
    QWidget* vulkanWidget = QWidget::createWindowContainer(renderSurface, this);
    vulkanWidget->setMinimumSize(1100, 700);

    ui->VulkanLayout->addWidget(vulkanWidget);

    //sets the keyboard input focus to the MainWindow when program starts
    this->setFocus();
   /*NOTE: Add resetting of consolelog
    *  connect(ui->actionToogle_Console_Log, &QAction::triggered,
            this, &MainWindow::shitButton);*/


    mEngine = std::make_unique<gea::EngineInit>();
    //mEngine.PostInitalizeEngineInitalization(renderSurface)
    //Initalize logger
    //NOTE: Could this be in engine class instead of here?
    logger::instance().initalize(ui->ConsoleDock);

    fileSystem.FilesContentUpdated = [&](){UpdateFileView();};
}

MainWindow::~MainWindow()
{
    /*
    if(mVulkanWindow)
    {
        delete mVulkanWindow;
        mVulkanWindow = nullptr;
    }*/
    delete ui;
}

void MainWindow::start()
{
    qDebug("Start is called");

    fileSystem.ReadInFilesFromContentDirectory();

    //mVulkanWindow->requestUpdate();
    mEngine->PostInitalizeEngineInitalization(renderSurface);

    refreshEntitiesList();

}

void MainWindow::setupEntitiesDock()
{
    refreshEntitiesList();
}

void MainWindow::refreshEntitiesList()
{
    ui->EntitiesList->clear();

    for (auto& [entityID, transform] : gea::EngineInit::registry.Transforms)
    {
        QString entityName;

        // Use the name from the transform component
        if (!transform.name.empty())
        {
            entityName = QString::fromStdString(transform.name);
        }
        else
        {
            entityName = QString("Entity %1").arg(entityID);
        }

        QListWidgetItem* item = new QListWidgetItem(entityName);
        item->setData(Qt::UserRole, entityID);
        ui->EntitiesList->addItem(item);
    }
}
void MainWindow::on_EntitiesList_itemClicked(QListWidgetItem *item)
{
    currentSelectedEntityID = item->data(Qt::UserRole).toUInt();
    updateInspector(currentSelectedEntityID);
}

void MainWindow::updateInspector(uint32_t entityID)
{
    ui->InspectorTree->clear();
    ui->InspectorTree->setHeaderHidden(false);
    ui->InspectorTree->setColumnCount(2);
    ui->InspectorTree->setHeaderLabels({"Property", "Value"});

    // Transform Component
    auto transformIt = gea::EngineInit::registry.Transforms.find(entityID);
    if (transformIt != gea::EngineInit::registry.Transforms.end())
    {
        QTreeWidgetItem* transformItem = new QTreeWidgetItem(ui->InspectorTree);
        transformItem->setText(0, "Transform");
        transformItem->setExpanded(true);

        // Position
        QTreeWidgetItem* posHeader = new QTreeWidgetItem(transformItem);
        posHeader->setText(0, "Position");

        // Position X
        QTreeWidgetItem* posX = new QTreeWidgetItem(posHeader);
        posX->setText(0, "X");
        QDoubleSpinBox* posXSpin = new QDoubleSpinBox();
        posXSpin->setRange(-10000, 10000);
        posXSpin->setValue(transformIt->second.mPosition.x);
        posXSpin->setProperty("entityID", entityID);
        posXSpin->setProperty("component", "position");
        posXSpin->setProperty("axis", "x");
        connect(posXSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &MainWindow::onTransformValueChanged);
        ui->InspectorTree->setItemWidget(posX, 1, posXSpin);

        // Position Y
        QTreeWidgetItem* posY = new QTreeWidgetItem(posHeader);
        posY->setText(0, "Y");
        QDoubleSpinBox* posYSpin = new QDoubleSpinBox();
        posYSpin->setRange(-10000, 10000);
        posYSpin->setValue(transformIt->second.mPosition.y);
        posYSpin->setProperty("entityID", entityID);
        posYSpin->setProperty("component", "position");
        posYSpin->setProperty("axis", "y");
        connect(posYSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &MainWindow::onTransformValueChanged);
        ui->InspectorTree->setItemWidget(posY, 1, posYSpin);

        // Position Z
        QTreeWidgetItem* posZ = new QTreeWidgetItem(posHeader);
        posZ->setText(0, "Z");
        QDoubleSpinBox* posZSpin = new QDoubleSpinBox();
        posZSpin->setRange(-10000, 10000);
        posZSpin->setValue(transformIt->second.mPosition.z);
        posZSpin->setProperty("entityID", entityID);
        posZSpin->setProperty("component", "position");
        posZSpin->setProperty("axis", "z");
        connect(posZSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &MainWindow::onTransformValueChanged);
        ui->InspectorTree->setItemWidget(posZ, 1, posZSpin);

        // Rotation
        QTreeWidgetItem* rotHeader = new QTreeWidgetItem(transformItem);
        rotHeader->setText(0, "Rotation");

        // Rotation X
        QTreeWidgetItem* rotX = new QTreeWidgetItem(rotHeader);
        rotX->setText(0, "X");
        QDoubleSpinBox* rotXSpin = new QDoubleSpinBox();
        rotXSpin->setRange(-360, 360);
        rotXSpin->setValue(transformIt->second.mRotation.x);
        rotXSpin->setProperty("entityID", entityID);
        rotXSpin->setProperty("component", "rotation");
        rotXSpin->setProperty("axis", "x");
        connect(rotXSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &MainWindow::onTransformValueChanged);
        ui->InspectorTree->setItemWidget(rotX, 1, rotXSpin);

        // Rotation Y
        QTreeWidgetItem* rotY = new QTreeWidgetItem(rotHeader);
        rotY->setText(0, "Y");
        QDoubleSpinBox* rotYSpin = new QDoubleSpinBox();
        rotYSpin->setRange(-360, 360);
        rotYSpin->setValue(transformIt->second.mRotation.y);
        rotYSpin->setProperty("entityID", entityID);
        rotYSpin->setProperty("component", "rotation");
        rotYSpin->setProperty("axis", "y");
        connect(rotYSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &MainWindow::onTransformValueChanged);
        ui->InspectorTree->setItemWidget(rotY, 1, rotYSpin);

        // Rotation Z
        QTreeWidgetItem* rotZ = new QTreeWidgetItem(rotHeader);
        rotZ->setText(0, "Z");
        QDoubleSpinBox* rotZSpin = new QDoubleSpinBox();
        rotZSpin->setRange(-360, 360);
        rotZSpin->setValue(transformIt->second.mRotation.z);
        rotZSpin->setProperty("entityID", entityID);
        rotZSpin->setProperty("component", "rotation");
        rotZSpin->setProperty("axis", "z");
        connect(rotZSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &MainWindow::onTransformValueChanged);
        ui->InspectorTree->setItemWidget(rotZ, 1, rotZSpin);

        // Scale
        QTreeWidgetItem* scaleHeader = new QTreeWidgetItem(transformItem);
        scaleHeader->setText(0, "Scale");

        // Scale X
        QTreeWidgetItem* scaleX = new QTreeWidgetItem(scaleHeader);
        scaleX->setText(0, "X");
        QDoubleSpinBox* scaleXSpin = new QDoubleSpinBox();
        scaleXSpin->setRange(0.001, 10000);
        scaleXSpin->setValue(transformIt->second.mScale.x);
        scaleXSpin->setProperty("entityID", entityID);
        scaleXSpin->setProperty("component", "scale");
        scaleXSpin->setProperty("axis", "x");
        connect(scaleXSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &MainWindow::onTransformValueChanged);
        ui->InspectorTree->setItemWidget(scaleX, 1, scaleXSpin);

        // Scale Y
        QTreeWidgetItem* scaleY = new QTreeWidgetItem(scaleHeader);
        scaleY->setText(0, "Y");
        QDoubleSpinBox* scaleYSpin = new QDoubleSpinBox();
        scaleYSpin->setRange(0.001, 10000);
        scaleYSpin->setValue(transformIt->second.mScale.y);
        scaleYSpin->setProperty("entityID", entityID);
        scaleYSpin->setProperty("component", "scale");
        scaleYSpin->setProperty("axis", "y");
        connect(scaleYSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &MainWindow::onTransformValueChanged);
        ui->InspectorTree->setItemWidget(scaleY, 1, scaleYSpin);

        // Scale Z
        QTreeWidgetItem* scaleZ = new QTreeWidgetItem(scaleHeader);
        scaleZ->setText(0, "Z");
        QDoubleSpinBox* scaleZSpin = new QDoubleSpinBox();
        scaleZSpin->setRange(0.001, 10000);
        scaleZSpin->setValue(transformIt->second.mScale.z);
        scaleZSpin->setProperty("entityID", entityID);
        scaleZSpin->setProperty("component", "scale");
        scaleZSpin->setProperty("axis", "z");
        connect(scaleZSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &MainWindow::onTransformValueChanged);
        ui->InspectorTree->setItemWidget(scaleZ, 1, scaleZSpin);
    }

    // Mesh Component (keep as read-only for now)
    auto meshIt = gea::EngineInit::registry.Meshes.find(entityID);
    if (meshIt != gea::EngineInit::registry.Meshes.end())
    {
        QTreeWidgetItem* meshItem = new QTreeWidgetItem(ui->InspectorTree);
        meshItem->setText(0, "Mesh");

        QTreeWidgetItem* pathItem = new QTreeWidgetItem(meshItem);
        pathItem->setText(0, "Path");
        pathItem->setText(1, QString::fromStdString(meshIt->second.path));

        meshItem->setExpanded(true);
    }

    // Texture Component (keep as read-only for now)
    auto textureIt = gea::EngineInit::registry.Textures.find(entityID);
    if (textureIt != gea::EngineInit::registry.Textures.end())
    {
        QTreeWidgetItem* textureItem = new QTreeWidgetItem(ui->InspectorTree);
        textureItem->setText(0, "Texture");

        QTreeWidgetItem* pathItem = new QTreeWidgetItem(textureItem);
        pathItem->setText(0, "Path");
        pathItem->setText(1, QString::fromStdString(textureIt->second.path));

        textureItem->setExpanded(true);
    }
}

void MainWindow::onTransformValueChanged()
{
    QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    if (!spinBox) return;

    uint32_t entityID = spinBox->property("entityID").toUInt();
    QString component = spinBox->property("component").toString();
    QString axis = spinBox->property("axis").toString();
    float value = spinBox->value();

    if (component == "position")
    {
        glm::vec3 pos = gea::TransformManager::getPosition(entityID);
        if (axis == "x") pos.x = value;
        else if (axis == "y") pos.y = value;
        else if (axis == "z") pos.z = value;
        gea::TransformManager::setPosition(entityID, pos);
    }
    else if (component == "rotation")
    {
        glm::vec3 rot = gea::TransformManager::getRotation(entityID);
        if (axis == "x") rot.x = value;
        else if (axis == "y") rot.y = value;
        else if (axis == "z") rot.z = value;
        gea::TransformManager::setRotation(entityID, rot);
    }
    else if (component == "scale")
    {
        glm::vec3 scale = gea::TransformManager::getScale(entityID);
        if (axis == "x") scale.x = value;
        else if (axis == "y") scale.y = value;
        else if (axis == "z") scale.z = value;
        gea::TransformManager::setScale(entityID, scale);
    }
}
void MainWindow::UpdateFileView()
{
    QLog("Updating File View...", "grey");

    ui->tableWidget_filesData->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui->tableWidget_filesData->setRowCount(fileSystem.FileAmount());

    std::vector<gea::File*> fileData = fileSystem.GetAllFileData();

    QLog("File amount received: " + QString::number(fileData.size()), "grey");

    QMimeDatabase mimeDataBase;
    QTableWidgetItem* item;
    QIcon icon;


    // Creating Tab 2 in main window
    for (size_t i = 0; i < fileData.size(); i++)
    {
        ui->tableWidget_filesData->setItem(i, 0, new QTableWidgetItem(QString::fromUtf8(fileData[i]->mFileName)));
        ui->tableWidget_filesData->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(fileData[i]->mFilePath.string())));
        ui->tableWidget_filesData->setItem(i, 2, new QTableWidgetItem(QString::number(fileData[i]->mFileSize)));

        if (mimeDataBase.mimeTypeForFile(QString::fromStdString(fileData[i]->mFilePath.string())).name().startsWith("image/"))
        {
            icon =  QIcon(QString::fromStdString(fileData[i]->mFilePath.string()));
            item = new QTableWidgetItem();
            item->setIcon(icon);

            ui->tableWidget_filesData->setItem(i, 3, item);
        }

        QLog("Added file #" + QString::number(i) + " to the file view.", "grey");
    }

    //Creating Tab 3 in main window
    WidgetFile* fileWidget;
    QGridLayout* grid = ui->grid_filesData;

    //Adds all files to the fileGrid
    for (size_t i = 0; i < fileData.size(); i++)
    {
        fileWidget = new WidgetFile();
        fileWidget->setFixedSize(QSize(128, 160));
        fileWidget->setIcon(QString::fromStdString(mimeDataBase.mimeTypeForFile(QString::fromStdString(
                                                                                    fileData[i]->mFilePath.string())).name().startsWith("image/") ?
                                                       fileData[i]->mFilePath.string() : fileSystem.getFileIcon(fileData[i]->mFileType).string()));
        fileWidget->setTitle(fileData[i]->mFileName);
        fileWidget->setFileType(fileData[i]->mFileType);
        grid->addWidget(fileWidget, i / fileGridColumnMaxCount, i % fileGridColumnMaxCount);
    }
}

void MainWindow::on_actionPlay_triggered()
{
    //TODO: MainWindow::on_actionPlay_triggered() - Add logic for play and editor mode
    //ui->consoleOutput->append("Start play mode");
    QLog("Start play", "green");
}


void MainWindow::on_actionStopPlay_triggered()
{
    //TODO: MainWindow::on_actionStopPlay_triggered() - Add logic for play and editor mode
    //ui->consoleOutput->append("Return to editor");
    FLog("Return to editor",logType::LogTemp);
}



void MainWindow::on_actionToogle_Console_Log_triggered()
{
    ui->ConsoleDock->toggleViewAction()->trigger();
}

void MainWindow::on_actionPlaySound_triggered()
{
    QtConcurrent::run([](){
        testOpenAL();
    });
}

void MainWindow::on_actionAdd_FIle_triggered()
{
    QLog("Trying to add new file", "yellow");
    QFileInfo fileInfo(QFileDialog::getOpenFileName(nullptr, "Open File", ""));
    auto re = fileSystem.AddFile(fileInfo.fileName().toStdString(), fileInfo.absoluteFilePath().toStdString());

    QLog("Added file " + fileInfo.fileName() + " to the project! Did it succeed? " + QString::fromStdString(std::to_string(re)), "green");

    UpdateFileView(); //TODO: MainWindow::on_actionAdd_FIle_triggered() - Should be changed to only add the new file to the file view, instead of updating the entire view from scratch
}
void MainWindow::on_actionPhysicsToggle_triggered(bool checked)
{
    gea::PhysicsSystem::setEnabled(checked);
}
