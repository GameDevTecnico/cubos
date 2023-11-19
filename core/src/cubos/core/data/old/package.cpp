#include <iterator>

#include <cubos/core/data/old/package.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/string.hpp>

using namespace cubos::core::data::old;

Package::Package(Type type)
{
    switch (type)
    {
    case Type::None:
        mData = std::monostate();
        break;
    case Type::I8:
        mData = static_cast<int8_t>(0);
        break;
    case Type::I16:
        mData = static_cast<int16_t>(0);
        break;
    case Type::I32:
        mData = static_cast<int32_t>(0);
        break;
    case Type::I64:
        mData = static_cast<int64_t>(0);
        break;
    case Type::U8:
        mData = static_cast<uint8_t>(0);
        break;
    case Type::U16:
        mData = static_cast<uint16_t>(0);
        break;
    case Type::U32:
        mData = static_cast<uint32_t>(0);
        break;
    case Type::U64:
        mData = static_cast<uint64_t>(0);
        break;
    case Type::F32:
        mData = static_cast<float>(0);
        break;
    case Type::F64:
        mData = static_cast<double>(0);
        break;
    case Type::Bool:
        mData = false;
        break;
    case Type::String:
        mData = std::string();
        break;
    case Type::Object:
        mData = Fields();
        break;
    case Type::Array:
        mData = Elements();
        break;
    case Type::Dictionary:
        mData = Dictionary();
        break;
    default:
        abort();
    }
}

bool Package::change(int64_t data)
{
    switch (this->type())
    {
    case Type::I8:
        mData = static_cast<int8_t>(data);
        return true;
    case Type::I16:
        mData = static_cast<int16_t>(data);
        return true;
    case Type::I32:
        mData = static_cast<int32_t>(data);
        return true;
    case Type::I64:
        mData = data;
        return true;
    case Type::U8:
        if (data < 0 || data > UINT8_MAX)
        {
            return false;
        }
        mData = static_cast<uint8_t>(data);
        return true;
    case Type::U16:
        if (data < 0 || data > UINT16_MAX)
        {
            return false;
        }
        mData = static_cast<uint16_t>(data);
        return true;
    case Type::U32:
        if (data < 0 || data > UINT32_MAX)
        {
            return false;
        }
        mData = static_cast<uint32_t>(data);
        return true;
    case Type::U64:
        if (data < 0)
        {
            return false;
        }
        mData = static_cast<uint64_t>(data);
        return true;
    default:
        return false;
    }
}

bool Package::change(uint64_t data)
{
    switch (this->type())
    {
    case Type::I8:
        if (data > INT8_MAX)
        {
            return false;
        }
        mData = static_cast<int8_t>(data);
        return true;
    case Type::I16:
        if (data > INT16_MAX)
        {
            return false;
        }
        mData = static_cast<int16_t>(data);
        return true;
    case Type::I32:
        if (data > INT32_MAX)
        {
            return false;
        }
        mData = static_cast<int32_t>(data);
        return true;
    case Type::I64:
        if (data > INT64_MAX)
        {
            return false;
        }
        mData = static_cast<int64_t>(data);
        return true;
    case Type::U8:
        if (data > UINT8_MAX)
        {
            return false;
        }
        mData = static_cast<uint8_t>(data);
        return true;
    case Type::U16:
        if (data > UINT16_MAX)
        {
            return false;
        }
        mData = static_cast<uint16_t>(data);
        return true;
    case Type::U32:
        if (data > UINT32_MAX)
        {
            return false;
        }
        mData = static_cast<uint32_t>(data);
        return true;
    case Type::U64:
        mData = data;
        return true;
    default:
        return false;
    }
}

bool Package::change(double data)
{
    switch (this->type())
    {
    case Type::F32:
        mData = static_cast<float>(data);
        return true;
    case Type::F64:
        mData = data;
        return true;
    default:
        return false;
    }
}

bool Package::change(bool data)
{
    switch (this->type())
    {
    case Type::Bool:
        mData = data;
        return true;
    default:
        return false;
    }
}

bool Package::change(const std::string& data)
{
    switch (this->type())
    {
    case Type::String:
        mData = data;
        return true;
    default:
        return false;
    }
}

Package::Type Package::type() const
{
    return static_cast<Package::Type>(mData.index());
}

std::size_t Package::size() const
{
    switch (this->type())
    {
    case Type::None:
        return 0;
    case Type::Object:
        return std::get<Fields>(mData).size();
    case Type::Array:
        return std::get<Elements>(mData).size();
    case Type::Dictionary:
        return std::get<Dictionary>(mData).size();
    default:
        return 1; // Its a scalar.
    }
}

