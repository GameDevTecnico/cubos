#include <array>
#include <cctype>
#include <cmath>
#include <cstring>

#include <cubos/core/log.hpp>
#include <cubos/core/memory/standard_stream.hpp>
#include <cubos/core/memory/stream.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using namespace cubos::core::memory;

// Digits used for printing
static const char* const DIGITS = "0123456789ABCDEF";

// Standard streams

static StandardStream stdInI(stdin);
static StandardStream stdOutI(stdout);
static StandardStream stdErrI(stderr);
Stream& Stream::stdIn = stdInI;
Stream& Stream::stdOut = stdOutI;
Stream& Stream::stdErr = stdErrI;

char Stream::get()
{
    char c = '\0';
    this->read(&c, sizeof(c));
    return c;
}

void Stream::put(char c)
{
    this->write(&c, sizeof(c));
}

void Stream::print(int64_t value, std::size_t base)
{
    if (value < 0)
    {
        value = -value;
        this->put('-');
    }

    this->print(static_cast<uint64_t>(value), base);
}

void Stream::print(uint64_t value, std::size_t base)
{
    // Validate input
    if (base > 16)
    {
        base = 10;
        CUBOS_WARN("Base must be <= 16, defaulting to base 10");
    }
    else if (base < 2)
    {
        base = 10;
        CUBOS_WARN("Base must be >= 2, defaulting to base 10");
    }

    if (value == 0)
    {
        this->put('0');
        return;
    }

    char buffer[64];     // 64 is enough for any 64-bit integer.
    std::size_t len = 0; // Number of characters used

    // While value is not zero
    while (value > 0)
    {
        buffer[len++] = DIGITS[value % base]; // Get the next digit
        value /= base;                        // Remove the digit
    }

    // Reverse the string
    for (std::size_t i = 0; i < len / 2; ++i)
    {
        char tmp = buffer[i];
        buffer[i] = buffer[len - i - 1];
        buffer[len - i - 1] = tmp;
    }

    this->print(buffer, len);
}

void Stream::print(float value, std::size_t decimalPlaces)
{
    this->print(static_cast<double>(value), decimalPlaces);
}

void Stream::print(double value, std::size_t decimalPlaces)
{
    constexpr std::size_t MaxDecimalPlaces = 32;

    if (value < 0.0)
    {
        this->put('-');
        value = -value;
    }
    else if (value == 0.0)
    {
        this->put('0');
        if (decimalPlaces > 0)
        {
            this->put('.');
            for (std::size_t i = 0; i < decimalPlaces; ++i)
            {
                this->put('0');
            }
        }
        return;
    }

    // Validate input
    if (decimalPlaces > MaxDecimalPlaces)
    {
        decimalPlaces = MaxDecimalPlaces;
        CUBOS_WARN("decimalPlaces must be <= {}, defaulting to {}", MaxDecimalPlaces, MaxDecimalPlaces);
    }

    if (value == INFINITY)
    {
        this->print("INF");
    }
    else if (value != value)
    {
        this->print("NAN");
        // 16777217 is the last number whose integer part can be represented exactly
    }
    else if (value <= 16777217.0 && log10(value) > -static_cast<double>(decimalPlaces))
    {
        char buffer[MaxDecimalPlaces + 1];
        std::size_t len = 0;

        // Print the integer part
        auto integerPart = static_cast<uint64_t>(value);
        this->print(integerPart);

        if (decimalPlaces == 0)
        {
            return;
        }

        // Print the decimal part
        buffer[len++] = '.';
        value -= static_cast<double>(integerPart);
        for (std::size_t i = 0; i < decimalPlaces; ++i)
        {
            value *= 10.0;
            auto decimalPart = static_cast<uint64_t>(value);
            buffer[len++] = DIGITS[decimalPart];
            value -= static_cast<double>(decimalPart);
        }

        this->print(buffer, len);
    }
    // If the number's integer part can't be represented exactly, print it in scientific notation
    else
    {
        // Print the integer part
        auto exponent = static_cast<int64_t>(floor(log10(value)));
        double fractional = value / pow(10.0, static_cast<double>(exponent));

        if (fractional > 10.0)
        {
            exponent += 1;
            fractional /= 10.0;
        }
        else if (fractional <= 0.1)
        {
            exponent -= 1;
            fractional *= 10.0;
        }

        // Print fractional part
        this->print(fractional, decimalPlaces);

        if (exponent > 0)
        {
            this->print("E+");
            this->print(exponent);
        }
        else
        {
            this->put('E'); // Sign here isn't required since it will be added by the print(int64_t) function
            this->print(exponent);
        }
    }
}

