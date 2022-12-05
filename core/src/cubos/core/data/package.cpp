#include <cubos/core/data/package.hpp>

#include <iterator>

using namespace cubos::core::data;

Package::Package(Type type)
{
    switch (type)
    {
    case Type::None:
        this->data = std::monostate();
        break;
    case Type::I8:
        this->data = static_cast<int8_t>(0);
        break;
    case Type::I16:
        this->data = static_cast<int16_t>(0);
        break;
    case Type::I32:
        this->data = static_cast<int32_t>(0);
        break;
    case Type::I64:
        this->data = static_cast<int64_t>(0);
        break;
    case Type::U8:
        this->data = static_cast<uint8_t>(0);
        break;
    case Type::U16:
        this->data = static_cast<uint16_t>(0);
        break;
    case Type::U32:
        this->data = static_cast<uint32_t>(0);
        break;
    case Type::U64:
        this->data = static_cast<uint64_t>(0);
        break;
    case Type::F32:
        this->data = static_cast<float>(0);
        break;
    case Type::F64:
        this->data = static_cast<double>(0);
        break;
    case Type::Bool:
        this->data = false;
        break;
    case Type::String:
        this->data = std::string();
        break;
    case Type::Object:
        this->data = Fields();
        break;
    case Type::Array:
        this->data = Elements();
        break;
    case Type::Dictionary:
        this->data = Dictionary();
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
        this->data = static_cast<int8_t>(data);
        return true;
    case Type::I16:
        this->data = static_cast<int16_t>(data);
        return true;
    case Type::I32:
        this->data = static_cast<int32_t>(data);
        return true;
    case Type::I64:
        this->data = data;
        return true;
    case Type::U8:
        if (data < 0 || data > UINT8_MAX)
            return false;
        this->data = static_cast<uint8_t>(data);
        return true;
    case Type::U16:
        if (data < 0 || data > UINT16_MAX)
            return false;
        this->data = static_cast<uint16_t>(data);
        return true;
    case Type::U32:
        if (data < 0 || data > UINT32_MAX)
            return false;
        this->data = static_cast<uint32_t>(data);
        return true;
    case Type::U64:
        if (data < 0)
            return false;
        this->data = static_cast<uint64_t>(data);
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
            return false;
        this->data = static_cast<int8_t>(data);
        return true;
    case Type::I16:
        if (data > INT16_MAX)
            return false;
        this->data = static_cast<int16_t>(data);
        return true;
    case Type::I32:
        if (data > INT32_MAX)
            return false;
        this->data = static_cast<int32_t>(data);
        return true;
    case Type::I64:
        if (data > INT64_MAX)
            return false;
        this->data = static_cast<int64_t>(data);
        return true;
    case Type::U8:
        if (data > UINT8_MAX)
            return false;
        this->data = static_cast<uint8_t>(data);
        return true;
    case Type::U16:
        if (data > UINT16_MAX)
            return false;
        this->data = static_cast<uint16_t>(data);
        return true;
    case Type::U32:
        if (data > UINT32_MAX)
            return false;
        this->data = static_cast<uint32_t>(data);
        return true;
    case Type::U64:
        this->data = data;
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
        this->data = static_cast<float>(data);
        return true;
    case Type::F64:
        this->data = data;
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
        this->data = data;
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
        this->data = data;
        return true;
    default:
        return false;
    }
}

Package::Type Package::type() const
{
    return static_cast<Package::Type>(this->data.index());
}