bool Package::isStructured() const
{
    switch (this->type())
    {
    case Type::Object:
    case Type::Array:
    case Type::Dictionary:
        return true;
    default:
        return false;
    }
}

Package& Package::field(const std::string& name)
{
    auto& fields = this->fields();
    for (auto& field : fields)
    {
        if (field.first == name)
        {
            return field.second;
        }
    }

    CUBOS_CRITICAL("No field '{}' in package", name);
    abort();
}

Package::Fields& Package::fields()
{
    return std::get<Fields>(mData);
}

const Package::Fields& Package::fields() const
{
    return std::get<Fields>(mData);
}

Package Package::removeField(std::string_view name)
{
    auto& fields = this->fields();
    for (size_t i = 0; i < fields.size(); ++i)
    {
        if (fields[i].first == name)
        {
            auto field = std::move(fields[i].second);
            fields.erase(fields.begin() + static_cast<ptrdiff_t>(i));
            return field;
        }
    }

    return {};
}

Package& Package::element(std::size_t index)
{
    auto& elements = this->elements();
    return elements[index];
}

Package::Elements& Package::elements()
{
    return std::get<Elements>(mData);
}

const Package::Elements& Package::elements() const
{
    return std::get<Elements>(mData);
}

Package::Dictionary& Package::dictionary()
{
    return std::get<Dictionary>(mData);
}

const Package::Dictionary& Package::dictionary() const
{
    return std::get<Dictionary>(mData);
}

const char* Package::typeToString(Type type)
{
    switch (type)
    {
    case Type::None:
        return "None";
    case Type::I8:
        return "I8";
    case Type::I16:
        return "I16";
    case Type::I32:
        return "I32";
    case Type::I64:
        return "I64";
    case Type::U8:
        return "U8";
    case Type::U16:
        return "U16";
    case Type::U32:
        return "U32";
    case Type::U64:
        return "U64";
    case Type::F32:
        return "F32";
    case Type::F64:
        return "F64";
    case Type::Bool:
        return "Bool";
    case Type::String:
        return "String";
    case Type::Object:
        return "Object";
    case Type::Array:
        return "Array";
    case Type::Dictionary:
        return "Dictionary";
    default:
        return "Unknown";
    }
}

void Package::serialize(Serializer& ser, const char* name) const
{
    switch (this->type())
    {
    case Package::Type::None:
        abort(); // Empty packages can't be serialized.
    case Package::Type::I8:
        ser.writeI8(std::get<int8_t>(mData), name);
        break;
    case Package::Type::I16:
        ser.writeI16(std::get<int16_t>(mData), name);
        break;
    case Package::Type::I32:
        ser.writeI32(std::get<int32_t>(mData), name);
        break;
    case Package::Type::I64:
        ser.writeI64(std::get<int64_t>(mData), name);
        break;
    case Package::Type::U8:
        ser.writeU8(std::get<uint8_t>(mData), name);
        break;
    case Package::Type::U16:
        ser.writeU16(std::get<uint16_t>(mData), name);
        break;
    case Package::Type::U32:
        ser.writeU32(std::get<uint32_t>(mData), name);
        break;
    case Package::Type::U64:
        ser.writeU64(std::get<uint64_t>(mData), name);
        break;
    case Package::Type::F32:
        ser.writeF32(std::get<float>(mData), name);
        break;
    case Package::Type::F64:
        ser.writeF64(std::get<double>(mData), name);
        break;
    case Package::Type::Bool:
        ser.writeBool(std::get<bool>(mData), name);
        break;
    case Package::Type::String:
        ser.writeString(std::get<std::string>(mData).c_str(), name);
        break;
    case Package::Type::Object: {
        const auto& fields = std::get<Package::Fields>(mData);
        ser.beginObject(name);
        for (const auto& field : fields)
        {
            ser.write(field.second, field.first.c_str());
        }
        ser.endObject();
        break;
    }
    case Package::Type::Array: {
        const auto& elements = std::get<Package::Elements>(mData);
        ser.beginArray(elements.size(), name);
        for (const auto& element : elements)
        {
            ser.write(element, nullptr);
        }
        ser.endArray();
        break;
    }
    case Package::Type::Dictionary: {
        const auto& dictionary = std::get<Package::Dictionary>(mData);
        ser.beginDictionary(dictionary.size(), name);
        for (const auto& entry : dictionary)
        {
            ser.write(entry.first, nullptr);
            ser.write(entry.second, nullptr);
        }
        ser.endDictionary();
        break;
    }
    }
}

impl::Packager::Packager(Package& pkg)
    : mPkg(pkg)
{
    // Do nothing.
}

