#ifndef CUBOS_IO_INPUT_MANAGER_HPP
#define CUBOS_IO_INPUT_MANAGER_HPP

#include <cubos/event.hpp>
#include <cubos/io/window.hpp>
#include <cubos/io/keyboard.hpp>
#include <cubos/io/action.hpp>
#include <cubos/io/context.hpp>
#include <cubos/io/sources/source.hpp>

#include <map>
#include <string>
#include <memory>
#include <variant>
#include <list>
#include <glm/glm.hpp>
namespace cubos::io
{
    /// InputManager is used to control the flow between Sources and the gameplay logic.
    /// Handles actions managment and callbacks.
    /// @see Actions
    /// @see Sources
    class InputManager
    {
    public:
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

        /// Registers a key down function callback
        /// @param callback the function to call when the key is down
        /// @param key the key to trigger the callback
        /// @return returns the callback ID
        static cubos::Event<>::ID registerKeyDownCallback(std::function<void(void)> callback, cubos::io::Key key);

        /// Unregisters a key down function callback
        /// @param callbackID the ID of the function calback to unregister
        /// @param key the key to unregister the function callback
        static void unregisterKeyDownCallback(cubos::Event<>::ID callbackID, cubos::io::Key key);

        /// Registers a key down method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be called when the key is down
        /// @param key the key to trigger the callback
        template <class T> static void registerKeyDownCallback(T* obj, void (T::*callback)(), cubos::io::Key key);

        /// Unregisters a key down method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be called when the key is down
        /// @param key the key to trigger the callback
        template <class T> static void unregisterKeyDownCallback(T* obj, void (T::*callback)(), cubos::io::Key key);

        /// Registers a key up function callback
        /// @param callback the function to call when the key is released
        /// @param key the key to trigger the callback
        /// @return returns the callback ID
        static cubos::Event<>::ID registerKeyUpCallback(std::function<void(void)> callback, cubos::io::Key key);

        /// Unregisters a key up function callback
        /// @param callbackID the ID of the function calback to unregister
        /// @param key the key to unregister the function callback
        static void unregisterKeyUpCallback(cubos::Event<>::ID callbackID, cubos::io::Key key);

        /// Registers a key up method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be called when the key is released
        /// @param key the key to trigger the callback
        template <class T> static void registerKeyUpCallback(T* obj, void (T::*callback)(), cubos::io::Key key);

        /// Unregisters a key up method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be unregistered
        /// @param key the key to unregister the callback
        template <class T> static void unregisterKeyUpCallback(T* obj, void (T::*callback)(), cubos::io::Key key);

        /// Registers a mouse button down function callback
        /// @param callback the function to call when the mouse button is down
        /// @param mouseButton the mouse button to trigger the callback
        /// @return returns the callback ID
        static cubos::Event<>::ID registerMouseButtonDownCallback(std::function<void(void)> callback,
                                                                  cubos::io::MouseButton mouseButton);

        /// Unregisters a mouse button down function callback
        /// @param callbackID the ID of the function calback to unregister
        /// @param mouseButton the key to unregister the function callback
        static void unregisterMouseButtonDownCallback(cubos::Event<>::ID callbackID,
                                                      cubos::io::MouseButton mouseButton);

        /// Registers a mouse button down method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be called when the mouse button is down
        /// @param mouseButton the mouse button to trigger the callback
        template <class T>
        static void registerMouseButtonDownCallback(T* obj, void (T::*callback)(), cubos::io::MouseButton mouseButton);

        /// Unregisters a mouse button down method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be called when the mouse is down
        /// @param mouseButton the mouse button to trigger the callback
        template <class T>
        static void unregisterMouseButtonDownCallback(T* obj, void (T::*callback)(),
                                                      cubos::io::MouseButton mouseButton);

        /// Registers a mouse button up function callback
        /// @param callback the function to call when the mouse button is released
        /// @param mouseButton the mouse button to trigger the callback
        /// @return returns the callback ID
        static cubos::Event<>::ID registerMouseButtonUpCallback(std::function<void(void)> callback,
                                                                  cubos::io::MouseButton mouseButton);

        /// Unregisters a mouse button up function callback
        /// @param callbackID the ID of the function calback to unregister
        /// @param mouseButton the key to unregister the function callback
        static void unregisterMouseButtonUpCallback(cubos::Event<>::ID callbackID,
                                                      cubos::io::MouseButton mouseButton);

        /// Registers a mouse button up method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be called when the mouse button is released
        /// @param mouseButton the mouse button to trigger the callback
        template <class T>
        static void registerMouseButtonUpCallback(T* obj, void (T::*callback)(), cubos::io::MouseButton mouseButton);

        /// Unregisters a mouse button up method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be called when the mouse is released
        /// @param mouseButton the mouse button to trigger the callback
        template <class T>
        static void unregisterMouseButtonUpCallback(T* obj, void (T::*callback)(),
                                                      cubos::io::MouseButton mouseButton);

        /// Registers a mouse axis function callback
        /// @param callback the function to call when the mouse axis changes
        /// @param mouseAxis the mouse axis to trigger the callback
        /// @return returns the callback ID
        static cubos::Event<float>::ID registerMouseAxisCallback(std::function<void(float)> callback,
                                                                 cubos::io::MouseAxis mouseAxis);

