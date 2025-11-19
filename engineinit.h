#ifndef ENGINEINIT_H
#define ENGINEINIT_H

#include <filesystem>
#include <QDir>
#include "TransformManager.h"
#include "qwindow.h"
#include "Systems/RenderSystem.h"
#include "Systems/CameraSystem.h"
#include "entityManager.h"
#include "registry.h"
#include "macros.h"
#include "Systems/inputsystem.h"
#include "TxtToObj.h"

namespace gea
{

class EngineInit
{

public:
    static QElapsedTimer timer;
    static float lastTime;
    static float deltaTime;
    static float currentTime;


    //Systems
    static std::unique_ptr<gea::RenderSystem> RenderSystem;
    static std::unique_ptr<gea::CameraSystem> CameraSystem;
    static std::unique_ptr<InputSystem> InputSystem;


    static QString mat4ToQString(const glm::mat4& mat);

    static void update();
    inline static entityManager entityManager; // this is where the mandems lay
    inline static registry registry; // component manager

    static std::filesystem::path rootPath;
    static QDir mEngineContentDirectory;
    static QDir mEngineAssetsDirectory;
    static std::filesystem::path mEngineContentDirectoryPath;
    static std::filesystem::path mEngineAssetsDirectoryPath;

    static std::filesystem::path splashArtPath;

    static void PreInitializeEngine();
    static void InitializeEngine();
    static void PostInitalizeEngineInitalization(Renderer* renderSurface); // To avoid messing with file code while i try this out


    };

} // End of namespace gea

#endif // ENGINEINIT_H
