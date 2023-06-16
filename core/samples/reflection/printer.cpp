#include "printer.hpp"

#include <cubos/core/reflection/array.hpp>
#include <cubos/core/reflection/dictionary.hpp>
#include <cubos/core/reflection/object.hpp>
#include <cubos/core/reflection/primitive.hpp>
#include <cubos/core/reflection/variant.hpp>

using cubos::core::memory::Stream;

static void indent(Stream& stream, int level)
{
    for (int i = 0; i < level; ++i)
    {
        stream.print("  ");
    }
}

Printer::Printer(Stream& stream)
    : mStream(stream)
    , mLevel(0)
{
}

void Printer::visit(const cubos::core::reflection::PrimitiveType& type)
{
    mStream.printf("primitive {}\n", type.name());
}

void Printer::visit(const cubos::core::reflection::ObjectType& type)
{
    mStream.printf("object {}\n", type.name());

    // Iterate over all fields of the object.
    ++mLevel;
    for (const auto& field : type.fields())
    {
        // Print the field name, its hints and its type.
        indent(mStream, mLevel);
        mStream.print(field->name());
        for (auto [hintType, hint] : field->hints())
        {
            mStream.printf(" [hint {}]", hintType->name());
        }
        mStream.print(": ");
        field->type().accept(*this);
    }
    --mLevel;
}

void Printer::visit(const cubos::core::reflection::VariantType& type)
{
    mStream.printf("variant {}\n", type.name());

    ++mLevel;
    for (const auto& variant : type.variants())
    {
        indent(mStream, mLevel);
        variant.type().accept(*this);
    }
    --mLevel;
}

void Printer::visit(const cubos::core::reflection::ArrayType& type)
{
    mStream.printf("array {}\n", type.name());
    ++mLevel;
    indent(mStream, mLevel);
    type.elementType().accept(*this);
    --mLevel;
}

void Printer::visit(const cubos::core::reflection::DictionaryType& type)
{
    mStream.printf("dictionary {}\n", type.name());
    ++mLevel;
    indent(mStream, mLevel);
    type.keyType().accept(*this);
    indent(mStream, mLevel);
    type.valueType().accept(*this);
    --mLevel;
}
