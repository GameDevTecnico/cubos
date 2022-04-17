#ifndef CUBOS_CORE_IO_INPUT_MANAGER_HPP
#define CUBOS_CORE_IO_INPUT_MANAGER_HPP

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
namespace cubos::core::io
{
    /// InputManager is used to control the flow between Sources and the gameplay logic.
    /// Handles actions managment and callbacks.
    /// @see Actions
    /// @see Sources
    class InputManager
    {
    public:
        static std::map<cubos::core::io::Key, std::shared_ptr<cubos::core::Event<>>>
            keyDownCallbacks; ///< Mapping of all key down Callbacks
        static std::map<cubos::core::io::Key, std::shared_ptr<cubos::core::Event<>>>
            keyUpCallbacks; ///< Mapping of all key up Callbacks
        static std::map<cubos::core::io::MouseButton, std::shared_ptr<cubos::core::Event<>>>
            mouseButtonDownCallbacks; ///< Mapping of all button down Callbacks
        static std::map<cubos::core::io::MouseButton, std::shared_ptr<cubos::core::Event<>>>
            mouseButtonUpCallbacks; ///< Mapping of all button up Callbacks
        static std::map<cubos::core::io::MouseAxis, std::shared_ptr<cubos::core::Event<float>>>
            mouseAxisCallbacks; ///< Mapping of all mouse axis Callbacks

        /// Associates a window to this InputManager. An InputManager can only be associated to one window at once
        /// @param window the window to assciate this InputManager to
        static void init(cubos::core::io::Window* window);

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
        static cubos::core::Event<>::ID registerKeyDownCallback(std::function<void(void)> callback,
                                                                cubos::core::io::Key key);

        /// Unregisters a key down function callback
        /// @param callbackID the ID of the function calback to unregister
        /// @param key the key to unregister the function callback
        static void unregisterKeyDownCallback(cubos::core::Event<>::ID callbackID, cubos::core::io::Key key);

        /// Registers a key down method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be called when the key is down
        /// @param key the key to trigger the callback
        template <class T> static void registerKeyDownCallback(T* obj, void (T::*callback)(), cubos::core::io::Key key);

        /// Unregisters a key down method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be called when the key is down
        /// @param key the key to trigger the callback
        template <class T>
        static void unregisterKeyDownCallback(T* obj, void (T::*callback)(), cubos::core::io::Key key);

        /// Registers a mouse button down function callback
        /// @param callback the function to call when the mouse button is down
        /// @param mouseButton the mouse button to trigger the callback
        /// @return returns the callback ID
        static cubos::core::Event<>::ID registerMouseButtonDownCallback(std::function<void(void)> callback,
                                                                        cubos::core::io::MouseButton mouseButton);

        /// Unregisters a mouse button down function callback
        /// @param callbackID the ID of the function calback to unregister
        /// @param mouseButton the key to unregister the function callback
        static void unregisterMouseButtonDownCallback(cubos::core::Event<>::ID callbackID,
                                                      cubos::core::io::MouseButton mouseButton);

        /// Registers a mouse button method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be called when the mouse button is down
        /// @param mouseButton the mouse button to trigger the callback
        template <class T>
        static void registerMouseButtonDownCallback(T* obj, void (T::*callback)(),
                                                    cubos::core::io::MouseButton mouseButton);

        /// Unregisters a mouse button down method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be called when the mouse is down
        /// @param mouseButton the mouse button to trigger the callback
        template <class T>
        static void unregisterMouseButtonDownCallback(T* obj, void (T::*callback)(),
                                                      cubos::core::io::MouseButton mouseButton);

        /// Registers a mouse axis function callback
        /// @param callback the function to call when the mouse axis changes
        /// @param mouseAxis the mouse axis to trigger the callback
        /// @return returns the callback ID
        static cubos::core::Event<float>::ID registerMouseAxisCallback(std::function<void(float)> callback,
                                                                       cubos::core::io::MouseAxis mouseAxis);

