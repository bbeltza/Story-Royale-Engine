#include <Base/Ini.hpp>
#include <Base/File.hpp>
#include <utils/logging.h>

using namespace sre;

enum IniTokenType
{
    TOKEN_NONE = 0,
    TOKEN_SECTION,
    TOKEN_VAR,
    TOKEN_VALUE,
    TOKEN_COMMENT
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
    IniToken current_token{TOKEN_NONE};

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
            
        }
        LOG("%c", *current);

        LOOPEND:
        continue;
    }

    while (!tokens.empty())
    {
        LOG("%d %s", tokens.front().type, tokens.front().value.c_str());
        tokens.pop();
    }


    return false;
}