#include <cubos/core/reflection/object.hpp>
#include <cubos/core/reflection/visitor.hpp>

using cubos::core::reflection::Hints;
using cubos::core::reflection::ObjectType;
using cubos::core::reflection::Type;

ObjectType::Field::Field(std::string name, const Type& type)
    : mName(std::move(name))
    , mType(type)
{
}

const std::string& ObjectType::Field::name() const
{
    return mName;
}

const Type& ObjectType::Field::type() const
{
    return mType;
}

const Hints& ObjectType::Field::hints() const
{
    return mHints;
}

Hints& ObjectType::Field::hints()
{
    return mHints;
}

ObjectType::FieldGetter::FieldGetter(std::string name, const Type& type, Getter getter)
    : Field(name, type)
    , mGetter(getter)
{
}

const void* ObjectType::FieldGetter::get(const void* object) const
{
    return reinterpret_cast<const void*>(mGetter(object));
}

void* ObjectType::FieldGetter::get(void* object) const
{
    return reinterpret_cast<void*>(mGetter(object));
}

ObjectType::Builder::Builder(ObjectType& type)
    : Type::Builder(type)
    , mType(type)
{
}

ObjectType::Builder& ObjectType::Builder::field(std::string name, const Type& type, FieldGetter::Getter getter)
{
    mType.mFields.emplace_back(std::make_unique<const FieldGetter>(std::move(name), type, getter));
    return *this;
}

ObjectType::Builder ObjectType::build(std::string name)
{
    return Builder{*new ObjectType{std::move(name)}};
}

const std::vector<std::unique_ptr<const ObjectType::Field>>& ObjectType::fields() const
{
    return mFields;
}

const ObjectType::Field& ObjectType::field(std::string_view name) const
{
    for (const auto& field : mFields)
    {
        if (field->name() == name)
        {
            return *field;
        }
    }

    CUBOS_FAIL("Field '{}' not found in type '{}'", name, this->name());
}

void cubos::core::reflection::ObjectType::accept(TypeVisitor& visitor) const
{
    visitor.visit(*this);
}

ObjectType::ObjectType(std::string name)
    : Type(std::move(name))
{
}
