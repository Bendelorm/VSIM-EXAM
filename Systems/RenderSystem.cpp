#include "RenderSystem.h"
#include "../core.h"

namespace gea
{

void RenderSystem::init(Renderer*  targetWindow)
{
    QLog("Creating a renderer","white");

    targetWindow->setTitle("Renderer");
    targetWindow->setWidth(1100);
    targetWindow->setHeight(700);
    targetWindow->initVulkan();

    if(mRenderer)
        QLog("renderer created", "white");
    //Trigger first frame
    targetWindow->requestUpdate();
}

} // End of namespace gea
