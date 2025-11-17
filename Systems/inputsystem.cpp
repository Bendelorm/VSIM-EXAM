#include "inputsystem.h"
#include "../Input.h"
#include "../engineinit.h"
gea::InputSystem::InputSystem(QObject *parent) : QObject(parent)
{
    //Install eventfilters for input system
    qApp->installEventFilter(this);

    //Create bindings
    createBinding(Action::MoveForward, InputType::KeyboardButton, Qt::Key_W);
    createBinding(Action::MoveBackwards,InputType::KeyboardButton, Qt::Key_S);
    createBinding(Action::MoveLeft, InputType::KeyboardButton, Qt::Key_A);
    createBinding(Action::MoveRight, InputType::KeyboardButton, Qt::Key_D);
    createBinding(Action::MouseRightButton,InputType::MouseButton,Qt::RightButton);


    QPoint cursorPos = QCursor::pos();
    oldMouseX = cursorPos.x();
    oldMouseY = cursorPos.y();


    connect(this, &InputSystem::actionPressed, this, &InputSystem::onActionPressed);
    connect(this, &InputSystem::actionReleased, this, &InputSystem::onActionReleased);
}


void gea::InputSystem::update(float deltaTime)
{
    deltaTimeBecauseTheRealDeltaTimeBeActingUp = deltaTime;
    //qDebug("inputSystem updated! Nothing happened...");
    for(auto &[entityID,component] : gea::EngineInit::registry.Cameras)
    {

        if(inputList.w == ButtonState::PRESSED)
        {
            component.Position += component.Direction * component.mSpeed;
        }

        if(inputList.a == ButtonState::PRESSED)
        {
           component.Position -= component.Right * component.mSpeed;
        }

        if(inputList.s == ButtonState::PRESSED)
        {
            component.Position -= component.Direction * component.mSpeed;
        }

        if(inputList.d == ButtonState::PRESSED)
        {
            component.Position += component.Right * component.mSpeed;
        }
        //update camera  rotation
        component.mPitch = mPitch;
        component.mYaw   = mYaw;
    }
}


void gea::InputSystem::createBinding(Action action, InputType type, int inputcode, float treshold)
{
    bindings[action].push_back({type,inputcode,treshold});
}

void gea::InputSystem::changeBinding(Action action, InputType type, int inputcode, float treshold)
{
    for(InputBinding &binding : bindings[action])
         {
            if(binding.type == type)
            {
                binding.type = type;
                binding.inputCode = inputcode;
                binding.treshold = treshold;

                return;
            }
         }
    // If none are found the function continues to here.
    qDebug("Could not find correct binding to change");
         return;
}

void gea::InputSystem::handleKeyPress(QKeyEvent* event)
{
    for(auto it = bindings.begin(); it != bindings.end(); it++)
    {

        //Get the type of action and binding data
        const Action action = it->first;
        const std::vector<InputBinding> &bindingList = it->second;

        // check each binding and find the one thats for keyboard
        for(const InputBinding &binding : bindingList )
        {
            if(binding.type == InputType::KeyboardButton && event->key() == binding.inputCode)
            {
                //send it to another function for logic
                // emit is qt sugar
                emit actionPressed(action);
            }

        }
    }
}

void gea::InputSystem::handleKeyRelease(QKeyEvent* event)
{
    for(auto it = bindings.begin(); it != bindings.end(); it++)
    {

        //Get the type of action and binding data
        const Action action = it->first;
        const std::vector<InputBinding> &bindingList = it->second;

        // check each binding and find the one thats for keyboard
        for(const InputBinding &binding : bindingList )
        {
            if(binding.type == InputType::KeyboardButton && event->key() == binding.inputCode)
            {
                //send it to another function for logic
                // emit is qt sugar
                emit actionReleased(action);
            }
        }


    }
}


//Logic for what happens when each action is peformed
void gea::InputSystem::onActionPressed(Action action)
{

    switch(action)
    {
        case Action::MoveForward:
           inputList.w = gea::ButtonState::PRESSED;
            break;
        case Action::MoveBackwards:
            inputList.s = gea::ButtonState::PRESSED;
            break;
        case Action::MoveLeft:
            inputList.a = gea::ButtonState::PRESSED;
            break;
        case Action::MoveRight:
            inputList.d = gea::ButtonState::PRESSED;
            break;
    }
}

void gea::InputSystem::onActionReleased(Action action)
{
    switch(action)
    {
    case Action::MoveForward:
        inputList.w = gea::ButtonState::RELEASED;
        break;
    case Action::MoveBackwards:
        inputList.s =gea::ButtonState::RELEASED;
        break;
    case Action::MoveLeft:
        inputList.a = gea::ButtonState::RELEASED;
        break;
    case Action::MoveRight:
        inputList.d = gea::ButtonState::RELEASED;
        break;
    case Action::MouseRightButton:
        break;
    }

}

void gea::InputSystem::handleMouseMovement(QMouseEvent* event)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    if(inputList.rmb == gea::ButtonState::PRESSED)
    {
        // Using the difference in mouse movement to adjust yaw and pitch
        float dx = mouseEvent->pos().x() - oldMouseX;
        float dy = mouseEvent->pos().y() - oldMouseY;

        mYaw    += dx * mSpeed * deltaTimeBecauseTheRealDeltaTimeBeActingUp;

        mPitch  -= dy * mSpeed * deltaTimeBecauseTheRealDeltaTimeBeActingUp;
        // clamp pitch to avoid flipping
        if (mPitch > 89.0f) mPitch = 89.0f;
        if (mPitch < -89.0f) mPitch = -89.0f;


    }
    oldMouseX = mouseEvent->pos().x();
    oldMouseY = mouseEvent->pos().y();


}

bool gea::InputSystem::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        handleKeyPress(static_cast<QKeyEvent*>(event));
    }
    if (event->type() == QEvent::KeyRelease)
    {
        handleKeyRelease(static_cast<QKeyEvent*>(event));
    }

    if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent != nullptr)
        {
            if(mouseEvent->button() == Qt::RightButton)  // Changed to button()
                inputList.rmb = gea::ButtonState::PRESSED;
            handleMouseMovement(mouseEvent);
        }
    }

    if(event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent != nullptr)
        {
            if(mouseEvent->button() == Qt::RightButton)  // Changed to button()
                inputList.rmb = gea::ButtonState::RELEASED;
            handleMouseMovement(mouseEvent);
        }
    }

    // ADD THIS - handle continuous mouse movement
    if(event->type() == QEvent::MouseMove)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent != nullptr)
        {
            handleMouseMovement(mouseEvent);
        }
    }

    return QObject::eventFilter(obj, event);
}
