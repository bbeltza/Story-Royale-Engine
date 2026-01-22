#include <Base/Ini.hpp>
#include <Base/File.hpp>
#include <utils/logging.h>

using namespace sre;

enum IniTokenType
{
    TOKEN_NONE = 0,
    TOKEN_SECTION,
    TOKEN_ASSIGNMENT
};

struct IniToken
{
    IniTokenType type;
    IniFile::string value;
};

bool IniFile::load(const char* from_path)
{
    // Perform a debug extension check
    const char* extension = strrchr(from_path, '.');
    if (!extension || strcmp(extension, ".ini"))
        WARN("IniFile::load(): Path '%s' does not have the '.ini' extension. Is it intended?", from_path);

    File f(from_path, "r");
    return load(f);
}

bool IniFile::load(const File& from_file)
{    
    Chunk chunk = from_file.allocate();
    if (!chunk) return false;

    return load_text(reinterpret_cast<const char*>(chunk->data));
}

bool IniFile::load_text(const char* text)
{
    if (!text) return false;

    std::queue<IniToken> tokens;

    for (const char* current = text; current - 1; current = strchr(current, '\n') + 1)
    {
        do
        {
            if (*current == '\n')
                goto LOOPEND;
            if (*current == ' ' || *current == '\t')
                *current++;
            else
                break;
        }
        while (true);
            
        if (*current == '[')
        {
            const char* section_end = strchr(current, ']');
            if (!section_end)
            {
                ERROR("IniFile::load_text(): Invalid section end at line %d", 'X');
                continue;
            }

            IniToken token { TOKEN_SECTION };
            token.value.assign(current, section_end - current + 1);
            tokens.push(std::move(token));
        }
        else if (*current == ';');
        else
        {
            const char* end = strchr(current, ';');
            end = end ? end : strchr(current, '\n');
            end = end ? end : strchr(current, '\0');
            assert(end != NULL);
            
            IniToken token { TOKEN_ASSIGNMENT };
            token.value.assign(current, end - current);
            tokens.push(std::move(token));
        }

        LOOPEND:
        continue;
    }

    string current_section;

    while (!tokens.empty())
    {
        IniToken& token = tokens.front();
        switch (token.type)
        {
        case TOKEN_SECTION:
            assert(token.value.front() == '[' && token.value.back() == ']');
            token.value.pop_back();
            current_section.assign(token.value.begin() + 1, token.value.end());
            LOG("%s %s", "TOKEN_SECTION", current_section.c_str());
            break;
        case TOKEN_ASSIGNMENT:
            {
                size_t offs = token.value.find('=');
                if (offs == token.value.npos)
                {
                    ERROR("Bad assignment in line '%d', could not find assignment operator", 'X');
                    break;
                }
                if (offs == 0)
                {
                    ERROR("Bad assignment in line '%d, there's no text before assignment operator", 'X');
                    break;
                }

                string key(token.value.c_str(), offs);
                string value(token.value.begin() + offs + 1, token.value.end());
                const char* value_cstr = value.c_str();
                while (*value_cstr == ' ')
                    value_cstr++;

                while (key.back() == ' ')
                    key.pop_back();
                while (value.back() == ' ')
                    value.pop_back();

                sections[current_section][key] = value_cstr;
                LOG("%s - '%s' '%s'", "TOKEN_ASSIGNMENT", key.c_str(), value_cstr);
            }
            break;
        default:
            break;
        }
        tokens.pop();
    }


    return false;
}

bool IniFile::save(const char* path)
{
    string text;
    text.reserve(24);

    for (auto& section : sections)
    {
        text.push_back('[');
        text.append(section.first);
        text.append("]\n");

        for (auto& vars : section.second)
        {
            text.append(vars.first);
            text.append(" = ");

            // Check if variable contains spaces
            if (vars.second.find(' ') != string::npos)
            {
                text.push_back('\'');
                text.append(vars.second);
                text.push_back('\'');
            }
            else
                text.append(vars.second);
            
            text.push_back('\n');
        }

        text.push_back('\n');
    }

    File file(path, "w");
    return file.write(text.data(), text.size());
}