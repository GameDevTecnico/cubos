#ifndef CUBOS_INPUT_INPUT_MANAGER_HPP
#define CUBOS_INPUT_INPUT_MANAGER_HPP

#include <cubos/event.hpp>
#include <cubos/io/window.hpp>
#include <cubos/io/keyboard.hpp>
#include <map>
#include <string>
#include <memory>
#include <variant>

#include <glm/glm.hpp>

// INPUT MANAGER
//   - MAPPINGS (EVENTS (e.g. keys, mouseclicks, gamepads, joysticks, etc..) -> ACTIONS (e.g. Fire, Shoot, Jumpt, Move))

namespace cubos::input
{

    // class Binding;

    class InputContext
    {
    };

    /*
    struct ButtonBindingDesc
    {
        enum ButtonType
        {
            Key,
            Mouse
        };

        ButtonType type;
        std::variant<cubos::io::Key, cubos::io::MouseButton> button;
    };*/
    /*
    class ActionMapping : public std::enable_shared_from_this<ActionMapping>
    {
    public:
        ActionMapping(const std::string& actionId);
        void RegisterBinding(ButtonBindingDesc bind);
        void SubscribeAction(std::function<void(InputContext)> function);

    private:
        Event<InputContext> onTrigger;
        std::vector<std::shared_ptr<Binding>> bindings;
    };*/

    class InputSource
    {
    public:
        virtual void SubscribeEvents(cubos::io::Window* window) = 0;
        virtual void UnsubscribeEvents(cubos::io::Window* window) = 0;
        virtual bool isTriggered() = 0;
    };

    class ButtonPress : public InputSource
    {
    public:
        std::variant<cubos::io::Key, cubos::io::MouseButton> button;
        ButtonPress(cubos::io::Key key)
        {
            button = key;
        }
        ButtonPress(cubos::io::MouseButton button)
        {
            this->button = button;
        }

        bool isTriggered();
        void SubscribeEvents(cubos::io::Window* window);
        void UnsubscribeEvents(cubos::io::Window* window);

    private:
        bool wasTriggered = false;
        void handleKeyUp(std::variant<cubos::io::Key, cubos::io::MouseButton>);
        void handleKeyDown(cubos::io::Key key);
    };

    class InputAction
    {
    public:
        std::list<InputSource*> inputSources;
        std::list<std::function<void(InputContext)>> functionBindings;
        std::string name;
        bool enable;

        InputAction(std::string actionName)
        {
            name = actionName;
        }
        void AddInput(InputSource* source);
        void AddBinding(std::function<void(InputContext)> binding);
        void ProcessSources();
    };

    class InputManager
    {
    public:
        static cubos::io::Window* window;
        static std::map<std::string, std::shared_ptr<InputAction>> actions;

        static void Init(cubos::io::Window* window);
        static std::shared_ptr<InputAction> CreateAction(std::string name);
        static std::shared_ptr<InputAction> GetAction(std::string name);
        static void ProcessActions();

        static void handleKeyDown(cubos::io::Key key);
        /*
        static void Init(cubos::io::Window* window);
        static std::shared_ptr<ActionMapping> CreateActionMapping(const std::string& name);
        static std::shared_ptr<ActionMapping> GetActionMapping(const std::string& name);
        static void RegisterKeyBinding(cubos::io::Key key, std::function<void(void)> callback);
        static void RegisterMouseBinding(cubos::io::MouseButton mouse, std::function<void(void)> callback);
        static void RegisterAxisBinding(cubos::io::Key keyPos, cubos::io::Key keyNeg,
                                        std::function<void(float)> callback);
    private:
        static cubos::io::Window* window;
        static std::map<std::string, std::shared_ptr<ActionMapping>> mappings;
        static std::map<cubos::io::Key, std::vector<std::function<void(void)>>> keyBindings;
        static std::map<cubos::io::MouseButton, std::vector<std::function<void(void)>>> mouseButtonsBindings;

        static void handleKeyDown(cubos::io::Key key);
        static void handleKeyUp(cubos::io::Key key);
        static void handleMouseDown(cubos::io::MouseButton mouseButton);
        static void handleMouseUp(cubos::io::MouseButton mouseButton);
        friend class ActionMapping;
    */
    };
} // namespace cubos::input

#endif // CUBOS_INPUT_INPUT_MANAGER_HPP