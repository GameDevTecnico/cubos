#include "tools.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

/// The input options of the program.
struct Options
{
    fs::path input = "";  ///< The input directory.
    fs::path output = ""; ///< The output directory.
    bool help = false;    ///< Prints the help message.
};

/// Parsed component type.
struct Component
{
    std::string namespaceStr;        ///< The namespace of the component (e.g.: "cubos::engine").
    std::string typeStr;             ///< The type of the component (e.g.: "Position").
    std::string storage;             ///< The storage type of the component (e.g.: "VecStorage").
    std::string name;                ///< The registered name of the component (e.g.: "cubos/position")
    std::vector<std::string> fields; ///< The fields of the component.
    fs::path header;                 ///< The file where the component is defined.
};

/// Prints the help message of the program.
static void printHelp()
{
    std::cerr << "Usage: cubinhos generate <INPUT> <OUTPUT>" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -h Prints this help message." << std::endl;
}

/// Parses the command line arguments.
/// @param argc The number of arguments.
/// @param argv The arguments.
/// @param options The options to fill.
/// @return True if the arguments were parsed successfully, false otherwise.
static bool parseArguments(int argc, char** argv, Options& options)
{
    int foundInput = 0;

    // Iterate over the arguments.
    for (int i = 0; i < argc; ++i)
    {
        if (std::string(argv[i]) == "-h")
        {
            options.help = true;
            return true;
        }
        else
        {
            if (foundInput == 0)
            {
                options.input = argv[i];
            }
            else if (foundInput == 1)
            {
                options.output = argv[i];
            }
            else
            {
                std::cerr << "Too many arguments." << std::endl;
                return false;
            }

            foundInput++;
        }
    }

    if (options.input.empty())
    {
        std::cerr << "Missing input directory." << std::endl;
        return false;
    }
    else if (options.output.empty())
    {
        std::cerr << "Missing output directory." << std::endl;
        return false;
    }
    else
        return true;
}

/// Trims the string.
/// @param str The string to trim.
/// @return The trimmed string.
static std::string trim(std::string str)
{
    // Trim left.
    size_t i;
    for (i = 0; i < str.size(); ++i)
        if (!isspace(str[i]))
            break;
    str = str.substr(i);

    // Trim right.
    for (i = str.size() - 1; i >= 0; --i)
        if (!isspace(str[i]))
            break;
    return str.substr(0, i + 1);
}

/// Parses a namespace.
/// @param line The line to parse.
/// @param namespaceStr The namespace to fill.
/// @return True if the namespace was parsed successfully, false otherwise.
static bool parseNamespace(std::string line, std::string& namespaceStr)
{
    // Check if the line is a namespace.
    line = trim(line);
    if (line.find("namespace") == 0)
    {
        // Get the namespace.
        std::string name = line.substr(10);
        name = trim(name);
        if (!name.empty())
        {
            if (namespaceStr.empty())
                namespaceStr = name;
            else
                namespaceStr += "::" + name;
        }
    }

    return true;
}

/// Parses a component attribute.
/// @param line The line to parse.
/// @param component The component to fill.
/// @param found Set to true if the attribute was found.
/// @return True if the attribute was parsed successfully, false otherwise.
bool parseAttribute(std::string line, Component& component, bool& found)
{
    // Trim the line.
    line = trim(line);
    std::string_view start = "[[cubos::component(";
    if (!line.starts_with(start))
        return true;
    found = true;

    // Extract the arguments.
    std::string_view args = line.substr(start.size());
    size_t end = args.find(")]");
    if (end == std::string::npos)
    {
        std::cerr << "Couldn't parse component attribute '" << line << "' at " << component.header << std::endl;
        return false;
    }
    args = line.substr(0, end);

    // Parse the name and storage.
    size_t comma = args.find(",");
    if (comma == std::string::npos)
    {
        std::cerr << "Couldn't parse component attribute arguments '" << args << "'" << std::endl;
        return false;
    }
    component.name = trim(std::string(args.substr(0, comma)));
    if (component.name[0] != '"' || component.name[component.name.size() - 1] != '"')
    {
        std::cerr << "Component name '" << component.name << "' must be a string literal" << std::endl;
        return false;
    }
    component.name = component.name.substr(1, component.name.size() - 2);
    component.storage = trim(std::string(args.substr(comma + 1)));

    return true;
}

