#pragma once

#include <cubos/core/memory/stream.hpp>
#include <cubos/core/reflection/visitor.hpp>

/// @brief An example visitor which simply prints the information of the types it visits.
class Printer : public cubos::core::reflection::TypeVisitor
{
public:
    /// @brief Construct a new Printer object.
    /// @param stream Stream to print to.
    Printer(cubos::core::memory::Stream& stream);

    virtual void visit(const cubos::core::reflection::PrimitiveType& type) override;
    virtual void visit(const cubos::core::reflection::ObjectType& type) override;
    virtual void visit(const cubos::core::reflection::VariantType& type) override;
    virtual void visit(const cubos::core::reflection::ArrayType& type) override;
    virtual void visit(const cubos::core::reflection::DictionaryType& type) override;

private:
    cubos::core::memory::Stream& mStream;
    int mLevel;
};
