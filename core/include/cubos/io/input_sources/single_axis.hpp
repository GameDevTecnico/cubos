#ifndef CUBOS_IO_SINGLE_AXIS_HPP
#define CUBOS_IO_SINGLE_AXIS_HPP

#include <cubos/io/window.hpp>
#include <cubos/io/input_sources/input_source.hpp>
#include <variant>
#include <tuple>

namespace cubos::io
{

    class SingleAxis : public InputSource
    {

    public:
        SingleAxis(cubos::io::MouseAxis axis);
        SingleAxis(cubos::io::Key negativeKey, cubos::io::Key positiveKey);
        bool isTriggered() override;
        void subscribeEvents(cubos::io::Window* window) override;
        void unsubscribeEvents(cubos::io::Window* window) override;
        InputContext createInputContext() override;

    private:
        std::variant<cubos::io::MouseAxis, std::tuple<cubos::io::Key, cubos::io::Key>> inputs;
        void handleAxis(float value);
        void handlePositive();
        void handleNegative();
        bool wasTriggered;
        float value;
    };
} // namespace cubos::io

#endif // CUBOS_IO_SINGLE_AXIS_HPP