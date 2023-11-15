# JSON Serializer {#examples-core-data-ser-json}

@brief Implementing @ref cubos::core::data::JSONSerializer "JSONSerializer".

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/data/ser/json).

You'll need to include 
@ref core/data/ser/json.hpp. For simplicity, in this sample we'll use
the following aliases:

@snippet data/ser/custom/main.cpp Include

Using the json serializer is as simple as calling @ref
cubos::core::data::JSONSerializer::write "JSONSerializer::write" on the data we want to
serialize.

# Update this section with final sample version

In this case, we'll be serializing a `std::vector<glm::ivec3>`, which is
an array of objects with three `int32_t` fields.

# Update this section with final sample version

@snippet data/ser/custom/main.cpp Usage

This should output:

@snippet data/ser/custom/main.cpp Output
