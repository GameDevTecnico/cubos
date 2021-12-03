#ifndef CUBOS_INPUT_INPUT_MANAGER_HPP
#define CUBOS_INPUT_INPUT_MANAGER_HPP

#include <cubos/event.hpp>
#include <cubos/io/window.hpp>
#include <map>
#include <string>
#include <memory>
#include <variant>

#include <glm/glm.hpp>

// INPUT MANAGER
//   - MAPPINGS (EVENTS (e.g. keys, mouseclicks, gamepads, joysticks, etc..) -> ACTIONS (e.g. Fire, Shoot, Jumpt, Move))

namespace cubos::input
{
    class Binding;

    class InputContext
    {
    };

    struct ButtonBindingDesc
    {
        enum ButtonType
        {
            Key,
            Mouse
        };

        ButtonType type;
        std::variant<cubos::io::Key, cubos::io::MouseButton> button;
    };

    class ActionMapping : public std::enable_shared_from_this<ActionMapping>
    {
    public:
        ActionMapping(const std::string& actionId);
        void RegisterBinding(ButtonBindingDesc bind);
        void SubscribeAction(std::function<void(InputContext)> function);

    private:
        Event<InputContext> onTrigger;
    };

    class InputManager
    {
    public:
        static void Init(cubos::io::Window* window);
        static std::shared_ptr<ActionMapping> CreateActionMapping(const std::string& name);
        static std::shared_ptr<ActionMapping> GetActionMapping(const std::string& name);
        static void RegisterKeyBinding(cubos::io::Key key, std::shared_ptr<Binding> binding);
        static void RegisterMouseBinding(cubos::io::MouseButton mouse, std::shared_ptr<Binding> binding);

    private:
        static cubos::io::Window* window;
        static std::map<std::string, std::shared_ptr<ActionMapping>> mappings;
        static std::map<cubos::io::Key, std::vector<std::shared_ptr<Binding>>> keyBindings;
        static std::map<cubos::io::MouseButton, std::vector<std::shared_ptr<Binding>>> mouseButtonsBindings;

        friend class ActionMapping;
    };
} // namespace cubos::input

#endif // CUBOS_INPUT_INPUT_MANAGER_HPP