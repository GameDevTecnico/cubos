#ifndef CUBOS_INPUT_INPUT_MANAGER_HPP
#define CUBOS_INPUT_INPUT_MANAGER_HPP

#include <cubos/event.hpp>
#include <cubos/io/window.hpp>
#include <cubos/io/keyboard.hpp>
#include <map>
#include <string>
#include <memory>
#include <variant>
#include <list>
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
        virtual void subscribeEvents(cubos::io::Window* window) = 0;
        virtual void unsubscribeEvents(cubos::io::Window* window) = 0;
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
        void subscribeEvents(cubos::io::Window* window);
        void unsubscribeEvents(cubos::io::Window* window);

    private:
        bool wasTriggered = false;
        void handleButtonDown();
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
        void addInput(InputSource* source);
        void addBinding(std::function<void(InputContext)> binding);
        void processSources();
    };

    class InputManager
    {
    public:
        static cubos::io::Window* window;
        static std::map<std::string, std::shared_ptr<InputAction>> actions;

        static std::map<cubos::io::Key, std::shared_ptr<cubos::Event<>>> keyDownCallbacks;
        static std::map<cubos::io::Key, std::shared_ptr<cubos::Event<>>> keyUpCallbacks;
        static std::map<cubos::io::MouseButton, std::shared_ptr<cubos::Event<>>> mouseButtonDownCallbacks;
        static std::map<cubos::io::MouseButton, std::shared_ptr<cubos::Event<>>> mouseButtonUpCallbacks;

        static void init(cubos::io::Window* window);
        static std::shared_ptr<InputAction> createAction(std::string name);
        static std::shared_ptr<InputAction> getAction(std::string name);
        static void processActions();

        static cubos::Event<>::ID registerKeyDownCallback(std::function<void(void)> callback, cubos::io::Key key)
        {
            if (!InputManager::keyDownCallbacks.contains(key))
            {
                InputManager::keyDownCallbacks[key] = std::make_shared<cubos::Event<>>();
            }
            InputManager::keyDownCallbacks[key]->registerCallback(callback);
        }

        static void unregisterKeyDownCallback(cubos::Event<>::ID callbackID, cubos::io::Key key)
        {
            if (!InputManager::keyDownCallbacks.contains(key))
            {
                return;
            }
            InputManager::keyDownCallbacks[key]->unregisterCallback(callbackID);
        }

        template <class T> static void registerKeyDownCallback(T* obj, void (T::*callback)(), cubos::io::Key key)
        {
            if (!InputManager::keyDownCallbacks.contains(key))
            {
                InputManager::keyDownCallbacks[key] = std::make_shared<cubos::Event<>>();
            }
            InputManager::keyDownCallbacks[key]->registerCallback<T>(obj, callback);
        }

        template <class T> static void unregisterKeyDownCallback(T* obj, void (T::*callback)(), cubos::io::Key key)
        {
            if (!InputManager::keyDownCallbacks.contains(key))
            {
                return;
            }
            InputManager::keyDownCallbacks[key]->unregisterCallback<T>(obj, callback);
        }

        static cubos::Event<>::ID registerMouseButtonDownCallback(std::function<void(void)> callback,
                                                                  cubos::io::MouseButton mouseButton)
        {
            if (!InputManager::mouseButtonDownCallbacks.contains(mouseButton))
            {
                InputManager::mouseButtonDownCallbacks[mouseButton] = std::make_shared<cubos::Event<>>();
            }
            InputManager::mouseButtonDownCallbacks[mouseButton]->registerCallback(callback);
        }

        static void unregisterMouseButtonDownCallback(cubos::Event<>::ID callbackID, cubos::io::MouseButton mouseButton)
        {
            if (!InputManager::mouseButtonDownCallbacks.contains(mouseButton))
            {
                return;
            }
            InputManager::mouseButtonDownCallbacks[mouseButton]->unregisterCallback(callbackID);
        }

        template <class T>
        static void registerMouseButtonDownCallback(T* obj, void (T::*callback)(), cubos::io::MouseButton mouseButton)
        {
            if (!InputManager::mouseButtonDownCallbacks.contains(mouseButton))
            {
                InputManager::mouseButtonDownCallbacks[mouseButton] = std::make_shared<cubos::Event<>>();
            }
            InputManager::mouseButtonDownCallbacks[mouseButton]->registerCallback<T>(obj, callback);
        }

        template <class T>
        static void unregisterMouseButtonDownCallback(T* obj, void (T::*callback)(), cubos::io::MouseButton mouseButton)
        {
            if (!InputManager::mouseButtonDownCallbacks.contains(mouseButton))
            {
                return;
            }
            InputManager::mouseButtonDownCallbacks[mouseButton]->unregisterCallback<T>(obj, callback);
        }

    private:
        static void handleKeyDown(cubos::io::Key key);
        static void handleKeyUp(cubos::io::Key key);
        static void handleMouseButtonDown(cubos::io::MouseButton mouseButton);
        static void handleMouseButtonUp(cubos::io::MouseButton mouseButton);
    };
} // namespace cubos::input

#endif // CUBOS_INPUT_INPUT_MANAGER_HPP