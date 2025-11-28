#include "engineinit.h"
#include <QRegularExpression>
#include "TxtToObj.h"

namespace gea
{

// Defining static variables

QElapsedTimer gea::EngineInit::timer{};
float gea::EngineInit::lastTime{0.0f};
float gea::EngineInit::deltaTime{0.0f};
float gea::EngineInit::currentTime{0.0f};

std::filesystem::path EngineInit::rootPath;
std::filesystem::path EngineInit::mEngineContentDirectoryPath;
std::filesystem::path EngineInit::mEngineAssetsDirectoryPath;
std::filesystem::path EngineInit::splashArtPath;


QDir EngineInit::mEngineContentDirectory;
QDir EngineInit::mEngineAssetsDirectory;

std::unique_ptr<RenderSystem> EngineInit::RenderSystem{nullptr};
std::unique_ptr<CameraSystem> EngineInit::CameraSystem{nullptr};
std::unique_ptr<InputSystem>  EngineInit::InputSystem{nullptr};

///
/// @brief The first thing that runs when starting the engine
///
void EngineInit::PreInitializeEngine()
{
    rootPath = std::filesystem::current_path();
    mEngineAssetsDirectoryPath = rootPath / "Engine/Assets";
    mEngineAssetsDirectory = QDir(mEngineAssetsDirectoryPath);

    if (!mEngineAssetsDirectory.exists()) mEngineAssetsDirectory.mkpath(".");

    QRegularExpression regex("^splash_art\\.[^\\.]+$");

    for (const QString &file : mEngineAssetsDirectory.entryList(QDir::Files))
    {
        if (regex.match(file).hasMatch())
        {
            splashArtPath = mEngineAssetsDirectory.absoluteFilePath(file).toStdString();
            break;
        }
    }
}

///
/// @brief Runs after the splash art, but before creating the MainWindow
///
void EngineInit::InitializeEngine()
{
    mEngineContentDirectoryPath = rootPath / "Content";
    mEngineContentDirectory = QDir(mEngineContentDirectoryPath);

    if (!mEngineContentDirectory.exists()) mEngineContentDirectory.mkpath(".");

}

void EngineInit::PostInitalizeEngineInitalization(Renderer* renderSurface)
{

    if (!timer.isValid())
        timer.start();

    // initialize systems
    RenderSystem = std::make_unique<gea::RenderSystem>();
    CameraSystem = std::make_unique<gea::CameraSystem>();
    InputSystem  = std::make_unique<gea::InputSystem>();

    // create camera entities
    gea::Entity CameraMan = entityManager.createEntity();

    //Create camera components
    gea::Camera cam1;
    cam1.isActive = true;

    //Connect components to entities CAMERA
    registry.addComponent(CameraMan.mEntityID,cam1);

    QLog(mat4ToQString(cam1.mViewMatrix),"blue");
    QLog(mat4ToQString(cam1.mProjectionMatrix),"blue");
    QLog(&" " [ cam1.isActive],"blue");


    //Transform/mesh/texture entities
    gea::Entity RoomOne = entityManager.createEntity();
    gea::Transform transform1;
    transform1.name = "VikingRoom";
    gea::Mesh mesh1;
    mesh1.path = "../../Assets/Models/viking_room.obj";
    gea::Texture texture1;
    texture1.path = "../../Assets/Textures/viking_room.png";




    gea::Entity RoomTwo = entityManager.createEntity();
    transform1.mPosition = glm::vec3(2,2,2);
    gea::Transform transform2;
    transform2.name = "Sphere";
    gea::Mesh mesh2;
    mesh2.path = "../../Assets/Models/Sphere.obj";
    gea::Texture texture2;
    texture2.path = "../../Assets/Textures/texture.jpg";


    //CREATING POINT CLOUD
    gea::Entity PointCloud = entityManager.createEntity();
    gea::Transform transform3;
    transform3.mPosition = glm::vec3(0,0,0);
    transform3.name = "PointCloud";
    gea::Mesh mesh3;
    mesh3.path = "../../las/lasdatafull2_downsampled.obj";
    gea::Texture texture3;
    texture3.path = "../../Assets/Textures/texture.jpg";
    transform3.mRotation = glm::vec3(-90, 0, 0);
    transform3.mScale = glm::vec3(0.1, 0.1, 0.1);


    //connect components to mesh
    registry.addComponent(RoomOne.mEntityID, transform1);
    registry.addComponent(RoomOne.mEntityID, mesh1);
    registry.addComponent(RoomOne.mEntityID, texture1);

    registry.addComponent(RoomTwo.mEntityID, transform2);
    registry.addComponent(RoomTwo.mEntityID, mesh2);
    registry.addComponent(RoomTwo.mEntityID, texture2);

    registry.addComponent(PointCloud.mEntityID, transform3);
    registry.addComponent(PointCloud.mEntityID, mesh3);
    registry.addComponent(PointCloud.mEntityID, texture3);


    //TxtToObj::convertTxtToObj("../../las/lasdata2_downsampled.txt", "../../las/lasdatafull2_downsampled.obj" );



    // this has to happen last
    RenderSystem->init(renderSurface);
}

QString EngineInit::mat4ToQString(const glm::mat4& mat) {
    QStringList rows;
    for (int i = 0; i < 4; ++i) {
        QStringList rowElements;
        for (int j = 0; j < 4; ++j) {
            rowElements << QString::number(mat[i][j], 'f', 3);
        }
        rows << rowElements.join("\t");
    }
    return rows.join("\n");
}

void EngineInit::update()
{
    //Getting time passed between frames
    qint64 currentTime = timer.nsecsElapsed(); // time in nanoseconds
    float deltaTime = (currentTime - lastTime) / 1e9f; // convert to seconds
    lastTime = currentTime;


    CameraSystem->Update();

    InputSystem->update(deltaTime);
}
} // End of namespace gea
