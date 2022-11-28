#include <cubos/core/data/handle.hpp>

using namespace cubos::core::data;

Handle::Handle(std::nullptr_t p)
{
    this->type = nullptr;
    this->refCount = nullptr;
    this->data = nullptr;
    this->id = nullptr;
}

Handle::Handle(Handle&& rhs)
{
    this->type = rhs.type;
    this->refCount = rhs.refCount;
    this->data = rhs.data;
    this->id = rhs.id;

    rhs.type = nullptr;
    rhs.refCount = nullptr;
    rhs.data = nullptr;
    rhs.id = nullptr;
}

Handle::Handle(const Handle& rhs)
{
    this->type = rhs.type;
    this->refCount = rhs.refCount;
    this->data = rhs.data;
    this->id = rhs.id;

    if (this->refCount != nullptr)
    {
        ++(*this->refCount);
    }
}

Handle::Handle(const char* type, size_t* refCount, void* data, const std::string* id)
{
    this->type = type;
    this->refCount = refCount;
    this->data = data;
    this->id = id;

    if (this->refCount != nullptr)
    {
        ++(*this->refCount);
    }
}

Handle::~Handle()
{
    if (this->refCount != nullptr)
    {
        --(*this->refCount);
    }
}

Handle::operator bool() const
{
    return this->isValid();
}

Handle& Handle::operator=(const Handle& rhs)
{
    if (this->refCount != rhs.refCount)
    {
        if (this->refCount != nullptr)
        {
            --(*this->refCount);
        }

        this->type = rhs.type;
        this->refCount = rhs.refCount;
        this->data = rhs.data;
        this->id = rhs.id;

        if (this->refCount != nullptr)
        {
            ++(*this->refCount);
        }
    }

    return *this;
}

bool Handle::isValid() const
{
    return this->refCount != nullptr;
}

const void* Handle::getRaw() const
{
    return this->data;
}

const std::string& Handle::getId() const
{
    if (this->data == nullptr)
    {
        core::logCritical("Handle::getId(): can't get identifier since the handle is null");
        abort();
    }
    else if (this->id == nullptr)
    {
        core::logCritical("Handle::getId(): can't get identifier since the handle doesn't have one");
        abort();
    }

    return *this->id;
}

void cubos::core::data::serialize(Serializer& serializer, const Handle& handle, Handle::SerContext ctx,
                                  const char* name)
{
    ctx(serializer, handle, name);
}

void cubos::core::data::deserialize(Deserializer& deserializer, Handle& handle, Handle::DesContext ctx)
{
    ctx(deserializer, handle);
}
