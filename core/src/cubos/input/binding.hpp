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

    protected:
        std::shared_ptr<ActionMapping> mapping;
    };

    class ButtonBinding : public Binding
    {
    public:
        ButtonBinding(std::shared_ptr<ActionMapping> mapping, cubos::io::Key key);
        ButtonBinding(std::shared_ptr<ActionMapping> mapping, cubos::io::MouseButton mouseButton);
        void handleButtonPress();
    };

    class AxisBinding : public Binding
    {
    public:
        AxisBinding(std::shared_ptr<ActionMapping> mapping, cubos::io::Key keyPos, cubos::io::Key keyNeg);
        void handlePosKey();
        void handleNegKey();
    };

} // namespace cubos::input

#endif // CUBOS_INPUT_BINDING_HPP