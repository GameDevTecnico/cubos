#ifndef CUBOS_INPUT_INPUT_MANAGER_HPP
#define CUBOS_INPUT_INPUT_MANAGER_HPP

#include <cubos/event.hpp>
#include <vector>
#include <string>

#include <glm/glm.hpp>

// INPUT MANAGER
//   - MAPPINGS (EVENTS (e.g. keys, mouseclicks, gamepads, joysticks, etc..) -> ACTIONS (e.g. Fire, Shoot, Jumpt, Move))

namespace cubos::input
{
    class InputContext
    {
    };

    class ActionMapping
    {
    public:
        ActionMapping(const std::string& action_id);
        void RegisterBidding();
        Event<InputContext> onTrigger;
    };

    class InputManager
    {
    public:
        std::vector<ActionMapping> mappings;
        void SubscribeToAction(const std::string& action_name, std::function<InputContext> function);
    };
} // namespace cubos::input

#endif // CUBOS_INPUT_INPUT_MANAGER_HPP
