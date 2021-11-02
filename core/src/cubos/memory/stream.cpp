#include <cubos/memory/stream.hpp>

#include <cubos/log.hpp>
#include <cubos/memory/std_stream.hpp>

#include <cctype>

using namespace cubos::memory;

// Digits used for printing
static const char* const DIGITS = "0123456789ABCDEF";

// Standard streams

static StdStream stdInI(stdin);
static StdStream stdOutI(stdout);
static StdStream stdErrI(stderr);
Stream& Stream::stdIn = stdInI;
Stream& Stream::stdOut = stdOutI;
Stream& Stream::stdErr = stdErrI;

char Stream::get()
{
    char c;
    this->read(&c, sizeof(c));
    return c;
}

void Stream::put(char c)
{
    this->write(&c, sizeof(c));
}

void Stream::print(int8_t value, size_t base)
{
    this->print(static_cast<int64_t>(value), base);
}

void Stream::print(int16_t value, size_t base)
{
    this->print(static_cast<int64_t>(value), base);
}

void Stream::print(int32_t value, size_t base)
{
    this->print(static_cast<int64_t>(value), base);
}

void Stream::print(int64_t value, size_t base)
{
    if (value < 0)
    {
        value = -value;
        this->put('-');
    }

    this->print(static_cast<uint64_t>(value), base);
}

void Stream::print(uint8_t value, size_t base)
{
    this->print(static_cast<uint64_t>(value), base);
}

void Stream::print(uint16_t value, size_t base)
{
    this->print(static_cast<uint64_t>(value), base);
}

void Stream::print(uint32_t value, size_t base)
{
    this->print(static_cast<uint64_t>(value), base);
}

void Stream::print(uint64_t value, size_t base)
{
    // Validate input
    if (base > 16)
    {
        base = 10;
        logWarning("Stream::print(integer): base must be <= 16, defaulting to base 10");
    }
    else if (base < 2)
    {
        base = 10;
        logWarning("Stream::print(integer): base must be >= 2, defaulting to base 10");
    }

    if (value == 0)
    {
        this->put('0');
        return;
    }

    char buffer[64]; // 64 is enough for any 64-bit integer.
    size_t len = 0;  // Number of characters used

    // While value is not zero
    while (value > 0)
    {
        buffer[len++] = DIGITS[value % base]; // Get the next digit
        value /= base;                        // Remove the digit
    }

    // Reverse the string
    for (size_t i = 0; i < len / 2; ++i)
    {
        char tmp = buffer[i];
        buffer[i] = buffer[len - i - 1];
        buffer[len - i - 1] = tmp;
    }

    this->print(buffer, len);
}

void Stream::print(float value, size_t decimalPlaces)
{
    this->print(static_cast<double>(value, decimalPlaces));
}

