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
        void handleKeyUp(cubos::io::Key key);
        void handleKeyDown();
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

        static std::map<cubos::io::Key, std::vector<std::function<void(void)>>> keyDownCallbacks;
        static std::map<cubos::io::Key, std::vector<std::function<void(void)>>> keyUpCallbacks;

        static void Init(cubos::io::Window* window);
        static std::shared_ptr<InputAction> CreateAction(std::string name);
        static std::shared_ptr<InputAction> GetAction(std::string name);
        static void ProcessActions();

        static void registerKeyDownCallback(std::function<void(void)> callback, cubos::io::Key key);

    private:
        static void handleKeyDown(cubos::io::Key key);
        static void handleKeyUp(cubos::io::Key key);
        static void handleMouseDown(cubos::io::MouseButton mouseButton);
        static void handleMouseUp(cubos::io::MouseButton mouseButton);
    };
} // namespace cubos::input

#endif // CUBOS_INPUT_INPUT_MANAGER_HPP