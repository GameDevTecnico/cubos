#include <cstdint>

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

auto cubos::core::reflection::makeAnonymousType(const char* name, const char* file, std::size_t size,
                                                std::size_t alignment, void (*destructor)(void*)) -> const Type&
{
    // Replace non-ASCII-alphanumeric characters in the file path with underscores.
    std::string fileName;
    for (const char* c = file; *c != '\0'; ++c)
    {
        if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') || (*c >= '0' && *c <= '9'))
        {
            fileName.push_back(*c);
        }
        else
        {
            fileName.push_back('_');
        }
    }

    // Remove the "_src_" prefix from the file name.
    if (auto cursor = fileName.find("_src_"); cursor != std::string::npos)
    {
        fileName.erase(0, cursor + 5);
    }

    // The "_include_" too.
    if (auto cursor = fileName.find("_include_"); cursor != std::string::npos)
    {
        fileName.erase(0, cursor + 9);
    }

    // Remove the "_cpp" suffix from the file name.
    if (fileName.ends_with("_cpp"))
    {
        fileName.erase(fileName.size() - 4, 4);
    }

    // And the "_hpp" suffix too.
    if (fileName.ends_with("_hpp"))
    {
        fileName.erase(fileName.size() - 4, 4);
    }

    std::string fullName = name;
    fullName.append("#");
    fullName.append(fileName);
    return Type::create(fullName).with(ConstructibleTrait{size, alignment, destructor});
}
