#include <string>

#include <cubos/core/io/action.hpp>
#include <cubos/core/io/context.hpp>
#include <cubos/core/io/input_manager.hpp>
#include <cubos/core/io/sources/source.hpp>

using namespace cubos::core::io;

Action::Action(std::string actionName)
{
    name = actionName;
}

std::string Action::getName() const
{
    return this->name;
}

void Action::addSource(Source* source)
{
    source->subscribeEvents();
    this->inputSources.push_back(source);
}

void Action::addBinding(std::function<void(Context)> binding)
{
    this->functionBindings.push_back(binding);
}

void Action::processSources()
{
    for (auto itSource = this->inputSources.begin(); itSource != this->inputSources.end(); itSource++)
    {
        if ((*itSource)->isTriggered())
        {
            for (auto itBinding = this->functionBindings.begin(); itBinding != this->functionBindings.end();
                 itBinding++)
            {
                (*itBinding)((*itSource)->createContext());
            }
            return;
        }
    }
}
