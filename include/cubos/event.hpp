#ifndef CUBOS_EVENT_HPP
#define CUBOS_EVENT_HPP

#include <functional>
#include <vector>

namespace cubos
{
    /// Class used to easily implement the observer pattern
    /// Example usage:
    ///
    ///     Event<const char*> event;
    ///     event.registerCallback([](const char* msg) => {
    ///         std::cout << msg << std::endl;
    ///     });
    ///     event.fire("Hello!"); // Prints "Hello!"
    /// 
    /// @tparam TArgs The argument types of the callback functions
    template <typename... TArgs> class Event
    {
    public:
        using Callback = std::function<void(TArgs...)>; ///< Callback type
        using ID = size_t;                              /// Callback identifier

        /// Registers a new callback
        /// @param callback The callback called when the event is fired
        /// @return The callback ID, used for unregistering the callback later on
        /// @see unregisterCallback
        ID registerCallback(Callback callback);

        /// Unregisters a callback
        /// @param id The ID returned when the callback was registered
        /// @see registerCallback
        void unregisterCallback(ID id);

        /// Fires the event, calling all the callbacks with the passed arguments
        /// @param args Event arguments
        void fire(TArgs... args) const;

    private:
        std::vector<Callback> callbacks;
    };

    template <typename... TArgs>
    Event<TArgs...>::ID Event<TArgs...>::registerCallback(Event<TArgs...>::Callback callback)
    {
        this->callbacks.push_back(callback);
        return this->callbacks.size() - 1;
    }

    template <typename... TArgs> void Event<TArgs...>::unregisterCallback(Event<TArgs...>::ID id)
    {
        this->callbacks[id] = nullptr;
        return this->callbacks.size() - 1;
    }

    template <typename... TArgs> void Event<TArgs...>::fire(TArgs... args) const
    {
        for (auto& callback : this->callbacks)
            if (callback)
                callback(args...);
    }

} // namespace cubos

#endif // CUBOS_EVENT_HPP