size_t Package::size() const
{
    switch (this->type())
    {
    case Type::None:
        return 0;
    case Type::Object:
        return std::get<Fields>(this->data).size();
    case Type::Array:
        return std::get<Elements>(this->data).size();
    case Type::Dictionary:
        return std::get<Dictionary>(this->data).size();
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
    // If the field already exists, return it, otherwise create it and return it.
    auto& fields = this->fields();
    for (auto& field : fields)
    {
        if (field.first == name)
        {
            return field.second;
        }
    }

    logCritical("Package::field() failed: package doesn't contain field '{}'", name);
    abort();
}

Package::Fields& Package::fields()
{
    return std::get<Fields>(this->data);
}

const Package::Fields& Package::fields() const
{
    return std::get<Fields>(this->data);
}

Package& Package::element(size_t index)
{
    auto& elements = this->elements();
    return elements[index];
}

Package::Elements& Package::elements()
{
    return std::get<Elements>(this->data);
}

const Package::Elements& Package::elements() const
{
    return std::get<Elements>(this->data);
}

Package::Dictionary& Package::dictionary()
{
    return std::get<Dictionary>(this->data);
}

const Package::Dictionary& Package::dictionary() const
{
    return std::get<Dictionary>(this->data);
}

void cubos::core::data::serialize(Serializer& serializer, const Package& pkg, const char* name)
{
    switch (pkg.type())
    {
    case Package::Type::None:
        abort(); // Empty packages can't be serialized.
    case Package::Type::I8:
        serializer.writeI8(std::get<int8_t>(pkg.data), name);
        break;
    case Package::Type::I16:
        serializer.writeI16(std::get<int16_t>(pkg.data), name);
        break;
    case Package::Type::I32:
        serializer.writeI32(std::get<int32_t>(pkg.data), name);
        break;
    case Package::Type::I64:
        serializer.writeI64(std::get<int64_t>(pkg.data), name);
        break;
    case Package::Type::U8:
        serializer.writeU8(std::get<uint8_t>(pkg.data), name);
        break;
    case Package::Type::U16:
        serializer.writeU16(std::get<uint16_t>(pkg.data), name);
        break;
    case Package::Type::U32:
        serializer.writeU32(std::get<uint32_t>(pkg.data), name);
        break;
    case Package::Type::U64:
        serializer.writeU64(std::get<uint64_t>(pkg.data), name);
        break;
    case Package::Type::F32:
        serializer.writeF32(std::get<float>(pkg.data), name);
        break;
    case Package::Type::F64:
        serializer.writeF64(std::get<double>(pkg.data), name);
        break;
    case Package::Type::Bool:
        serializer.writeBool(std::get<bool>(pkg.data), name);
        break;
    case Package::Type::String:
        serializer.writeString(std::get<std::string>(pkg.data).c_str(), name);
        break;
    case Package::Type::Object: {
        auto& fields = std::get<Package::Fields>(pkg.data);
        serializer.beginObject(name);
        for (auto& field : fields)
        {
            serialize(serializer, field.second, field.first.c_str());
        }
        serializer.endObject();
        break;
    }
    case Package::Type::Array: {
        auto& elements = std::get<Package::Elements>(pkg.data);
        serializer.beginArray(elements.size(), name);
        for (auto& element : elements)
        {
            serialize(serializer, element, nullptr);
        }
        serializer.endArray();
        break;
    }
    case Package::Type::Dictionary: {
        auto& dictionary = std::get<Package::Dictionary>(pkg.data);
        serializer.beginDictionary(dictionary.size(), name);
        for (auto& entry : dictionary)
        {
            serialize(serializer, entry.first, nullptr);
            serialize(serializer, entry.second, nullptr);
        }
        serializer.endDictionary();
    }
    }
}

impl::Packager::Packager(Package& pkg) : pkg(pkg)
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
    this->stack.push({this->push(Package::Fields(), name), false});
}

void impl::Packager::endObject()
{
    assert(!this->stack.empty());
    this->stack.pop();
}

void impl::Packager::beginArray(size_t size, const char* name)
{
    this->stack.push({this->push(Package::Elements(), name), false});
}

void impl::Packager::endArray()
{
    assert(!this->stack.empty());
    this->stack.pop();
}

void impl::Packager::beginDictionary(size_t size, const char* name)
{
    this->stack.push({this->push(Package::Dictionary(), name), true});
}

void impl::Packager::endDictionary()
{
    assert(!this->stack.empty());
    this->stack.pop();
}

Package* impl::Packager::push(Package::Data&& data, const char* name)
{
    if (this->stack.empty())
    {
        this->pkg.data = std::move(data);
        return &this->pkg;
    }
    else
    {
        auto& [pkg, isKey] = this->stack.top();
        switch (pkg->type())
        {
        case Package::Type::Object:
            assert(name != nullptr);
            pkg->fields().emplace_back(name, Package());
            pkg->fields().back().second.data = std::move(data);
            return &pkg->fields().back().second;
        case Package::Type::Array:
            pkg->elements().emplace_back();
            pkg->elements().back().data = std::move(data);
            return &pkg->elements().back();
        case Package::Type::Dictionary:
            if (isKey)
            {
                isKey = false;
                pkg->dictionary().push_back({Package(), Package()});
                pkg->dictionary().back().first.data = std::move(data);
                return &pkg->dictionary().back().first;
            }
            else
            {
                isKey = true;
                pkg->dictionary().back().second.data = std::move(data);
                return &pkg->dictionary().back().second;
            }
        default:
            abort(); // Unreachable.
        }
    }
}

impl::Unpackager::Unpackager(const Package& pkg) : pkg(pkg)
{
    // Do nothing.
}

void impl::Unpackager::readI8(int8_t& value)
{
    auto d = this->pop();
    if (d == nullptr || d->type() != Package::Type::I8)
    {
        logError("impl::Unpackager::readI8() failed: type mismatch");
        this->failBit = true;
    }
    else
        value = std::get<int8_t>(d->data);
}

void impl::Unpackager::readI16(int16_t& value)
{
    auto d = this->pop();
    if (d == nullptr || d->type() != Package::Type::I16)
    {
        logError("impl::Unpackager::readI16() failed: type mismatch");
        this->failBit = true;
    }
    else
        value = std::get<int16_t>(d->data);
}

void impl::Unpackager::readI32(int32_t& value)
{
    auto d = this->pop();
    if (d == nullptr || d->type() != Package::Type::I32)
    {
        logError("impl::Unpackager::readI32() failed: type mismatch");
        this->failBit = true;
    }
    else
        value = std::get<int32_t>(d->data);
}

