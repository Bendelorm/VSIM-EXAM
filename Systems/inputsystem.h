#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "System.h"
#include <unordered_map>
#include <vector>
#include <QKeyEvent>
#include <QEvent>
#include "../Input.h"
namespace gea
{

    //NOTE: Testing a different approach that allows bindings
enum class InputType
{
    KeyboardButton,
    GamepadButton,
    GamepadAxis,
    MouseButton,
    MouseButtonAxis,
};


enum class Action
{
    MoveForward,
    MoveBackwards,
    MoveLeft,
    MoveRight,
    MouseX,
    MouseY,
    MouseRightButton
};

struct InputBinding
{
    InputType type;
    int inputCode;
    float treshold {0.0f}; // for gamepad
};


class InputSystem : public QObject, public gea::System
{
    Q_OBJECT
public:
    InputSystem(QObject *parent = nullptr);
    ~InputSystem() {};

    void update(float deltaTime);
    bool isActionActive();
    void createBinding(Action action, InputType type, int inputCode, float treshold = 0.0f);
    void changeBinding(Action action, InputType type, int inputcode, float treshold = 0.0f);
    void handleKeyPress(QKeyEvent* event);
    void handleKeyRelease(QKeyEvent* event);
    void handleMouseMovement(QMouseEvent* event);

    bool eventFilter(QObject *obj, QEvent *event) override;

    //input struct with every button
    Inputlist inputList{};

    //deltatime
    float deltaTimeBecauseTheRealDeltaTimeBeActingUp{1.0f};

private:
    std::unordered_map<Action, std::vector<InputBinding>> bindings;

    //TODO: fix this shitty way of doing things
    float mYaw{0.0f}, mPitch{0.0f}, mSpeed{100.0f};
    float oldMouseX{-1.0f}, oldMouseY{-1.0f};

signals:
    void actionPressed(Action action);
    void actionReleased(Action action);

private slots:
    void onActionPressed(Action action);
    void onActionReleased(Action action);
};
}
#endif // INPUTSYSTEM_H
