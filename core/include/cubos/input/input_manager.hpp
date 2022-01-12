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

        static std::map<cubos::io::Key, std::vector<std::function<void(void)>>> keyDownCallbacks;

        class Callback
        {
        public:
            virtual void call() = 0;
        };
        template <class TObj> class ObjectCallback : public Callback
        {
        public:
            ObjectCallback(TObj* obj, void (TObj::*callback)()) : obj(obj), callback(callback)
            {
            }
            TObj* obj;
            void (TObj::*callback)();
            virtual void call() override
            {
                (obj->*callback)();
            }
        };
        static std::map<cubos::io::Key, std::vector<Callback*>> keyDownObjectCallbacks;
        static std::map<cubos::io::Key, std::vector<std::function<void(void)>>> keyUpCallbacks;
        static std::map<cubos::io::MouseButton, std::vector<std::function<void(void)>>> mouseButtonDownCallbacks;
        static std::map<cubos::io::MouseButton, std::vector<std::function<void(void)>>> mouseButtonUpCallbacks;

        static void init(cubos::io::Window* window);
        static std::shared_ptr<InputAction> createAction(std::string name);
        static std::shared_ptr<InputAction> getAction(std::string name);
        static void processActions();

        static void registerKeyDownCallback(std::function<void(void)> callback, cubos::io::Key key);
        template <class T> static void registerKeyDownCallback(T* obj, void (T::*callback)(), cubos::io::Key key)
        {
            auto cb = new ObjectCallback<T>(obj, callback);
            if (InputManager::keyDownObjectCallbacks.contains(key))
            {
                InputManager::keyDownObjectCallbacks[key].push_back(cb);
            }
            else
            {
                std::vector<Callback*> v = std::vector<Callback*>();
                v.push_back(cb);
                InputManager::keyDownObjectCallbacks.insert({key, v});
            }
        }
        template <class T> static void removeKeyDownCallback(T* obj, void (T::*callback)(), cubos::io::Key key)
        {
            if (InputManager::keyDownObjectCallbacks.contains(key))
            {
                auto it = keyDownObjectCallbacks[key].begin();
                for (; it < keyDownObjectCallbacks[key].end(); it++)
                {
                    auto cb = dynamic_cast<ObjectCallback<T>*>(*it);
                    if (cb != nullptr && cb->obj == obj && cb->callback == callback)
                        break;
                }
                if (it != keyDownObjectCallbacks[key].end())
                {
                    keyDownObjectCallbacks[key].erase(it);
                }
            }
        }
        static void registerMouseButtonDownCallback(std::function<void(void)> callback, cubos::io::MouseButton);

    private:
        static void handleKeyDown(cubos::io::Key key);
        static void handleKeyUp(cubos::io::Key key);
        static void handleMouseButtonDown(cubos::io::MouseButton mouseButton);
        static void handleMouseButtonUp(cubos::io::MouseButton mouseButton);
    };
} // namespace cubos::input

#endif // CUBOS_INPUT_INPUT_MANAGER_HPP