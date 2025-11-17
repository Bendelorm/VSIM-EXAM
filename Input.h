#ifndef INPUT_H
#define INPUT_H

namespace gea
{

///
/// @brief All valid states an Input can have, like PRESSED or RELEASED
///
enum class ButtonState
{
    NONE,
    PRESSED,
    RELEASED,
    HELD
};

///
/// @brief Hardcoded Input Variables, should be refactored to let users map their own Input in the editor.
///
struct Inputlist
{
    //TODO: Input - Refactor Input struct, allowing the user to add their own key bindings to a map, where both the key (arbitrary button name) and the value (which button) are unique.

    ButtonState w = ButtonState::NONE;
    ButtonState a = ButtonState::NONE;
    ButtonState s = ButtonState::NONE;
    ButtonState d = ButtonState::NONE;
    ButtonState esc = ButtonState::NONE;
    ButtonState lmb = ButtonState::NONE;
    ButtonState rmb = ButtonState::NONE;
};

} //namespace gea






#endif // INPUT_H

