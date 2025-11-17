#ifndef SYSTEM_H
#define SYSTEM_H

//Need namespace, since we start to get naming collisions with other code
namespace gea
{

class System
{
public:
    virtual ~System() = default;     // virtual dtor: safe deletion

};

} //Namespace gea

#endif // SYSTEM_H
