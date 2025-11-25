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
std::unique_ptr<PhysicsSystem> EngineInit::PhysicsSystem{nullptr};
Renderer* EngineInit::rendererPointer = nullptr;

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

    rendererPointer = renderSurface;

    // initialize systems
    RenderSystem = std::make_unique<gea::RenderSystem>();
    CameraSystem = std::make_unique<gea::CameraSystem>();
    InputSystem  = std::make_unique<gea::InputSystem>();
    PhysicsSystem = std::make_unique<gea::PhysicsSystem>();

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

    //TxtToObj::convertTxtToObj("../../las/lasdata2_downsample1000.txt", "../../las/lasdatafull2_downsample1000_triangles.obj" );
    //TxtToObj converter;
    //if (converter.loadPointsFromTxt("../../las/lasdata4_downsample10.txt", 1))
    //{
    //    converter.createRegularGrid(5.0);
    //    converter.generateTriangulation();
    //    converter.calculateNormals();
    //    converter.generateUVCoordinates();
    //
    //    if (!converter.exportToOBJ("../../las/lasdatafull4_downsample10.obj"))
    //    {
    //        qDebug() << "Failed to export mesh!";
    //    }
    //}
    //else
    //{
    //    qDebug() << "Failed to load point cloud!";
    //}
    // Transform/mesh/texture entities
    // gea::Entity RoomOne = entityManager.createEntity();
    // gea::Transform transform1;
    // transform1.name = "VikingRoom";
    // gea::Mesh mesh1;
    // mesh1.path = "../../Assets/Models/viking_room.obj";
    // gea::Texture texture1;
    // texture1.path = "../../Assets/Textures/viking_room.png";

    //connect components to mesh
    //registry.addComponent(RoomOne.mEntityID, transform1);
    //registry.addComponent(RoomOne.mEntityID, mesh1);
    //registry.addComponent(RoomOne.mEntityID, texture1);

    // OBSTACLE
    gea::Entity Obstacle = entityManager.createEntity();
    gea::Transform ObstacleTransform;
    ObstacleTransform.mPosition = glm::vec3(-9.0f, -7.0f, 26.0f);  // adjust as needed
    ObstacleTransform.mRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    ObstacleTransform.mScale    = glm::vec3(4.0f, 4.0f, 4.0f);  // cube size
    ObstacleTransform.name      = "Obstacle";
    ObstacleTransform.isObstacle = true;


    // assumes you have a cube model
    gea::Mesh ObstacleMesh;
    ObstacleMesh.path = "../../Assets/Models/Cube.obj";

    gea::Texture ObstacleTex;
    ObstacleTex.path = "../../Assets/Textures/viking_room.png";


    registry.addComponent(Obstacle.mEntityID, ObstacleTransform);
    registry.addComponent(Obstacle.mEntityID, ObstacleMesh);
    registry.addComponent(Obstacle.mEntityID, ObstacleTex);

    //gea::Entity Ball = entityManager.createEntity();
    //gea::Transform BallTransform;
    //BallTransform.mPosition = glm::vec3(-12,2,-17);
    //BallTransform.name = "Ball";
    //gea::Mesh BallMesh;
    //BallMesh.path = "../../Assets/Models/Sphere.obj";
    //gea::Texture BallTexture;
    //BallTexture.path = "../../Assets/Textures/texture.jpg";
    //
    //gea::Physics ballPhysics;
    //ballPhysics.velocity = glm::vec3(0.0f);
    //ballPhysics.acceleration = glm::vec3(0.0f);
    //ballPhysics.mass = 20.0f;
    //ballPhysics.radius = 0.2f; // adjust to mesh
    //
    //registry.addComponent(Ball.mEntityID, BallTransform);
    //registry.addComponent(Ball.mEntityID, BallMesh);
    //registry.addComponent(Ball.mEntityID, BallTexture);
    //registry.addComponent(Ball.mEntityID, ballPhysics);
    //gea::TransformManager::setScale(Ball.mEntityID, glm::vec3(0.2, 0.2, 0.2));

    // FLUID-LIKE: many balls
    const int   numBalls      = 1000; // how many balls
    const float spawnInterval = 0.3f; // time between launches (seconds)
    const glm::vec3 spawnPos  = glm::vec3(-12,2,-17); // same origin for all
    for (int i = 0; i < numBalls; ++i)
    {
        gea::Entity Ball = entityManager.createEntity();

        gea::Transform BallTransform;
        BallTransform.mPosition = spawnPos;
        BallTransform.name = "Ball_" + std::to_string(i);
        BallTransform.mScale = glm::vec3(0.2, 0.2, 0.2);

        gea::Mesh BallMesh;
        BallMesh.path = "../../Assets/Models/Sphere.obj";

        gea::Texture BallTexture;
        BallTexture.path = "../../Assets/Textures/texture.jpg";

        gea::Physics ballPhysics;
        ballPhysics.velocity      = glm::vec3(0.0f);
        ballPhysics.acceleration  = glm::vec3(0.0f);
        ballPhysics.mass          = 100.0f;
        ballPhysics.radius        = 0.2f;
        ballPhysics.currentTriIndex = -1;
        ballPhysics.orientation   = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

        ballPhysics.active     = false;
        ballPhysics.spawnDelay = i * spawnInterval;

        registry.addComponent(Ball.mEntityID, BallTransform);
        registry.addComponent(Ball.mEntityID, BallMesh);
        registry.addComponent(Ball.mEntityID, BallTexture);
        registry.addComponent(Ball.mEntityID, ballPhysics);
    }


    //CREATING TERRAIN
    gea::Entity Terrain = entityManager.createEntity();
    gea::Transform TerrainTransform;
    TerrainTransform.mPosition = glm::vec3(0,0,0);
    TerrainTransform.name = "Terrain";
    TerrainTransform.isTerrain = true;
    gea::Mesh TerrainMesh;
    TerrainMesh.path = "../../las/lasdatafull4_downsample10.obj";
    gea::Texture TerrainTexture;
    TerrainTexture.path = "../../Assets/Textures/texture.jpg";

    registry.addComponent(Terrain.mEntityID, TerrainTransform);
    registry.addComponent(Terrain.mEntityID, TerrainMesh);
    registry.addComponent(Terrain.mEntityID, TerrainTexture);




    gea::TransformManager::setRotation(Terrain.mEntityID, glm::vec3(-90, 0, 0));
    gea::TransformManager::setPosition(Terrain.mEntityID, glm::vec3(0, -2, 0));
    gea::TransformManager::setScale(Terrain.mEntityID, glm::vec3(0.1, 0.1, 0.1));

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
    PhysicsSystem->update(deltaTime, rendererPointer);

    InputSystem->update(deltaTime);
}
} // End of namespace gea