/// Removes comments in the file and unnecessary whitespaces.
/// @param contents
bool preprocessFile(std::string& contents)
{
    // Remove inline comments.
    size_t pos = 0;
    while (pos < contents.size())
    {
        // Find the next comment.
        size_t commentStart = contents.find("//", pos);
        if (commentStart == std::string::npos)
            break;

        // Find the end of the comment.
        size_t commentEnd = contents.find("\n", commentStart);
        if (commentEnd == std::string::npos)
            commentEnd = contents.size();

        // Remove the comment.
        contents.erase(commentStart, commentEnd - commentStart);
    }

    // Remove multiline comments.
    pos = 0;
    while (pos < contents.size())
    {
        // Find the next comment.
        size_t commentStart = contents.find("/*", pos);
        if (commentStart == std::string::npos)
            break;

        // Find the end of the comment.
        size_t commentEnd = contents.find("*/", commentStart);
        if (commentEnd == std::string::npos)
        {
            std::cerr << "Couldn't find the end of the comment" << std::endl;
            return false;
        }

        // Remove the comment.
        contents.erase(commentStart, commentEnd - commentStart + 2);
    }

    // Remove unnecessary whitespaces.
    for (pos = 0; pos < contents.size(); pos++)
    {
        if (isspace(contents[pos]))
        {
            contents[pos] = ' ';

            size_t whitespaceStart = pos;
            size_t whitespaceEnd = pos;
            while (whitespaceEnd < contents.size() && isspace(contents[whitespaceEnd]))
                whitespaceEnd++;
            if (pos != 0 && pos != contents.size() - 1)
                whitespaceStart += 1;
            if (whitespaceEnd - whitespaceStart >= 1)
                contents.erase(whitespaceStart + 1, whitespaceEnd - whitespaceStart);
        }
        else if (contents[pos] == '{' || contents[pos] == '}')
        {
            if (pos > 0 && !isspace(contents[pos - 1]))
                contents.insert(pos, " ");
            if (pos < contents.size() - 1 && !isspace(contents[pos + 1]))
                contents.insert(pos + 1, " ");
        }
    }

    return true;
}

/// Removes commented sections of the current line.
/// @param line The line to process.
void removeComments(std::string& line, bool& isComment)
{
    // If we're in a comment, check if it ends.
    if (isComment)
    {
        size_t pos = line.find("*/");
        if (pos != std::string::npos)
        {
            isComment = false;
            line = line.substr(pos + 2);
        }
        else
        {
            line.clear();
            return;
        }
    }

    // Remove single line comments.
    size_t pos = line.find("//");
    if (pos != std::string::npos)
        line = line.substr(0, pos);

    // Remove multi-line comments starting in this line.
    do
    {
        size_t pos = line.find("/*");
        if (pos != std::string::npos)
        {
            size_t end = line.find("*/");
            if (end != std::string::npos)
            {
                line = line.substr(0, pos) + line.substr(end + 2);
                continue;
            }
            else
            {
                line = line.substr(0, pos);
                isComment = true;
            }
        }
    } while (false);
}

/// Utility used to parse the source C++ code.
/// Automatically skips comments and whitespace.
class Parser
{
public:
    Parser(fs::path path, std::string_view input) : path(path), input(input)
    {
        this->line = 1;
        this->column = 1;
    }

    /// If the next token matches the given punctuation string, it is consumed and true is returned.
    bool acceptPunctuation(std::string_view token)
    {
        this->skipWS();
        if (this->input.starts_with(token))
        {
            this->input.remove_prefix(token.size());
            this->column += token.size();
            return true;
        }
        return false;
    }

    /// If the next token matches the given keyword string, it is consumed and true is returned.
    bool acceptKeyword(std::string_view keyword)
    {
        this->skipWS();
        if (this->input.starts_with(keyword))
            if (this->input.size() <= keyword.size() || !isalnum(this->input[keyword.size()]))
            {
                this->input.remove_prefix(keyword.size());
                this->column += keyword.size();
                return true;
            }
        return false;
    }

    /// Accepts any of the given keywords.
    bool acceptKeywords(std::initializer_list<std::string_view> keywords)
    {
        for (auto keyword : keywords)
            if (this->acceptKeyword(keyword))
                return true;
        return false;
    }