        /// Unregisters a mouse axis function callback
        /// @param callbackID the ID of the function calback to unregister
        /// @param mouseAxis the mouse axis to unregister the function callback
        static void unregisterMouseAxisCallback(cubos::Event<float>::ID callbackID, cubos::io::MouseAxis mouseAxis);

        /// Registers a mouse axis method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be called when the mouse axis change
        /// @param mouseAxis the mouse axis that will trigger the callback
        template <class T>
        static void registerMouseAxisCallback(T* obj, void (T::*callback)(float), cubos::io::MouseAxis mouseAxis);

        /// Unregisters a mouse axis method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be called when the mouse axis change
        /// @param mouseAxis the mouse axis that will trigger the callback
        template <class T>
        static void unregisterMouseAxisCallback(T* obj, void (T::*callback)(float), cubos::io::MouseAxis mouseAxis);

    private:
        static cubos::io::Window* window;
        static std::map<std::string, std::shared_ptr<Action>> actions;

        static void handleKeyDown(cubos::io::Key key);
        static void handleKeyUp(cubos::io::Key key);
        static void handleMouseButtonDown(cubos::io::MouseButton mouseButton);
        static void handleMouseButtonUp(cubos::io::MouseButton mouseButton);
        static void handleMouseAxis(glm::ivec2 coordinates);
    };

    template <class T> void InputManager::registerKeyDownCallback(T* obj, void (T::*callback)(), cubos::io::Key key)
    {
        if (!InputManager::keyDownCallbacks.contains(key))
        {
            InputManager::keyDownCallbacks[key] = std::make_shared<cubos::Event<>>();
        }
        InputManager::keyDownCallbacks[key]->registerCallback<T>(obj, callback);
    }

    template <class T> void InputManager::unregisterKeyDownCallback(T* obj, void (T::*callback)(), cubos::io::Key key)
    {
        if (!InputManager::keyDownCallbacks.contains(key))
        {
            return;
        }
        InputManager::keyDownCallbacks[key]->unregisterCallback<T>(obj, callback);
    }

    template <class T> void InputManager::registerKeyUpCallback(T* obj, void (T::*callback)(), cubos::io::Key key)
    {
        if (!InputManager::keyUpCallbacks.contains(key))
        {
            InputManager::keyUpCallbacks[key] = std::make_shared<cubos::Event<>>();
        }
        InputManager::keyUpCallbacks[key]->registerCallback<T>(obj, callback);
    }

    template <class T> void InputManager::unregisterKeyUpCallback(T* obj, void (T::*callback)(), cubos::io::Key key)
    {
        if (!InputManager::keyUpCallbacks.contains(key))
        {
            return;
        }
        InputManager::keyUpCallbacks[key]->unregisterCallback<T>(obj, callback);
    }

    template <class T>
    void InputManager::registerMouseButtonDownCallback(T* obj, void (T::*callback)(),
                                                       cubos::io::MouseButton mouseButton)
    {
        if (!InputManager::mouseButtonDownCallbacks.contains(mouseButton))
        {
            InputManager::mouseButtonDownCallbacks[mouseButton] = std::make_shared<cubos::Event<>>();
        }
        InputManager::mouseButtonDownCallbacks[mouseButton]->registerCallback<T>(obj, callback);
    }

    template <class T>
    void InputManager::unregisterMouseButtonDownCallback(T* obj, void (T::*callback)(),
                                                         cubos::io::MouseButton mouseButton)
    {
        if (!InputManager::mouseButtonDownCallbacks.contains(mouseButton))
        {
            return;
        }
        InputManager::mouseButtonDownCallbacks[mouseButton]->unregisterCallback<T>(obj, callback);
    }

    template <class T>
    void InputManager::registerMouseButtonUpCallback(T* obj, void (T::*callback)(),
                                                       cubos::io::MouseButton mouseButton)
    {
        if (!InputManager::mouseButtonUpCallbacks.contains(mouseButton))
        {
            InputManager::mouseButtonUpCallbacks[mouseButton] = std::make_shared<cubos::Event<>>();
        }
        InputManager::mouseButtonUpCallbacks[mouseButton]->registerCallback<T>(obj, callback);
    }

    template <class T>
    void InputManager::unregisterMouseButtonUpCallback(T* obj, void (T::*callback)(),
                                                         cubos::io::MouseButton mouseButton)
    {
        if (!InputManager::mouseButtonUpCallbacks.contains(mouseButton))
        {
            return;
        }
        InputManager::mouseButtonUpCallbacks[mouseButton]->unregisterCallback<T>(obj, callback);
    }

    template <class T>
    void InputManager::registerMouseAxisCallback(T* obj, void (T::*callback)(float), cubos::io::MouseAxis mouseAxis)
    {
        if (!InputManager::mouseAxisCallbacks.contains(mouseAxis))
        {
            InputManager::mouseAxisCallbacks[mouseAxis] = std::make_shared<cubos::Event<float>>();
        }
        InputManager::mouseAxisCallbacks[mouseAxis]->registerCallback<T>(obj, callback);
    }

    template <class T>
    void InputManager::unregisterMouseAxisCallback(T* obj, void (T::*callback)(float), cubos::io::MouseAxis mouseAxis)
    {
        if (!InputManager::mouseAxisCallbacks.contains(mouseAxis))
        {
            return;
        }
        InputManager::mouseAxisCallbacks[mouseAxis]->unregisterCallback<T>(obj, callback);
    }
} // namespace cubos::io

#endif // CUBOS_IO_INPUT_MANAGER_HPP