void impl::Packager::writeI8(int8_t value, const char* name)
{
    this->push(value, name);
}

void impl::Packager::writeI16(int16_t value, const char* name)
{
    this->push(value, name);
}

void impl::Packager::writeI32(int32_t value, const char* name)
{
    this->push(value, name);
}

void impl::Packager::writeI64(int64_t value, const char* name)
{
    this->push(value, name);
}

void impl::Packager::writeU8(uint8_t value, const char* name)
{
    this->push(value, name);
}

void impl::Packager::writeU16(uint16_t value, const char* name)
{
    this->push(value, name);
}

void impl::Packager::writeU32(uint32_t value, const char* name)
{
    this->push(value, name);
}

void impl::Packager::writeU64(uint64_t value, const char* name)
{
    this->push(value, name);
}

void impl::Packager::writeF32(float value, const char* name)
{
    this->push(value, name);
}

void impl::Packager::writeF64(double value, const char* name)
{
    this->push(value, name);
}

void impl::Packager::writeBool(bool value, const char* name)
{
    this->push(value, name);
}

void impl::Packager::writeString(const char* value, const char* name)
{
    this->push(std::string(value), name);
}

void impl::Packager::beginObject(const char* name)
{
    mStack.push({this->push(Package::Fields(), name), false});
}

void impl::Packager::endObject()
{
    CUBOS_ASSERT(!mStack.empty());
    mStack.pop();
}

void impl::Packager::beginArray(std::size_t /*length*/, const char* name)
{
    mStack.push({this->push(Package::Elements(), name), false});
}

void impl::Packager::endArray()
{
    CUBOS_ASSERT(!mStack.empty());
    mStack.pop();
}

void impl::Packager::beginDictionary(std::size_t /*length*/, const char* name)
{
    mStack.push({this->push(Package::Dictionary(), name), true});
}

void impl::Packager::endDictionary()
{
    CUBOS_ASSERT(!mStack.empty());
    mStack.pop();
}

Package* impl::Packager::push(Package::Data&& data, const char* name)
{
    if (mStack.empty())
    {
        mPkg.mData = std::move(data);
        return &mPkg;
    }

    auto& [pkg, isKey] = mStack.top();
    switch (pkg->type())
    {
    case Package::Type::Object:
        CUBOS_ASSERT(name != nullptr);
        pkg->fields().emplace_back(name, Package());
        pkg->fields().back().second.mData = std::move(data);
        return &pkg->fields().back().second;
    case Package::Type::Array:
        pkg->elements().emplace_back();
        pkg->elements().back().mData = std::move(data);
        return &pkg->elements().back();
    case Package::Type::Dictionary:
        if (isKey)
        {
            isKey = false;
            pkg->dictionary().push_back({Package(), Package()});
            pkg->dictionary().back().first.mData = std::move(data);
            return &pkg->dictionary().back().first;
        }
        else
        {
            isKey = true;
            pkg->dictionary().back().second.mData = std::move(data);
            return &pkg->dictionary().back().second;
        }
    default:
        abort(); // Unreachable.
    }
}

Unpackager::Unpackager(const Package& pkg)
    : mPkg(pkg)
{
    // Do nothing.
}

#define UNEXPECTED(expected, found)                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        CUBOS_ERROR("Expected {}, found {}", expected, found);                                                         \
        mFailBit = true;                                                                                               \
    } while (0)

#define TYPE_MISMATCH(expected, found) UNEXPECTED(Package::typeToString(expected), Package::typeToString(found))

void Unpackager::readI8(int8_t& value)
{
    const auto* d = this->pop();
    if (d == nullptr || d->type() != Package::Type::I8)
    {
        TYPE_MISMATCH(Package::Type::I8, d->type());
    }
    else
    {
        value = std::get<int8_t>(d->mData);
    }
}

void Unpackager::readI16(int16_t& value)
{
    const auto* d = this->pop();
    if (d == nullptr || d->type() != Package::Type::I16)
    {
        TYPE_MISMATCH(Package::Type::I16, d->type());
    }
    else
    {
        value = std::get<int16_t>(d->mData);
    }
}

void Unpackager::readI32(int32_t& value)
{
    const auto* d = this->pop();
    if (d == nullptr || d->type() != Package::Type::I32)
    {
        TYPE_MISMATCH(Package::Type::I32, d->type());
    }
    else
    {
        value = std::get<int32_t>(d->mData);
    }
}

void Unpackager::readI64(int64_t& value)
{
    const auto* d = this->pop();
    if (d == nullptr || d->type() != Package::Type::I64)
    {
        TYPE_MISMATCH(Package::Type::I64, d->type());
    }
    else
    {
        value = std::get<int64_t>(d->mData);
    }
}

