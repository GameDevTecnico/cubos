#ifndef CUBOS_INPUT_BINDING_HPP
#define CUBOS_INPUT_BINDING_HPP

#include <memory>
#include <cubos/input/input_manager.hpp>

namespace cubos::input
{

    class Binding : public std::enable_shared_from_this<Binding>
    {
    public:
        Binding(std::shared_ptr<ActionMapping> mapping);
        virtual void Register();
        virtual int GetValue() = 0;

    private:
        std::shared_ptr<ActionMapping> mapping;
    };

    class ButtonBinding : public Binding
    {
    public:
        ButtonBinding(std::shared_ptr<ActionMapping> mapping, cubos::io::Key key);
        ButtonBinding(std::shared_ptr<ActionMapping> mapping, cubos::io::MouseButton mouseButton);
        int GetValue()
        {
            return 1;
        }
    };

    class AxisBinding : public Binding
    {
    public:
        AxisBinding(std::shared_ptr<ActionMapping> mapping, cubos::io::Key keyPos, cubos::io::Key keyNeg);
        int GetValue()
        {
            return 1;
        }
    };

} // namespace cubos::input

#endif // CUBOS_INPUT_BINDING_HPP