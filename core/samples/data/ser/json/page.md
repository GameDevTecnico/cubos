# JSON Serializer {#examples-core-data-ser-json}

@brief Implementing @ref cubos::core::data::JSONSerializer "JSONSerializer".

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/data/ser/json).

You'll need to include 
@ref core/data/ser/json.hpp. For simplicity, in this sample we'll use
the following aliases:

@snippet data/ser/custom/main.cpp Include

Using the JSON serializer is as simple as calling @ref
cubos::core::data::JSONSerializer::write "JSONSerializer::write" on the data we want to
serialize.

In this case, we'll be serializing a `std::vector<std::vector<std::vector<glm::ivec3>>>`, which is
a nested array of objects with three `int32_t` fields.

@snippet data/ser/custom/main.cpp Usage

This should output:

@snippet data/ser/custom/main.cpp Output
