#ifndef CUBOS_CORE_EVENT_HPP
#define CUBOS_CORE_EVENT_HPP

#include <functional>
#include <mutex>
#include <vector>

namespace cubos::core
{
    /// Class used to easily implement the observer pattern.
    /// Example usage:
    ///
    ///     Event<const char*> event;
    ///     event.registerCallback([](const char* msg) => {
    ///         std::cout << msg << std::endl;
    ///     });
    ///     event.fire("Hello!"); // Prints "Hello!"
    ///
    /// @tparam TArgs The argument types of the callback functions.
    template <typename... TArgs>
    class Event
    {
    public:
        using Callback = std::function<void(TArgs...)>; ///< Callback type.

        using ID = size_t; ///< Callback identifier.

        ~Event();

        /// Registers a new callback.
        /// @param callback The callback called when the event is fired.
        /// @return The callback ID, used for unregistering the callback later on.
        /// @see unregisterCallback.
        ID registerCallback(Callback callback);

        /// Unregisters a callback.
        /// @param id The ID returned when the callback was registered.
        /// @see registerCallback.
        void unregisterCallback(ID id);

        /// Registers a callback.
        /// @param obj The instance associated with the method callback that should be called when the event is fired.
        /// @param callback The method callback when the event is fired.
        /// @see unregisterCallback.
        template <class TObj>
        void registerCallback(TObj* obj, void (TObj::*callback)(TArgs...));

        /// Unregisters a callback.
        /// @param obj The instance associated with the method callback that should be unregistered.
        /// @param callback The method callback that should be unregistred.
        /// @see registerCallback.
        template <class TObj>
        void unregisterCallback(TObj* obj, void (TObj::*callback)(TArgs...));

        /// Fires the event, calling all the callbacks with the passed arguments.
        /// @param args Event arguments.
        void fire(TArgs... args) const;

    private:
        struct MethodCallback
        {
            virtual ~MethodCallback() = default;
            virtual void call(TArgs...) = 0;

            void* obj;
        };

        template <typename TObj>
        struct ObjectMethodCallback : public MethodCallback
        {

            ObjectMethodCallback(TObj* obj, void (TObj::*callback)(TArgs...));
            virtual ~ObjectMethodCallback() override = default;
            virtual void call(TArgs... args) override;

            void (TObj::*callback)(TArgs...);
        };

        mutable std::mutex mutex;
        std::vector<Callback> callbacks;
        std::vector<MethodCallback*> methodCallbacks;
    };

    template <typename... TArgs>
    Event<TArgs...>::~Event()
    {
        for (auto& m : this->methodCallbacks)
            delete m;
    }

    template <typename... TArgs>
    typename Event<TArgs...>::ID Event<TArgs...>::registerCallback(Event<TArgs...>::Callback callback)
    {
        auto lock = std::lock_guard<std::mutex>(this->mutex);
        this->callbacks.push_back(callback);
        return this->callbacks.size() - 1;
    }

    template <typename... TArgs>
    void Event<TArgs...>::unregisterCallback(Event<TArgs...>::ID id)
    {
        auto lock = std::lock_guard<std::mutex>(this->mutex);
        this->callbacks[id] = nullptr;
    }

    template <typename... TArgs>
    void Event<TArgs...>::fire(TArgs... args) const
    {
        auto lock = std::lock_guard<std::mutex>(this->mutex);
        for (auto& callback : this->callbacks)
            if (callback)
                callback(args...);
        for (auto& callback : this->methodCallbacks)
            if (callback)
                callback->call(args...);
    }

    template <typename... TArgs>
    template <typename TObj>
    void Event<TArgs...>::registerCallback(TObj* obj, void (TObj::*callback)(TArgs...))
    {
        auto lock = std::lock_guard<std::mutex>(this->mutex);
        auto cb = new ObjectMethodCallback<TObj>(obj, callback);
        this->methodCallbacks.push_back(cb);
    }

    template <typename... TArgs>
    template <typename TObj>
    void Event<TArgs...>::unregisterCallback(TObj* obj, void (TObj::*callback)(TArgs...))
    {
        auto lock = std::lock_guard<std::mutex>(this->mutex);

        auto it = methodCallbacks.begin();
        for (; it < methodCallbacks.end(); it++)
            if ((*it)->obj == static_cast<void*>(obj))
            {
                auto cb = static_cast<ObjectMethodCallback<TObj>*>(*it);
                if (cb->callback == callback)
                    break;
            }

        if (it != methodCallbacks.end())
        {
            auto cb = *it;
            methodCallbacks.erase(it);
            delete cb;
        }
    }

    template <typename... TArgs>
    template <typename TObj>
    Event<TArgs...>::ObjectMethodCallback<TObj>::ObjectMethodCallback(TObj* obj, void (TObj::*callback)(TArgs...))
    {
        this->obj = obj;
        this->callback = callback;
    }

    template <typename... TArgs>
    template <typename TObj>
    void Event<TArgs...>::ObjectMethodCallback<TObj>::call(TArgs... args)
    {
        (static_cast<TObj*>(this->obj)->*callback)(args...);
    }

} // namespace cubos::core

#endif // CUBOS_CORE_EVENT_HPP
