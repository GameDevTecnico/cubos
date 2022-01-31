#include <cubos/io/input_action.hpp>
#include <cubos/io/input_manager.hpp>
#include <cubos/io/input_context.hpp>
#include <cubos/io/input_sources/input_source.hpp>
#include <string>

using namespace cubos::io;

InputAction::InputAction(std::string actionName)
{
    name = actionName;
}

void InputAction::addInput(InputSource* source)
{
    source->subscribeEvents(InputManager::window);
    this->inputSources.push_back(source);
}

void InputAction::addBinding(std::function<void(InputContext)> binding)
{
    this->functionBindings.push_back(binding);
}

void InputAction::processSources()
{
    for (auto itSource = this->inputSources.begin(); itSource != this->inputSources.end(); itSource++)
    {
        if ((*itSource)->isTriggered())
        {
            for (auto itBinding = this->functionBindings.begin(); itBinding != this->functionBindings.end();
                 itBinding++)
            {
                (*itBinding)((*itSource)->createInputContext());
            }
            return;
        }
    }
}