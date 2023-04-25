#include <filesystem>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

#include "tools.hpp"

namespace fs = std::filesystem;

/// The input options of the program.
struct GenerateOptions
{
    fs::path input = "";  ///< The input directory.
    fs::path output = ""; ///< The output source file.
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
static bool parseArguments(int argc, char** argv, GenerateOptions& options)
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

    if (options.input.empty())
    {
        std::cerr << "Missing input directory." << std::endl;
        return false;
    }
    if (options.output.empty())
    {
        std::cerr << "Missing output directory." << std::endl;
        return false;
    }
    return true;
}

/// Trims the string.
/// @param str The string to trim.
/// @return The trimmed string.
static std::string_view trim(std::string_view str)
{
    // Trim left.
    int64_t i;
    for (i = 0; i < static_cast<int64_t>(str.size()); ++i)
    {
        if (isspace(str[static_cast<std::size_t>(i)]) == 0)
        {
            break;
        }
    }
    str = str.substr(static_cast<std::size_t>(i));

    // Trim right.
    for (i = static_cast<int64_t>(str.size()) - 1; i >= 0; --i)
    {
        if (isspace(str[static_cast<std::size_t>(i)]) == 0)
        {
            break;
        }
    }
    return str.substr(0, static_cast<std::size_t>(i) + 1);
}

