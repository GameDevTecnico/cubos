#include <cubos/core/ecs/dynamic.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

using cubos::core::ecs::DynamicPlugin;
using cubos::core::ecs::Plugin;

DynamicPlugin::~DynamicPlugin()
{
    this->unload();
}

bool DynamicPlugin::load(const std::string& name)
{
    CUBOS_ASSERT(mHandle == nullptr, "Plugin already loaded");

    // Load the shared library.
#ifdef _WIN32
    mHandle = static_cast<void*>(LoadLibrary(name.c_str()));
#else
    mHandle = dlopen(name.c_str(), RTLD_LAZY);
#endif

    // Check if the shared library was loaded successfully.
    if (mHandle == nullptr)
    {
#ifdef _WIN32
        CUBOS_ERROR("Failed to load plugin {}: error {}", name, GetLastError());
#else
        CUBOS_ERROR("Failed to load plugin {}: {}", name, dlerror());
#endif
        return false;
    }

    // Get entry plugin function in the shared library.
#ifdef _WIN32
    mPlugin = reinterpret_cast<Plugin>(GetProcAddress(static_cast<HMODULE>(mHandle), "plugin"));
#else
    mPlugin = reinterpret_cast<Plugin>(dlsym(mHandle, "plugin"));
#endif

    // Check if the plugin function was found.
    if (mPlugin == nullptr)
    {
#ifdef _WIN32
        CUBOS_ERROR("Failed to get plugin function from {}: error {}", name, GetLastError());
#else
        CUBOS_ERROR("Failed to get plugin function from {}: {}", name, dlerror());
#endif

        this->unload();
        return false;
    }

    return true;
}

void DynamicPlugin::unload()
{
    if (mHandle != nullptr)
    {
#ifdef _WIN32
        FreeLibrary(static_cast<HMODULE>(mHandle));
#else
        dlclose(mHandle);
#endif

        mHandle = nullptr;
    }
}

Plugin DynamicPlugin::function() const
{
    CUBOS_ASSERT(mHandle != nullptr, "Plugin not loaded");
    return mPlugin;
}
