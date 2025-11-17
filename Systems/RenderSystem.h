#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "System.h"
#include "../Renderer.h"

namespace gea
{

class RenderSystem : public System
{
public:
    RenderSystem() {};
    ~RenderSystem() {};
    void init(Renderer* targetWindow);
    void render();
    Renderer *getRenderer() const{return mRenderer.get();}

private:
    std::unique_ptr<Renderer> mRenderer;
};

} // End of namespace gea

#endif // RENDERSYSTEM_H