void Unpackager::readU8(uint8_t& value)
{
    const auto* d = this->pop();
    if (d == nullptr || d->type() != Package::Type::U8)
    {
        TYPE_MISMATCH(Package::Type::U8, d->type());
    }
    else
    {
        value = std::get<uint8_t>(d->mData);
    }
}

void Unpackager::readU16(uint16_t& value)
{
    const auto* d = this->pop();
    if (d == nullptr || d->type() != Package::Type::U16)
    {
        TYPE_MISMATCH(Package::Type::U16, d->type());
    }
    else
    {
        value = std::get<uint16_t>(d->mData);
    }
}

void Unpackager::readU32(uint32_t& value)
{
    const auto* d = this->pop();
    if (d == nullptr || d->type() != Package::Type::U32)
    {
        TYPE_MISMATCH(Package::Type::U32, d->type());
    }
    else
    {
        value = std::get<uint32_t>(d->mData);
    }
}

void Unpackager::readU64(uint64_t& value)
{
    const auto* d = this->pop();
    if (d == nullptr || d->type() != Package::Type::U64)
    {
        TYPE_MISMATCH(Package::Type::U64, d->type());
    }
    else
    {
        value = std::get<uint64_t>(d->mData);
    }
}

void Unpackager::readF32(float& value)
{
    const auto* d = this->pop();
    if (d == nullptr || d->type() != Package::Type::F32)
    {
        TYPE_MISMATCH(Package::Type::F32, d->type());
    }
    else
    {
        value = std::get<float>(d->mData);
    }
}

void Unpackager::readF64(double& value)
{
    const auto* d = this->pop();
    if (d == nullptr || d->type() != Package::Type::F64)
    {
        TYPE_MISMATCH(Package::Type::F64, d->type());
    }
    else
    {
        value = std::get<double>(d->mData);
    }
}

void Unpackager::readBool(bool& value)
{
    const auto* d = this->pop();
    if (d == nullptr || d->type() != Package::Type::Bool)
    {
        TYPE_MISMATCH(Package::Type::Bool, d->type());
    }
    else
    {
        value = std::get<bool>(d->mData);
    }
}

void Unpackager::readString(std::string& value)
{
    const auto* d = this->pop();
    if (d == nullptr || d->type() != Package::Type::String)
    {
        TYPE_MISMATCH(Package::Type::String, d->type());
    }
    else
    {
        value = std::get<std::string>(d->mData);
    }
}

void Unpackager::beginObject()
{
    const auto* d = this->pop();
    if (d == nullptr || d->type() != Package::Type::Object)
    {
        TYPE_MISMATCH(Package::Type::Object, d->type());
    }
    else
    {
        mStack.push({d, 0});
    }
}

void Unpackager::endObject()
{
    CUBOS_ASSERT(!mStack.empty());
    mStack.pop();
}

std::size_t Unpackager::beginArray()
{
    const auto* d = this->pop();
    if (d == nullptr || d->type() != Package::Type::Array)
    {
        TYPE_MISMATCH(Package::Type::Array, d->type());
        return 0;
    }

    mStack.push({d, 0});
    return d->elements().size();
}

void Unpackager::endArray()
{
    CUBOS_ASSERT(!mStack.empty());
    mStack.pop();
}

std::size_t Unpackager::beginDictionary()
{
    const auto* d = this->pop();
    if (d == nullptr || d->type() != Package::Type::Dictionary)
    {
        TYPE_MISMATCH(Package::Type::Dictionary, d->type());
        return 0;
    }

    mStack.push({d, 0});
    return d->dictionary().size();
}

void Unpackager::endDictionary()
{
    CUBOS_ASSERT(!mStack.empty());
    mStack.pop();
}

const Package* Unpackager::pop()
{
    if (mStack.empty())
    {
        return &mPkg;
    }
    auto& [pkg, index] = mStack.top();
    switch (pkg->type())
    {
    case Package::Type::Object:
        if (index >= pkg->fields().size())
        {
            return nullptr;
        }
        else
        {
            auto it = pkg->fields().begin();
            std::advance(it, index++);
            return &it->second;
        }
    case Package::Type::Array:
        if (index >= pkg->elements().size())
        {
            return nullptr;
        }

        return &pkg->elements()[index++];

    case Package::Type::Dictionary:
        if (index / 2 >= pkg->dictionary().size())
        {
            return nullptr;
        }
        if (index % 2 == 0)
        {
            return &pkg->dictionary()[(index++) / 2].first;
        }

        return &pkg->dictionary()[(index++) / 2].second;

    default:
        abort(); // Unreachable.
    }
}
