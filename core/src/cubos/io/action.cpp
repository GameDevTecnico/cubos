#include <cubos/io/action.hpp>
#include <cubos/io/input_manager.hpp>
#include <cubos/io/context.hpp>
#include <cubos/io/sources/source.hpp>
#include <string>

using namespace cubos::io;

Action::Action(std::string actionName)
{
    name = actionName;
}

void Action::addInput(Source* source)
{
    source->subscribeEvents(InputManager::window);
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