    /// Accepts the next token if it is a string literal.
    bool acceptString(std::string& value)
    {
        this->skipWS();
        if (!this->input.starts_with("\""))
            return false;
        for (size_t i = 1; i < this->input.size(); ++i)
        {
            if (this->input[i] == '\n')
                break;
            else if (this->input[i] == '\\')
                value += this->input[i++];
            else if (this->input[i] == '"')
            {
                this->input.remove_prefix(i + 1);
                this->column += i + 1;
                return true;
            }
            else
                value += this->input[i];
        }

        this->error("Unterminated string literal");
        return false;
    }

    /// Assumes the current token is an identifier and returns it.
    std::string_view extractIdentifier()
    {
        this->skipWS();
        size_t start = 0;
        while (start < this->input.size() && (isalnum(this->input[start]) || this->input[start] == ':'))
            start++;
        std::string_view identifier = this->input.substr(0, start);
        this->input.remove_prefix(start);
        this->column += start;
        return identifier;
    }

    /// Skips the next token.
    void skipToken()
    {
        this->skipWS();

        if (this->input.empty())
            return;

        if (isalnum(this->input[0]))
        {
            size_t start = 1;
            while (start < this->input.size() && (isalnum(this->input[start]) || this->input[start] == ':'))
                start++;
            this->input.remove_prefix(start);
            this->column += start;
        }
        else
        {
            this->input.remove_prefix(1);
            this->column += 1;
        }
    }

    /// Checks if end of file has been reached.
    bool eof() const
    {
        return this->input.empty();
    }

    /// Prints an error message.
    void error(std::string_view msg)
    {
        std::cerr << msg << " at " << this->path << ":" << this->line << ":" << this->column << std::endl;
    }

private:
    void skipWS()
    {
        while (true)
        {
            bool skip = false;

            while (!this->input.empty() && isspace(this->input[0]))
            {
                this->column += 1;
                if (this->input[0] == '\n')
                {
                    this->line += 1;
                    this->column = 1;
                }
                this->input.remove_prefix(1);
                skip = true;
            }

            // Skip comments
            if (this->input.starts_with("/*"))
            {
                this->input = this->input.substr(this->input.find("*/") + 2);
                skip = true;
            }
            else if (this->input.starts_with("//"))
            {
                while (!this->input.empty() && this->input[0] != '\n')
                {
                    this->input.remove_prefix(1);
                    this->column += 1;
                }
                skip = true;
            }

            if (!skip)
                break;
        }
    }

    fs::path path;
    int line;
    int column;
    std::string_view input;
};

/// Parses a component.
/// @param parser The parser to use.
/// @param component The component to output to.
/// @return True if the component was parsed successfully.
bool parseComponent(Parser& parser, Component& component)
{
    component.typeStr = parser.extractIdentifier();

    if (!parser.acceptPunctuation("{"))
    {
        parser.error("Expected '{'");
        return false;
    }

    int scope = 1;
    bool ignoreUntilOpenBrace = false;
    while (scope > 0)
    {
        bool consumed = true;

        // Handle scope changes.
        if (parser.acceptPunctuation("{"))
        {
            scope += 1;
            ignoreUntilOpenBrace = false;
        }
        else if (parser.acceptPunctuation("}"))
        {
            parser.acceptPunctuation(";");
            scope -= 1;
        }
        else
            consumed = false;

        // Parse statements within the componnet's scope.
        if (scope == 1 && !ignoreUntilOpenBrace)
        {
            if (!consumed && parser.acceptKeywords({"struct", "class"}))
            {
                consumed = true;
                ignoreUntilOpenBrace = true;
            }

            // Skip some non-field statements.
            if (!consumed && parser.acceptKeywords({"static", "const", "constexpr", "using"}))
            {
                while (!parser.acceptPunctuation(";"))
                    parser.skipToken();
                consumed = true;
            }

            // Try to parse a field.
            if (!consumed)
            {
                bool isField = true;
                parser.skipToken();
                auto name = std::string(parser.extractIdentifier());
                if (parser.acceptPunctuation("="))
                    while (!parser.acceptPunctuation(";"))
                        parser.skipToken();
                else if (!parser.acceptPunctuation(";"))
                    isField = false;

                if (isField)
                    component.fields.push_back(name);
                consumed = true;
            }
        }

        if (!consumed)
            parser.skipToken();
    }

    std::cerr << "Found component '" << component.name << "' ";
    std::cerr << "(" << component.namespaceStr << "::" << component.typeStr << ") ";
    std::cerr << "with " << component.fields.size() << " fields in ";
    std::cerr << component.header << std::endl;
    return true;
}