        /// Unregisters a mouse axis function callback
        /// @param callbackID the ID of the function calback to unregister
        /// @param mouseAxis the mouse axis to unregister the function callback
        static void unregisterMouseAxisCallback(cubos::core::Event<float>::ID callbackID,
                                                cubos::core::io::MouseAxis mouseAxis);

        /// Registers a mouse axis method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be called when the mouse axis change
        /// @param mouseAxis the mouse axis that will trigger the callback
        template <class T>
        static void registerMouseAxisCallback(T* obj, void (T::*callback)(float), cubos::core::io::MouseAxis mouseAxis);

        /// Unregisters a mouse axis method callback
        /// @param obj the object owning the method
        /// @param callback the method that will be called when the mouse axis change
        /// @param mouseAxis the mouse axis that will trigger the callback
        template <class T>
        static void unregisterMouseAxisCallback(T* obj, void (T::*callback)(float),
                                                cubos::core::io::MouseAxis mouseAxis);

    private:
        static cubos::core::io::Window* window;
        static std::map<std::string, std::shared_ptr<Action>> actions;

        static void handleKeyDown(cubos::core::io::Key key);
        static void handleKeyUp(cubos::core::io::Key key);
        static void handleMouseButtonDown(cubos::core::io::MouseButton mouseButton);
        static void handleMouseButtonUp(cubos::core::io::MouseButton mouseButton);
        static void handleMouseAxis(glm::ivec2 coordinates);
    };

    template <class T>
    void InputManager::registerKeyDownCallback(T* obj, void (T::*callback)(), cubos::core::io::Key key)
    {
        if (!InputManager::keyDownCallbacks.contains(key))
        {
            InputManager::keyDownCallbacks[key] = std::make_shared<cubos::core::Event<>>();
        }
        InputManager::keyDownCallbacks[key]->registerCallback<T>(obj, callback);
    }

    template <class T>
    void InputManager::unregisterKeyDownCallback(T* obj, void (T::*callback)(), cubos::core::io::Key key)
    {
        if (!InputManager::keyDownCallbacks.contains(key))
        {
            return;
        }
        InputManager::keyDownCallbacks[key]->unregisterCallback<T>(obj, callback);
    }

    template <class T>
    void InputManager::registerMouseButtonDownCallback(T* obj, void (T::*callback)(),
                                                       cubos::core::io::MouseButton mouseButton)
    {
        if (!InputManager::mouseButtonDownCallbacks.contains(mouseButton))
        {
            InputManager::mouseButtonDownCallbacks[mouseButton] = std::make_shared<cubos::core::Event<>>();
        }
        InputManager::mouseButtonDownCallbacks[mouseButton]->registerCallback<T>(obj, callback);
    }

    template <class T>
    void InputManager::unregisterMouseButtonDownCallback(T* obj, void (T::*callback)(),
                                                         cubos::core::io::MouseButton mouseButton)
    {
        if (!InputManager::mouseButtonDownCallbacks.contains(mouseButton))
        {
            return;
        }
        InputManager::mouseButtonDownCallbacks[mouseButton]->unregisterCallback<T>(obj, callback);
    }

    template <class T>
    void InputManager::registerMouseAxisCallback(T* obj, void (T::*callback)(float),
                                                 cubos::core::io::MouseAxis mouseAxis)
    {
        if (!InputManager::mouseAxisCallbacks.contains(mouseAxis))
        {
            InputManager::mouseAxisCallbacks[mouseAxis] = std::make_shared<cubos::core::Event<float>>();
        }
        InputManager::mouseAxisCallbacks[mouseAxis]->registerCallback<T>(obj, callback);
    }

    template <class T>
    void InputManager::unregisterMouseAxisCallback(T* obj, void (T::*callback)(float),
                                                   cubos::core::io::MouseAxis mouseAxis)
    {
        if (!InputManager::mouseAxisCallbacks.contains(mouseAxis))
        {
            return;
        }
        InputManager::mouseAxisCallbacks[mouseAxis]->unregisterCallback<T>(obj, callback);
    }
} // namespace cubos::core::io

#endif // CUBOS_CORE_IO_INPUT_MANAGER_HPP