void Stream::print(const char* str)
{
    this->write(str, strlen(str));
}

void Stream::print(const char* str, std::size_t size)
{
    this->write(str, size);
}

void Stream::print(const std::string& str)
{
    this->write(str.c_str(), str.length());
}

void Stream::parse(int8_t& value, std::size_t base)
{
    int64_t v = 0;
    this->parse(v, base);
    if (v < INT8_MIN || v > INT8_MAX)
    {
        value = 0;
        CUBOS_WARN("Value {} out of range, defaulting to 0", v);
    }
    else
    {
        value = static_cast<int8_t>(v);
    }
}

void Stream::parse(int16_t& value, std::size_t base)
{
    int64_t v = 0;
    this->parse(v, base);
    if (v < INT16_MIN || v > INT16_MAX)
    {
        value = 0;
        CUBOS_WARN("Value {} out of range, defaulting to 0", v);
    }
    else
    {
        value = static_cast<int16_t>(v);
    }
}

void Stream::parse(int32_t& value, std::size_t base)
{
    int64_t v = 0;
    this->parse(v, base);
    if (v < INT32_MIN || v > INT32_MAX)
    {
        value = 0;
        CUBOS_WARN("Value {} out of range, defaulting to 0", v);
    }
    else
    {
        value = static_cast<int32_t>(v);
    }
}

void Stream::parse(int64_t& value, std::size_t base)
{
    // Ignore whitespace
    while (isspace(this->peek()) != 0)
    {
        this->get();
    }

    // Check sign
    bool negative = false;
    if (this->peek() == '-' || this->peek() == '+')
    {
        negative = this->get() == '-';
    }

    uint64_t v = 0;
    this->parse(v, base);
    if (v > INT64_MAX)
    {
        value = 0;
        CUBOS_WARN("Value {} out of range, defaulting to 0", v);
    }
    else if (negative)
    {
        value = -static_cast<int64_t>(v);
    }
    else
    {
        value = +static_cast<int64_t>(v);
    }
}

void Stream::parse(uint8_t& value, std::size_t base)
{
    uint64_t v = 0;
    this->parse(v, base);
    if (v > UINT8_MAX)
    {
        value = 0;
        CUBOS_WARN("Value {} out of range, defaulting to 0", v);
    }
    else
    {
        value = static_cast<uint8_t>(v);
    }
}

void Stream::parse(uint16_t& value, std::size_t base)
{
    uint64_t v = 0;
    this->parse(v, base);
    if (v > UINT16_MAX)
    {
        value = 0;
        CUBOS_WARN("Value {} out of range, defaulting to 0", v);
    }
    else
    {
        value = static_cast<uint16_t>(v);
    }
}

void Stream::parse(uint32_t& value, std::size_t base)
{
    uint64_t v = 0;
    this->parse(v, base);
    if (v > UINT32_MAX)
    {
        value = 0;
        CUBOS_WARN("Value {} out of range, defaulting to 0", v);
    }
    else
    {
        value = static_cast<uint32_t>(v);
    }
}

void Stream::parse(uint64_t& value, std::size_t base)
{
    // Validate input
    if (base > 16)
    {
        base = 10;
        CUBOS_WARN("Base must be <= 16, defaulting to base 10");
    }
    else if (base < 2)
    {
        base = 10;
        CUBOS_WARN("Base must be >= 2, defaulting to base 10");
    }

    // Ignore whitespace
    while (isspace(this->peek()) != 0)
    {
        this->get();
    }

    // Parse the number
    uint64_t v = 0;
    char c;

    while (true)
    {
        c = this->peek();
        if (c >= '0' && c <= '9')
        {
            if (c - '0' >= static_cast<char>(base))
            {
                CUBOS_WARN("{} is not a base {} digit, defaulting to 0", c, base);
                value = 0;
                return;
            }

            v *= base;
            v += static_cast<uint64_t>(c - '0');
            this->get();
        }
        // For bases higher than 10
        else if (c >= 'a' && c <= 'a' + static_cast<char>(base - 10))
        {
            v *= base;
            v += static_cast<uint64_t>(c - 'a' + 10);
            this->get();
        }
        // For bases higher than 10
        else if (c >= 'A' && c <= 'A' + static_cast<char>(base - 10))
        {
            v *= base;
            v += static_cast<uint64_t>(c - 'A' + 10);
            this->get();
        }
        else
        {
            break;
        }
    }

    value = v;
}

