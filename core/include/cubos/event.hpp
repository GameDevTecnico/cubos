#ifndef CUBOS_EVENT_HPP
#define CUBOS_EVENT_HPP

#include <functional>
#include <vector>
#include <mutex>

namespace cubos
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
    template <typename... TArgs> class Event
    {
    private:
        class MethodCallback
        {
        public:
            virtual void call(TArgs...) = 0;
        };

        template <class TObj> class ObjectMethodCallback : public MethodCallback
        {
        public:
            ObjectMethodCallback(TObj* obj, void (TObj::*callback)(TArgs...)) : obj(obj), callback(callback)
            {
            }
            TObj* obj;
            void (TObj::*callback)(TArgs...);
            virtual void call(TArgs... args) override
            {
                (obj->*callback)(args...);
            }
        };

    public:
        using Callback = std::function<void(TArgs...)>; ///< Callback type.

        using ID = size_t; ///< Callback identifier.

        /// Registers a new callback.
        /// @param callback The callback called when the event is fired.
        /// @return The callback ID, used for unregistering the callback later on.
        /// @see unregisterCallback.
        ID registerCallback(Callback callback);

        /// Unregisters a callback.
        /// @param id The ID returned when the callback was registered.
        /// @see registerCallback.
        void unregisterCallback(ID id);

        template <class TObj> void registerCallback(TObj* obj, void (TObj::*callback)(TArgs...))
        {
            auto lock = std::lock_guard<std::mutex>(this->mutex);
            auto cb = new ObjectMethodCallback<TObj>(obj, callback);
            this->methodCallbacks.push_back(cb);
        }

        template <class TObj> void unregisterCallback(TObj* obj, void (TObj::*callback)(TArgs...))
        {
            auto lock = std::lock_guard<std::mutex>(this->mutex);
            auto it = methodCallbacks.begin();
            for (; it < methodCallbacks.end(); it++)
            {
                MethodCallback& ref = **it;
                if (typeid(ref) == typeid(ObjectMethodCallback<TObj>))
                {
                    auto cb = static_cast<ObjectMethodCallback<TObj>*>(*it);
                    if (cb->obj == obj && cb->callback == callback)
                        break;
                }
            }
            if (it != methodCallbacks.end())
            {
                auto cb = *it;
                methodCallbacks.erase(it);
                delete cb;
            }
        }

        /// Fires the event, calling all the callbacks with the passed arguments.
        /// @param args Event arguments.
        void fire(TArgs... args) const;

    private:
        mutable std::mutex mutex;
        std::vector<Callback> callbacks;
        std::vector<MethodCallback*> methodCallbacks;
    };

    template <typename... TArgs>
    Event<TArgs...>::ID Event<TArgs...>::registerCallback(Event<TArgs...>::Callback callback)
    {
        auto lock = std::lock_guard<std::mutex>(this->mutex);
        this->callbacks.push_back(callback);
        return this->callbacks.size() - 1;
    }

    template <typename... TArgs> void Event<TArgs...>::unregisterCallback(Event<TArgs...>::ID id)
    {
        auto lock = std::lock_guard<std::mutex>(this->mutex);
        this->callbacks[id] = nullptr;
    }

    template <typename... TArgs> void Event<TArgs...>::fire(TArgs... args) const
    {
        auto lock = std::lock_guard<std::mutex>(this->mutex);
        for (auto& callback : this->callbacks)
            if (callback)
                callback(args...);
        for (auto& callback : this->methodCallbacks)
            if (callback)
                callback->call(args...);
    }
} // namespace cubos

#endif // CUBOS_EVENT_HPP