/// Parses a header file or recurses if it's a directory.
/// Extracts the component data from the file.
/// @param path The path to the file or directory.
/// @param components The components to fill.
/// @return True if the file was parsed successfully, false otherwise.
static bool parse(fs::path path, std::vector<Component>& components)
{
    if (fs::is_directory(path))
    {
        // Recurse into the directory.
        for (auto& entry : fs::directory_iterator(path))
        {
            if (!parse(entry.path(), components))
            {
                return false;
            }
        }
    }
    else if (path.extension() == ".hpp")
    {
        // Read the file contents.
        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " << path << std::endl;
            return false;
        }
        std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        // State necessary for parsing.
        std::string namespaceStr;
        std::vector<int> namespaceScope;
        int scope = 0;
        bool anonymous = false;

        // Parse the file.
        auto parser = Parser(path, contents);
        while (!parser.eof())
        {
            // Check if it's a namespace.
            if (parser.acceptKeyword("namespace") && !anonymous)
            {
                namespaceScope.push_back(scope);

                if (parser.acceptPunctuation("{"))
                {
                    anonymous = true; // Its an anonymous namespace.
                    scope += 1;
                }
                else if (namespaceStr.empty())
                    namespaceStr = parser.extractIdentifier();
                else
                    namespaceStr += "::" + std::string(parser.extractIdentifier());
            }

            // Update scope level if braces are found.
            else if (parser.acceptPunctuation("{"))
                scope += 1;
            else if (parser.acceptPunctuation("}"))
            {
                scope -= 1;

                if (!namespaceScope.empty() && namespaceScope.back() == scope)
                {
                    namespaceScope.pop_back();
                    if (!anonymous)
                    {
                        size_t pos = namespaceStr.find_last_of("::");
                        if (pos != std::string::npos)
                            namespaceStr = namespaceStr.substr(0, pos);
                        else
                            namespaceStr.clear();
                    }
                    else
                        anonymous = false;
                }
            }

            // If we find a struct, parse it.
            else if (parser.acceptKeyword("struct"))
            {
                if (!parser.acceptPunctuation("[["))
                    continue;
                if (!parser.acceptKeyword("cubos"))
                    continue;
                if (!parser.acceptPunctuation("::"))
                    continue;
                if (!parser.acceptKeyword("component"))
                    continue;
                if (!parser.acceptPunctuation("("))
                {
                    parser.error("Expected '(' after 'cubos::component'");
                    return false;
                }

                if (anonymous)
                {
                    parser.error("Expected component to be in a named namespace");
                    return false;
                }

                Component component;
                component.namespaceStr = namespaceStr;
                component.header = path;

                // Parse the component name.
                if (!parser.acceptString(component.name))
                {
                    parser.error("Expected string literal for component name");
                    return false;
                }

                if (!parser.acceptPunctuation(","))
                {
                    parser.error("Expected ',' after component name");
                    return false;
                }

                // Parse the component storage type.
                component.storage = parser.extractIdentifier();

                if (!parser.acceptPunctuation(")"))
                {
                    parser.error("Expected ')' after component storage type");
                    return false;
                }

                if (!parser.acceptPunctuation("]]"))
                {
                    parser.error("Expected ']]' after component storage type");
                    return false;
                }

                if (!parseComponent(parser, component))
                    return false;

                components.push_back(component);
            }

            // Skip unknown token.
            else
                parser.skipToken();
        }
    }

    return true;
}