void Stream::parse(float& value)
{
    double v = NAN;
    this->parse(v);
    value = static_cast<float>(v);
}

void Stream::parse(double& value)
{
    // Ignore whitespace
    while (isspace(this->peek()) != 0)
    {
        this->get();
    }

    // Check sign
    bool negative = false;
    if (this->peek() == '-' || this->peek() == '+')
    {
        negative = this->get() == '-';
    }

    // Parse the integer part
    double v = 0.0;
    char c;
    while (true)
    {
        c = this->peek();
        if (c >= '0' && c <= '9')
        {
            v *= 10.0;
            v += static_cast<double>(c - '0');
            this->get();
        }
        else
        {
            break;
        }
    }

    // Parse the decimal point
    if (this->peek() == '.')
    {
        this->get();

        double divisor = 10.0;

        while (true)
        {
            c = this->peek();
            if (c >= '0' && c <= '9')
            {
                v += static_cast<double>(c - '0') / divisor;
                divisor *= 10.0;
                this->get();
            }
            else
            {
                break;
            }
        }
    }

    // Check for exponent
    if (this->peek() == 'E' || this->peek() == 'e')
    {
        this->get();

        // Parse the exponent
        int64_t exponent = 0;
        this->parse(exponent);

        // Apply exponent
        v *= pow(10.0, static_cast<double>(exponent));
    }

    value = negative ? -v : v;
}

void Stream::readUntil(std::string& str, const char* terminator)
{
    if (terminator == nullptr)
    {
        terminator = "";
    }
    str = "";

    std::size_t terminatorI = 0;

    for (char c = this->get(); !this->eof() && c != '\0'; c = this->get())
    {
        if (c == terminator[terminatorI])
        {
            ++terminatorI;
            if (terminator[terminatorI] == '\0')
            {
                break;
            }
        }
        else
        {
            if (terminatorI > 0)
            {
                str.insert(str.end(), terminator, terminator + terminatorI);
            }
            str += c;
            terminatorI = 0;
        }
    }
}

std::size_t Stream::readUntil(char* buffer, std::size_t size, const char* terminator)
{
    if (terminator == nullptr)
    {
        terminator = "";
    }

    std::size_t terminatorI = 0;
    std::size_t position = 0;

    for (char c = this->get(); !this->eof() && c != '\0' && position < size; c = this->get())
    {
        if (c == terminator[terminatorI])
        {
            ++terminatorI;
            if (terminator[terminatorI] == '\0')
            {
                break;
            }
        }
        else if (terminatorI > 0)
        {
            if (position + terminatorI < size)
            {
                memcpy(buffer + position, terminator, terminatorI);
                position += terminatorI;
            }
            else
            {
                memcpy(buffer + position, terminator, size - position);
                position = size;
            }

            terminatorI = 0;
        }
        else
        {
            buffer[position++] = c;
        }
    }

    return position;
}

void Stream::ignore(std::size_t size)
{
    for (std::size_t i = 0; i < size; ++i)
    {
        this->get();
    }
}

bool Stream::writeExact(const void* data, std::size_t size)
{
    const char* dataPtr = static_cast<const char*>(data);
    std::size_t totalBytesWritten = 0;

    while (totalBytesWritten < size)
    {
        auto bytesWritten = write(dataPtr + totalBytesWritten, size - totalBytesWritten);
        if (bytesWritten <= 0)
        {
            return false;
        }

        totalBytesWritten += bytesWritten;
    }

    return true;
}

bool Stream::readExact(void* data, std::size_t size)
{
    char* dataPtr = static_cast<char*>(data);
    std::size_t totalBytesRead = 0;

    while (totalBytesRead < size)
    {
        auto bytesRead = read(dataPtr + totalBytesRead, size - totalBytesRead);
        if (bytesRead <= 0)
        {
            return false;
        }

        totalBytesRead += bytesRead;
    }

    return true;
}