void impl::Unpackager::readI64(int64_t& value)
{
    auto d = this->pop();
    if (d == nullptr || d->type() != Package::Type::I64)
    {
        logError("impl::Unpackager::readI64() failed: type mismatch");
        this->failBit = true;
    }
    else
        value = std::get<int64_t>(d->data);
}

void impl::Unpackager::readU8(uint8_t& value)
{
    auto d = this->pop();
    if (d == nullptr || d->type() != Package::Type::U8)
    {
        logError("impl::Unpackager::readU8() failed: type mismatch");
        this->failBit = true;
    }
    else
        value = std::get<uint8_t>(d->data);
}

void impl::Unpackager::readU16(uint16_t& value)
{
    auto d = this->pop();
    if (d == nullptr || d->type() != Package::Type::U16)
    {
        logError("impl::Unpackager::readU16() failed: type mismatch");
        this->failBit = true;
    }
    else
        value = std::get<uint16_t>(d->data);
}

void impl::Unpackager::readU32(uint32_t& value)
{
    auto d = this->pop();
    if (d == nullptr || d->type() != Package::Type::U32)
    {
        logError("impl::Unpackager::readU32() failed: type mismatch");
        this->failBit = true;
    }
    else
        value = std::get<uint32_t>(d->data);
}

void impl::Unpackager::readU64(uint64_t& value)
{
    auto d = this->pop();
    if (d == nullptr || d->type() != Package::Type::U64)
    {
        logError("impl::Unpackager::readU64() failed: type mismatch");
        this->failBit = true;
    }
    else
        value = std::get<uint64_t>(d->data);
}

void impl::Unpackager::readF32(float& value)
{
    auto d = this->pop();
    if (d == nullptr || d->type() != Package::Type::F32)
    {
        logError("impl::Unpackager::readF32() failed: type mismatch");
        this->failBit = true;
    }
    else
        value = std::get<float>(d->data);
}

void impl::Unpackager::readF64(double& value)
{
    auto d = this->pop();
    if (d == nullptr || d->type() != Package::Type::F64)
    {
        logError("impl::Unpackager::readF64() failed: type mismatch");
        this->failBit = true;
    }
    else
        value = std::get<double>(d->data);
}

void impl::Unpackager::readBool(bool& value)
{
    auto d = this->pop();
    if (d == nullptr || d->type() != Package::Type::Bool)
    {
        logError("impl::Unpackager::readBool() failed: type mismatch");
        this->failBit = true;
    }
    else
        value = std::get<bool>(d->data);
}

void impl::Unpackager::readString(std::string& value)
{
    auto d = this->pop();
    if (d == nullptr || d->type() != Package::Type::String)
    {
        logError("impl::Unpackager::readString() failed: type mismatch");
        this->failBit = true;
    }
    else
        value = std::get<std::string>(d->data);
}

void impl::Unpackager::beginObject()
{
    auto d = this->pop();
    if (d == nullptr || d->type() != Package::Type::Object)
    {
        logError("impl::Unpackager::beginObject() failed: type mismatch");
        this->failBit = true;
    }
    else
        this->stack.push({d, 0});
}

void impl::Unpackager::endObject()
{
    assert(!this->stack.empty());
    this->stack.pop();
}

size_t impl::Unpackager::beginArray()
{
    auto d = this->pop();
    if (d == nullptr || d->type() != Package::Type::Array)
    {
        logError("impl::Unpackager::beginArray() failed: type mismatch");
        this->failBit = true;
        return 0;
    }
    else
    {
        this->stack.push({d, 0});
        return d->elements().size();
    }
}

void impl::Unpackager::endArray()
{
    assert(!this->stack.empty());
    this->stack.pop();
}

size_t impl::Unpackager::beginDictionary()
{
    auto d = this->pop();
    if (d == nullptr || d->type() != Package::Type::Dictionary)
    {
        logError("impl::Unpackager::beginDictionary() failed: type mismatch");
        this->failBit = true;
        return 0;
    }
    else
    {
        this->stack.push({d, 0});
        return d->dictionary().size();
    }
}

void impl::Unpackager::endDictionary()
{
    assert(!this->stack.empty());
    this->stack.pop();
}

const Package* impl::Unpackager::pop()
{
    if (this->stack.empty())
        return &this->pkg;
    else
    {
        auto& [pkg, index] = this->stack.top();
        switch (pkg->type())
        {
        case Package::Type::Object:
            if (index >= pkg->fields().size())
                return nullptr;
            else
            {
                auto it = pkg->fields().begin();
                std::advance(it, index++);
                return &it->second;
            }
        case Package::Type::Array:
            if (index >= pkg->elements().size())
                return nullptr;
            else
                return &pkg->elements()[index++];
        case Package::Type::Dictionary:
            if (index / 2 >= pkg->dictionary().size())
                return nullptr;
            else if (index % 2 == 0)
                return &pkg->dictionary()[(index++) / 2].first;
            else
                return &pkg->dictionary()[(index++) / 2].second;
        default:
            abort(); // Unreachable.
        }
    }
}