/// Parses a component attribute.
/// @param line The line to parse.
/// @param component The component to fill.
/// @param found Set to true if the attribute was found.
/// @return True if the attribute was parsed successfully, false otherwise.
bool parseAttribute(std::string_view line, Component& component, bool& found)
{
    // Trim the line.
    line = trim(line);
    std::string_view start = "[[cubos::component(";
    if (!line.starts_with(start))
    {
        return true;
    }
    found = true;

    // Extract the arguments.
    std::string_view args = line.substr(start.size());
    std::size_t end = args.find(")]");
    if (end == std::string::npos)
    {
        std::cerr << "Couldn't parse component attribute '" << line << "' at " << component.header << std::endl;
        return false;
    }
    args = line.substr(0, end);

    // Parse the name and storage.
    std::size_t comma = args.find(',');
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
    std::size_t pos = 0;
    while (pos < contents.size())
    {
        // Find the next comment.
        std::size_t commentStart = contents.find("//", pos);
        if (commentStart == std::string::npos)
        {
            break;
        }

        // Find the end of the comment.
        std::size_t commentEnd = contents.find('\n', commentStart);
        if (commentEnd == std::string::npos)
        {
            commentEnd = contents.size();
        }

        // Remove the comment.
        contents.erase(commentStart, commentEnd - commentStart);
    }

    // Remove multiline comments.
    pos = 0;
    while (pos < contents.size())
    {
        // Find the next comment.
        std::size_t commentStart = contents.find("/*", pos);
        if (commentStart == std::string::npos)
        {
            break;
        }

        // Find the end of the comment.
        std::size_t commentEnd = contents.find("*/", commentStart);
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
        if (isspace(contents[pos]) != 0)
        {
            contents[pos] = ' ';

            std::size_t whitespaceStart = pos;
            std::size_t whitespaceEnd = pos;
            while (whitespaceEnd < contents.size() && (isspace(contents[whitespaceEnd]) != 0))
            {
                whitespaceEnd++;
            }
            if (pos != 0 && pos != contents.size() - 1)
            {
                whitespaceStart += 1;
            }
            if (whitespaceEnd - whitespaceStart >= 1)
            {
                contents.erase(whitespaceStart + 1, whitespaceEnd - whitespaceStart);
            }
        }
        else if (contents[pos] == '{' || contents[pos] == '}')
        {
            if (pos > 0 && (isspace(contents[pos - 1]) == 0))
            {
                contents.insert(pos, " ");
            }
            if (pos < contents.size() - 1 && (isspace(contents[pos + 1]) == 0))
            {
                contents.insert(pos + 1, " ");
            }
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
        std::size_t pos = line.find("*/");
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
    std::size_t pos = line.find("//");
    if (pos != std::string::npos)
    {
        line = line.substr(0, pos);
    }

    // Remove multi-line comments starting in this line.
    do
    {
        std::size_t pos = line.find("/*");
        if (pos != std::string::npos)
        {
            std::size_t end = line.find("*/");
            if (end != std::string::npos)
            {
                line = line.substr(0, pos) + line.substr(end + 2);
                break;
            }

            line = line.substr(0, pos);
            isComment = true;
        }
    } while (false);
}

/// Utility used to parse the source C++ code.
/// Automatically skips comments and whitespace.
class Parser
{
public:
    Parser(fs::path path, std::string_view input)
        : m_path(std::move(path))
        , m_input(input)
    {
        this->m_line = 1;
        this->m_column = 1;
    }

    /// If the next token matches the given punctuation string, it is consumed and true is returned.
    bool acceptPunctuation(std::string_view token)
    {
        this->skipWS();
        if (this->m_input.starts_with(token))
        {
            this->m_input.remove_prefix(token.size());
            this->m_column += static_cast<int>(token.size());
            return true;
        }
        return false;
    }

    /// If the next token matches the given keyword string, it is consumed and true is returned.
    bool acceptKeyword(std::string_view keyword)
    {
        this->skipWS();
        if (this->m_input.starts_with(keyword))
        {
            if (this->m_input.size() <= keyword.size() || (isalnum(this->m_input[keyword.size()]) == 0))
            {
                this->m_input.remove_prefix(keyword.size());
                this->m_column += static_cast<int>(keyword.size());
                return true;
            }
        }
        return false;
    }

    /// Accepts any of the given keywords.
    bool acceptKeywords(std::initializer_list<std::string_view> keywords)
    {
        for (auto keyword : keywords)
        {
            if (this->acceptKeyword(keyword))
            {
                return true;
            }
        }

        return false;
    }

    /// Accepts the next token if it is a string literal.
    bool acceptString(std::string& value)
    {
        this->skipWS();
        if (!this->m_input.starts_with("\""))
        {
            return false;
        }
        for (std::size_t i = 1; i < this->m_input.size(); ++i)
        {
            if (this->m_input[i] == '\n')
            {
                break;
            }
            if (this->m_input[i] == '\\')
            {
                value += this->m_input[i++];
            }
            else if (this->m_input[i] == '"')
            {
                this->m_input.remove_prefix(i + 1);
                this->m_column += static_cast<int>(i + 1);
                return true;
            }
            else
            {
                value += this->m_input[i];
            }
        }

        this->error("Unterminated string literal");
        return false;
    }

    /// Assumes the current token is an identifier and returns it.
    std::string_view extractIdentifier()
    {
        this->skipWS();
        std::size_t start = 0;
        while (start < this->m_input.size() && ((isalnum(this->m_input[start]) != 0) || this->m_input[start] == ':'))
        {
            start++;
        }
        std::string_view identifier = this->m_input.substr(0, start);
        this->m_input.remove_prefix(start);
        this->m_column += static_cast<int>(start);
        return identifier;
    }

    /// Skips the next token.
    void skipToken()
    {
        this->skipWS();

        if (this->m_input.empty())
        {
            return;
        }

        if (isalnum(this->m_input[0]) != 0)
        {
            std::size_t start = 1;
            while (start < this->m_input.size() &&
                   ((isalnum(this->m_input[start]) != 0) || this->m_input[start] == ':'))
            {
                start++;
            }
            this->m_input.remove_prefix(start);
            this->m_column += static_cast<int>(start);
        }
        else
        {
            this->m_input.remove_prefix(1);
            this->m_column += 1;
        }
    }

    /// Checks if end of file has been reached.
    bool eof() const
    {
        return this->m_input.empty();
    }

    /// Prints an error message.
    void error(std::string_view msg)
    {
        std::cerr << msg << " at " << this->m_path << ":" << this->m_line << ":" << this->m_column << std::endl;
    }

private:
    void skipWS()
    {
        while (true)
        {
            bool skip = false;

            while (!this->m_input.empty() && (isspace(this->m_input[0]) != 0))
            {
                this->m_column += 1;
                if (this->m_input[0] == '\n')
                {
                    this->m_line += 1;
                    this->m_column = 1;
                }
                this->m_input.remove_prefix(1);
                skip = true;
            }

            // Skip comments
            if (this->m_input.starts_with("/*"))
            {
                this->m_input = this->m_input.substr(this->m_input.find("*/") + 2);
                skip = true;
            }
            else if (this->m_input.starts_with("//"))
            {
                while (!this->m_input.empty() && this->m_input[0] != '\n')
                {
                    this->m_input.remove_prefix(1);
                    this->m_column += 1;
                }
                skip = true;
            }

            if (!skip)
            {
                break;
            }
        }
    }

    fs::path m_path;
    int m_line;
    int m_column;
    std::string_view m_input;
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
        {
            consumed = false;
        }

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
                {
                    parser.skipToken();
                }
                consumed = true;
            }

            // Try to parse a field.
            if (!consumed)
            {
                bool isField = true;
                parser.skipToken();
                auto name = std::string(parser.extractIdentifier());
                if (parser.acceptPunctuation("="))
                {
                    while (!parser.acceptPunctuation(";"))
                    {
                        parser.skipToken();
                    }
                }
                else if (!parser.acceptPunctuation(";"))
                {
                    isField = false;
                }

                if (isField)
                {
                    component.fields.push_back(name);
                }
                consumed = true;
            }
        }

        if (!consumed)
        {
            parser.skipToken();
        }
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
static bool parse(const fs::path& path, std::vector<Component>& components)
{
    if (fs::is_directory(path))
    {
        // Recurse into the directory.
        for (const auto& entry : fs::directory_iterator(path))
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
                {
                    namespaceStr = parser.extractIdentifier();
                }
                else
                {
                    namespaceStr += "::" + std::string(parser.extractIdentifier());
                }
            }

            // Update scope level if braces are found.
            else if (parser.acceptPunctuation("{"))
            {
                scope += 1;
            }
            else if (parser.acceptPunctuation("}"))
            {
                scope -= 1;

                if (!namespaceScope.empty() && namespaceScope.back() == scope)
                {
                    namespaceScope.pop_back();
                    if (!anonymous)
                    {
                        std::size_t pos = namespaceStr.find_last_of("::");
                        if (pos != std::string::npos)
                        {
                            namespaceStr = namespaceStr.substr(0, pos);
                        }
                        else
                        {
                            namespaceStr.clear();
                        }
                    }
                    else
                    {
                        anonymous = false;
                    }
                }
            }

            // If we find a struct, parse it.
            else if (parser.acceptKeyword("struct"))
            {
                if (!parser.acceptPunctuation("[["))
                {
                    continue;
                }
                if (!parser.acceptKeyword("cubos"))
                {
                    continue;
                }
                if (!parser.acceptPunctuation("::"))
                {
                    continue;
                }
                if (!parser.acceptKeyword("component"))
                {
                    continue;
                }
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
                {
                    return false;
                }

                components.push_back(component);
            }

            // Skip unknown token.
            else
            {
                parser.skipToken();
            }
        }
    }

    return true;
}