/// Runs the generator from the command line options.
/// @param options The command line options.
/// @return True if the generation was successful, false otherwise.
static bool generate(const Options& options)
{
    std::vector<Component> components;
    if (!parse(options.input, components))
        return false;

    for (auto& component : components)
    {
        auto path = options.output;
        path.append(component.name + ".hpp");
        fs::create_directories(path.parent_path());

        if (std::filesystem::exists(path) &&
            std::filesystem::last_write_time(path) > std::filesystem::last_write_time(component.header))
        {
            std::cerr << "Skipping '" << component.name << "' because it's up to date" << std::endl;
            continue;
        }

        std::ofstream file(path);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " << path << std::endl;
            return false;
        }

        std::string macro_name = component.name;
        for (auto& c : macro_name)
        {
            c = toupper(c);
            if (c == '/')
                c = '_';
        }

        std::string id = component.namespaceStr + "::" + component.typeStr;

        file << "#ifndef GEN_COMPONENT_" << macro_name << "_HPP" << std::endl;
        file << "#define GEN_COMPONENT_" << macro_name << "_HPP" << std::endl;
        file << std::endl;
        file << "/// This file was generated by cubinhos generate." << std::endl;
        file << "/// Do not edit this file." << std::endl;
        file << std::endl;
        file << "#include " << fs::absolute(component.header) << std::endl;
        file << std::endl;
        file << "#include <cubos/core/ecs/registry.hpp>" << std::endl;
        file << "#include <cubos/core/ecs/vec_storage.hpp>" << std::endl;
        file << "#include <cubos/core/ecs/map_storage.hpp>" << std::endl;
        file << "#include <cubos/core/ecs/null_storage.hpp>" << std::endl;
        file << std::endl;
        file << "namespace cubos::core::data" << std::endl;
        file << "{" << std::endl;
        file << "    inline static void serialize(Serializer& ser, const " << id << "& c," << std::endl;
        file << "                                 std::tuple<const SerializationMap<ecs::Entity, std::string>&, "
                "Handle::SerContext> ctx,"
             << std::endl
             << "                                 const char* name)" << std::endl;
        file << "    {" << std::endl;
        if (component.fields.size() == 1)
            file << "        ser.write(c." << component.fields[0] << ", ctx, name);" << std::endl;
        else
        {
            file << "        ser.beginObject(name);" << std::endl;
            for (auto& field : component.fields)
                file << "        ser.write(c." << field << ", ctx, \"" << field << "\");" << std::endl;
            file << "        ser.endObject();" << std::endl;
        }
        file << "    }" << std::endl;
        file << std::endl;
        file << "    inline static void deserialize(Deserializer& des, " << id << "& c," << std::endl;
        file << "                                   std::tuple<const SerializationMap<ecs::Entity, std::string>&, "
                "Handle::DesContext> ctx)"
             << std::endl;
        file << "    {" << std::endl;
        if (component.fields.size() == 1)
            file << "        des.read(c." << component.fields[0] << ", ctx);" << std::endl;
        else
        {
            file << "        des.beginObject();" << std::endl;
            for (auto& field : component.fields)
                file << "        des.read(c." << field << ", ctx);" << std::endl;
            file << "        des.endObject();" << std::endl;
        }
        file << "    }" << std::endl;
        file << "} // namespace cubos::core::data" << std::endl;
        file << std::endl;
        file << "namespace cubos::core::ecs" << std::endl;
        file << "{" << std::endl;
        file << "    template <>" << std::endl;
        file << "    struct ComponentStorage<" << id << ">" << std::endl;
        file << "    {" << std::endl;
        file << "        using Type = " << component.storage << "<" << id << ">;" << std::endl;
        file << "    };" << std::endl;
        file << "} // namespace cubos::core::ecs" << std::endl;
        file << std::endl;
        file << "namespace " << component.namespaceStr << std::endl;
        file << "{" << std::endl;
        file << "    CUBOS_REGISTER_COMPONENT(" << component.typeStr << ", \"" << component.name << "\");" << std::endl;
        file << "} // namespace " << component.namespaceStr << std::endl;
        file << std::endl;
        file << "#endif // GEN_COMPONENT_" << macro_name << "_HPP" << std::endl;
    }

    return true;
}

int runGenerate(int argc, char** argv)
{
    // Parse command line arguments.
    Options options;
    if (!parseArguments(argc, argv, options))
    {
        printHelp();
        return 1;
    }
    else if (options.help)
    {
        printHelp();
        return 0;
    }

    // Convert the input file.
    if (!generate(options))
    {
        std::cerr << "Failed to generate component include files." << std::endl;
        return 1;
    }

    return 0;
}
