#include <cubos/core/reflection/object.hpp>
#include <cubos/core/reflection/visitor.hpp>

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

ObjectType::Builder::Builder(ObjectType& type)
    : Type::Builder(type)
    , mType(type)
{
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