void Stream::print(double value, size_t decimalPlaces)
{
    constexpr size_t MAX_DECIMAL_PLACES = 32;

    if (value < 0.0)
    {
        this->put('-');
        value = -value;
    }

    // Validate input
    if (decimalPlaces > MAX_DECIMAL_PLACES)
    {
        decimalPlaces = MAX_DECIMAL_PLACES;
        logWarning("Stream::print(double): decimalPlaces must be <= {}, defaulting to {}", MAX_DECIMAL_PLACES,
                   MAX_DECIMAL_PLACES);
    }

    if (value == INFINITY)
        this->print("INF");
    else if (value != value)
        this->print("NAN");
    // 16777217 is the last number whose integer part can be represented exactly
    else if (value <= 16777217.0 && log10(value) > -static_cast<double>(decimalPlaces))
    {
        char buffer[MAX_DECIMAL_PLACES + 1];
        size_t len = 0;

        // Print the integer part
        uint64_t integerPart = static_cast<uint64_t>(value);
        this->print(integerPart);

        if (decimalPlaces == 0)
            return;

        // Print the decimal part
        buffer[len++] = '.';
        value -= static_cast<double>(integerPart);
        for (size_t i = 0; i < decimalPlaces; ++i)
        {
            value *= 10.0;
            uint64_t decimalPart = static_cast<uint64_t>(value);
            buffer[len++] = DIGITS[decimalPart];
            value -= static_cast<double>(decimalPart);
        }

        this->print(buffer, len);
    }
    // If the number's integer part can't be represented exactly, print it in scientific notation
    else
    {
        // Print the integer part
        int64_t exponent = static_cast<uint64_t>(floor(log10(value)));
        double fractional = value / pow(10.0, exponent);

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

void Stream::print(const char* str, size_t length)
{
    this->write(str, length);
}

void Stream::print(const std::string& str)
{
    this->write(str.c_str(), str.length());
}

void Stream::parse(int8_t& value, size_t base)
{
    int64_t v = 0;
    this->parse(v, base);
    if (v < INT8_MIN || v > INT8_MAX)
    {
        value = 0;
        logWarning("Stream::parse(int8_t): value out of range, defaulting to 0");
    }
    else
        value = static_cast<int8_t>(v);
}

void Stream::parse(int16_t& value, size_t base)
{
    int64_t v = 0;
    this->parse(v, base);
    if (v < INT16_MIN || v > INT16_MAX)
    {
        value = 0;
        logWarning("Stream::parse(int16_t): value out of range, defaulting to 0");
    }
    else
        value = static_cast<int16_t>(v);
}

void Stream::parse(int32_t& value, size_t base)
{
    int64_t v = 0;
    this->parse(v, base);
    if (v < INT32_MIN || v > INT32_MAX)
    {
        value = 0;
        logWarning("Stream::parse(int32_t): value out of range, defaulting to 0");
    }
    else
        value = static_cast<int32_t>(v);
}

void Stream::parse(int64_t& value, size_t base)
{
    // Ignore whitespace
    while (isspace(this->peek()))
        this->get();

    // Check sign
    bool negative = false;
    if (this->peek() == '-' || this->peek() == '+')
        negative = this->get() == '-';

    uint64_t v = 0;
    this->parse(v, base);
    if (v > INT64_MAX)
    {
        value = 0;
        logWarning("Stream::parse(int64_t): value out of range, defaulting to 0");
    }
    else if (negative)
        value = -static_cast<int64_t>(v);
    else
        value = +static_cast<int64_t>(v);
}

void Stream::parse(uint8_t& value, size_t base)
{
    uint64_t v = 0;
    this->parse(v, base);
    if (v > UINT8_MAX)
    {
        value = 0;
        logWarning("Stream::parse(uint8_t): value out of range, defaulting to 0");
    }
    else
        value = static_cast<uint8_t>(v);
}

void Stream::parse(uint16_t& value, size_t base)
{
    uint64_t v = 0;
    this->parse(v, base);
    if (v > UINT16_MAX)
    {
        value = 0;
        logWarning("Stream::parse(uint16_t): value out of range, defaulting to 0");
    }
    else
        value = static_cast<uint16_t>(v);
}

void Stream::parse(uint32_t& value, size_t base)
{
    uint64_t v = 0;
    this->parse(v, base);
    if (v > UINT32_MAX)
    {
        value = 0;
        logWarning("Stream::parse(uint32_t): value out of range, defaulting to 0");
    }
    else
        value = static_cast<uint32_t>(v);
}

void Stream::parse(uint64_t& value, size_t base)
{
    // Validate input
    if (base > 16)
    {
        base = 10;
        logWarning("Stream::parse(integer): base must be <= 16, defaulting to base 10");
    }
    else if (base < 2)
    {
        base = 10;
        logWarning("Stream::parse(integer): base must be >= 2, defaulting to base 10");
    }

    // Ignore whitespace
    while (isspace(this->peek()))
        this->get();

    // Parse the number
    uint64_t v = 0;
    char c;

    while (true)
    {
        c = this->peek();
        if (c >= '0' && c <= '9')
        {
            if (c - '0' >= base)
            {
                logWarning("Stream::parse(uint64_t): digit out of range, defaulting to 0");
                value = 0;
                return;
            }

            v *= base;
            v += c - '0';
            this->get();
        }
        // For bases higher than 10
        else if (c >= 'a' && c <= 'a' + (base - 10))
        {
            v *= base;
            v += c - 'a' + 10;
            this->get();
        }
        // For bases higher than 10
        else if (c >= 'A' && c <= 'A' + (base - 10))
        {
            v *= base;
            v += c - 'A' + 10;
            this->get();
        }
        else
            break;
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
    while (isspace(this->peek()))
        this->get();

    // Check sign
    bool negative = false;
    if (this->peek() == '-' || this->peek() == '+')
        negative = this->get() == '-';

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
            break;
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
                break;
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
