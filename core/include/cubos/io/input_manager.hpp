#ifndef CUBOS_IO_INPUT_MANAGER_HPP
#define CUBOS_IO_INPUT_MANAGER_HPP

#include <cubos/event.hpp>
#include <cubos/io/window.hpp>
#include <cubos/io/keyboard.hpp>
#include <map>
#include <string>
#include <memory>
#include <variant>
#include <list>
#include <glm/glm.hpp>
#include <cubos/io/action.hpp>
#include <cubos/io/context.hpp>
#include <cubos/io/sources/source.hpp>

namespace cubos::io
{
    /// InputManager is used to control the flow between Sources and the gameplay logic.
    /// Handles actions managment and callbacks.
    /// @see Actions
    class InputManager
    {
    public:
        static cubos::io::Window* window;                              ///< Window associated with this Input Manager
        static std::map<std::string, std::shared_ptr<Action>> actions; ///< Mapping of all registered Actions

        static std::map<cubos::io::Key, std::shared_ptr<cubos::Event<>>>
            keyDownCallbacks; ///< Mapping of all key down Callbacks
        static std::map<cubos::io::Key, std::shared_ptr<cubos::Event<>>>
            keyUpCallbacks; ///< Mapping of all key up Callbacks
        static std::map<cubos::io::MouseButton, std::shared_ptr<cubos::Event<>>>
            mouseButtonDownCallbacks; ///< Mapping of all button down Callbacks
        static std::map<cubos::io::MouseButton, std::shared_ptr<cubos::Event<>>>
            mouseButtonUpCallbacks; ///< Mapping of all button up Callbacks
        static std::map<cubos::io::MouseAxis, std::shared_ptr<cubos::Event<float>>>
            mouseAxisCallbacks; ///< Mapping of all mouse axis Callbacks

        /// Associates a window to this InputManager. An InputManager can only be associated to one window at once
        /// @param window the window to assciate this InputManager to
        static void init(cubos::io::Window* window);

        /// Creates an action with a given name
        /// @param name the name of the Action to be created
        /// @return A pointer to the created action
        static std::shared_ptr<Action> createAction(std::string name);

        /// Returns an Action with a given name
        /// @param name the name of the Action to be returned
        /// @return A pointer to the Action with the given name or null if no Action with the given name exists.
        static std::shared_ptr<Action> getAction(std::string name);

        /// Calls the processSources method in all registered Actions
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

        static cubos::Event<float>::ID registerMouseAxisCallback(std::function<void(float)> callback,
                                                                 cubos::io::MouseAxis mouseAxis)
        {
            if (!InputManager::mouseAxisCallbacks.contains(mouseAxis))
            {
                InputManager::mouseAxisCallbacks[mouseAxis] = std::make_shared<cubos::Event<float>>();
            }
            InputManager::mouseAxisCallbacks[mouseAxis]->registerCallback(callback);
        }

        static void unregisterMouseAxisCallback(cubos::Event<float>::ID callbackID, cubos::io::MouseAxis mouseAxis)
        {
            if (!InputManager::mouseAxisCallbacks.contains(mouseAxis))
            {
                return;
            }
            InputManager::mouseAxisCallbacks[mouseAxis]->unregisterCallback(callbackID);
        }

        template <class T>
        static void registerMouseAxisCallback(T* obj, void (T::*callback)(float), cubos::io::MouseAxis mouseAxis)
        {
            if (!InputManager::mouseAxisCallbacks.contains(mouseAxis))
            {
                InputManager::mouseAxisCallbacks[mouseAxis] = std::make_shared<cubos::Event<float>>();
            }
            InputManager::mouseAxisCallbacks[mouseAxis]->registerCallback<T>(obj, callback);
        }

        template <class T>
        static void unregisterMouseAxisCallback(T* obj, void (T::*callback)(float), cubos::io::MouseAxis mouseAxis)
        {
            if (!InputManager::mouseAxisCallbacks.contains(mouseAxis))
            {
                return;
            }
            InputManager::mouseAxisCallbacks[mouseAxis]->unregisterCallback<T>(obj, callback);
        }

    private:
        static void handleKeyDown(cubos::io::Key key);
        static void handleKeyUp(cubos::io::Key key);
        static void handleMouseButtonDown(cubos::io::MouseButton mouseButton);
        static void handleMouseButtonUp(cubos::io::MouseButton mouseButton);
        static void handleMouseAxis(glm::ivec2 coordinates);
    };
} // namespace cubos::io

#endif // CUBOS_IO_INPUT_MANAGER_HPP