/// Runs the generator from the command line options.
/// @param options The command line options.
/// @return True if the generation was successful, false otherwise.
static bool generate(const GenerateOptions& options)
{
    std::vector<Component> components;
    if (!parse(options.input, components))
    {
        return false;
    }

    fs::create_directories(options.output.parent_path());
    if (std::filesystem::exists(options.output))
    {
        // Check if any of the components are out of date.
        auto writeTime = std::filesystem::last_write_time(options.output);
        for (auto& component : components)
        {
            if (std::filesystem::last_write_time(component.header) > writeTime)
            {
                break;
            }
        }

        // If none of the components are out of date, skip the generation.
        std::cerr << "Skipping generation because it's up to date" << std::endl;
        return true;
    }

    std::ofstream file(options.output);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << options.output << std::endl;
        return false;
    }

    file << "/// This file was generated by cubinhos generate." << std::endl;
    file << "/// Do not edit this file." << std::endl;
    file << std::endl;
    file << "#include <cubos/core/ecs/registry.hpp>" << std::endl;
    file << "#include <cubos/core/ecs/vec_storage.hpp>" << std::endl;
    file << "#include <cubos/core/ecs/map_storage.hpp>" << std::endl;
    file << "#include <cubos/core/ecs/null_storage.hpp>" << std::endl;
    file << std::endl;

    // Include all the component headers.
    for (auto& component : components)
    {
        file << "#include " << fs::absolute(component.header) << std::endl;
    }

    for (auto& component : components)
    {
        std::string id = "::" + component.namespaceStr + "::" + component.typeStr;
        std::string storageId = "::cubos::core::ecs::" + component.storage + "<" + id + ">";

        file << std::endl;
        file << "template <>" << std::endl;
        file << "void cubos::core::data::serialize<" << id << ">(Serializer& ser, const " << id << "& obj,"
             << std::endl;
        file << "                                         const char* name)" << std::endl;
        file << "{" << std::endl;
        if (component.fields.size() == 1)
        {
            file << "    ser.write(obj." << component.fields[0] << ", name);" << std::endl;
        }
        else
        {
            file << "    ser.beginObject(name);" << std::endl;
            for (auto& field : component.fields)
            {
                file << "    ser.write(obj." << field << ", \"" << field << "\");" << std::endl;
            }
            file << "    ser.endObject();" << std::endl;
        }
        file << "}" << std::endl;
        file << std::endl;
        file << "template <>" << std::endl;
        file << "void cubos::core::data::deserialize<" << id << ">(Deserializer& des, " << id << "& obj)" << std::endl;
        file << "{" << std::endl;
        if (component.fields.size() == 1)
        {
            file << "    des.read(obj." << component.fields[0] << ");" << std::endl;
        }
        else
        {
            file << "    des.beginObject();" << std::endl;
            for (auto& field : component.fields)
            {
                file << "    des.read(obj." << field << ");" << std::endl;
            }
            file << "    des.endObject();" << std::endl;
        }
        file << "}" << std::endl;
        file << std::endl;
        if (!component.namespaceStr.empty())
        {
            file << "namespace " << component.namespaceStr << std::endl;
            file << "{" << std::endl;
            file << "    ";
        }
        file << "CUBOS_REGISTER_COMPONENT(" << component.typeStr << ", " << storageId << ", \"" << component.name
             << "\")" << std::endl;
        if (!component.namespaceStr.empty())
        {
            file << "} // namespace " << component.namespaceStr << std::endl;
        }
    }

    return true;
}

int runGenerate(int argc, char** argv)
{
    // Parse command line arguments.
    GenerateOptions options = {};
    if (!parseArguments(argc, argv, options))
    {
        printHelp();
        return 1;
    }
    if (options.help)
    {
        printHelp();
        return 0;
    }

    // Convert the input file.
    if (!generate(options))
    {
        std::cerr << "Failed to generate component source files." << std::endl;
        return 1;
    }

    return 0;
}
