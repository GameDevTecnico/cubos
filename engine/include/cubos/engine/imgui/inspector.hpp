/// @file
/// @brief System argument @ref cubos::engine::ImGuiInspector.
/// @ingroup imgui-plugin

#pragma once

#include <string>
#include <vector>

#include <cubos/core/ecs/system/arguments/resources.hpp>
#include <cubos/core/ecs/system/arguments/world.hpp>
#include <cubos/core/memory/function.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief System argument which allows the user to show or edit any reflectable value through ImGui.
    /// @see Take a look at the @ref examples-engine-imgui example for a demonstration of this argument.
    /// @ingroup imgui-plugin
    class CUBOS_ENGINE_API ImGuiInspector final
    {
    public:
        /// @brief Possible outputs of a hook function.
        enum class HookResult
        {
            Shown,     /// Value was not modified, but was handled by the hook.
            Modified,  /// Value was modified by the hook.
            Unhandled, /// Value was not handled by the hook, and should be passed to the next hook.
        };

        /// @brief Function type for general inspector hooks.
        ///
        /// Receives the value name, whether it is read only, the inspector, the type of the value being inspected, and
        /// a pointer to the value.
        ///
        /// The passed value may only be modified if the value is not read only and the hook returns
        /// @ref HookResult::Modified.
        using Hook = core::memory::Function<HookResult(const std::string&, bool, ImGuiInspector&,
                                                       const core::reflection::Type&, void*)>;

        /// @brief Function type for specific inspector hooks.
        ///
        /// Receives the value name, whether it is read only, the inspector, and a reference to the value.
        /// The passed value may only be modified if the value returns @ref HookResult::Modified.
        template <typename T>
        using TypedHook = core::memory::Function<HookResult(const std::string&, bool, ImGuiInspector, T&)>;

        /// @brief Resource which holds the internal inspector state.
        ///
        /// Initialized automatically by the ImGui plugin.
        struct State
        {
            CUBOS_REFLECT;

            State();

            std::vector<Hook> hooks;
        };

        /// @brief Constructs a new inspector argument.
        /// @param world ECS world.
        /// @param state Inspector state.
        ImGuiInspector(core::ecs::World& world, State& state);

        /// @brief Displays a reflectable value on the UI.
        /// @param name Name of the value.
        /// @param type Value type.
        /// @param value Pointer to value.
        void show(const std::string& name, const core::reflection::Type& type, const void* value);

        /// @brief Displays a reflectable value on the UI and allows modifying it.
        /// @param name Name of the value.
        /// @param type Value type.
        /// @param value Pointer to value.
        /// @return Whether the object was modified.
        bool edit(const std::string& name, const core::reflection::Type& type, void* value);

        /// @brief Displays a reflectable value on the UI and optionally allows modifying it.
        /// @param name Name of the value.
        /// @param readOnly Whether the value is read only.
        /// @param type Value type.
        /// @param value Pointer to value.
        /// @return Whether the object was modified.
        bool inspect(const std::string& name, bool readOnly, const core::reflection::Type& type, void* value);

        /// @copybrief show
        /// @param name Name of the value.
        /// @tparam T Value type.
        /// @param value Pointer to value.
        template <typename T>
        void show(const std::string& name, const T& value)
        {
            this->show(name, core::reflection::reflect<T>(), &value);
        }

        /// @copybrief edit
        /// @param name Name of the value.
        /// @tparam T Value type.
        /// @param value Pointer to value.
        /// @return Whether the object was modified.
        template <typename T>
        bool edit(const std::string& name, T& value)
        {
            return this->edit(name, core::reflection::reflect<T>(), &value);
        }

        /// @brief Overrides the default inspector UI for any types accepted by the given hook.
        ///
        /// Takes priority over any hook previously defined.
        ///
        /// @param hook Hook to use.
        void hook(Hook hook);

        /// @brief Overrides the default inspector UI for a specific type accepted by the given hook.
        ///
        /// Takes priority over any hook previously defined.
        ///
        /// @tparam T Type to override.
        /// @param hook Hook to use.
        template <typename T>
        void hook(TypedHook<T> hook)
        {
            this->hook([hook = core::memory::move(hook)](const std::string& name, bool readOnly,
                                                         ImGuiInspector& inspector, core::reflection::Type& type,
                                                         void* value) {
                if (type.is<T>())
                {
                    return hook(name, readOnly, inspector, *reinterpret_cast<T*>(value));
                }
                return HookResult::Unhandled;
            });
        }

        /// @brief Gets the world the inspector is operating on.
        core::ecs::World& world()
        {
            return mWorld;
        }

    private:
        core::ecs::World& mWorld;
        State& mState;
    };
} // namespace cubos::engine

namespace cubos::core::ecs
{
    template <>
    class SystemFetcher<engine::ImGuiInspector>
    {
    public:
        static inline constexpr bool ConsumesOptions = false;

        SystemFetcher<World&> world;
        SystemFetcher<engine::ImGuiInspector::State&> state;

        SystemFetcher(World& world, const SystemOptions& options)
            : world{world, options}
            , state{world, options}
        {
        }

        void analyze(SystemAccess& access) const
        {
            world.analyze(access);
            state.analyze(access);
        }

        engine::ImGuiInspector fetch(const SystemContext& ctx)
        {
            return {world.fetch(ctx), state.fetch(ctx)};
        }
    };
} // namespace cubos::core::ecs
