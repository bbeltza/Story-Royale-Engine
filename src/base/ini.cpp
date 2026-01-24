#include <Base/Ini.hpp>
#include <Base/File.hpp>
#include <utils/logging.h>

#if HAVE_STRTOK_R
    #define strtok_s strtok_r
#endif

using namespace sre;

enum IniTokenType
{
    TOKEN_NONE = 0,
    TOKEN_NEWLINE,
    TOKEN_SECTION,
    TOKEN_ASSIGN,
    TOKEN_WORD,
    TOKEN_QUOTE
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

    std::string str_text{text};
    char in_quotes = 0;
    for (auto& chr : str_text)
    {
        if ((chr == '"' || chr == '\''))
        {
            if (in_quotes == chr)
            {
                in_quotes = 0;
                chr = ' ';
            }
            else if (in_quotes == 0)
            {
                in_quotes = chr;
                chr = ' ';
            }
        }
        else if (chr == ';' && !in_quotes)
            in_quotes = ';';
        
        if (in_quotes == ';')
        {
            if (chr == '\n')
                in_quotes = 0;
            else
                chr = ' ';
        }
        else if (chr == '\\' && !in_quotes)
        {
            const char* lineend = strchr(&chr, '\n');
            if (lineend)
                memset(&chr, ' ', lineend - &chr + 1);
        }
    }

    std::vector<char*> words;
    char* context;
    char* tk = strtok_s(&str_text.at(0), " ", &context);
    while (tk != NULL)
    {
        words.push_back(tk);
        tk = strtok_s(NULL, " ", &context);
    }

    std::queue<IniToken> tokens;
    for (char* word : words)
    {
        BEGIN:
        char* nl = strchr(word, '\n');
        if (nl)
            *nl = '\0';
        if (*word == '[')
        {
            char* sec_end = strchr(word, ']');
            if (!sec_end)
                return false;
            
            IniToken token {TOKEN_SECTION};
            token.value.assign(word + 1, sec_end - word - 1);
            tokens.push(std::move(token));
        }
        else
        {
            char* assign = strchr(word, '=');
            if (assign)
                *assign = '\0';

            if (*word)
            {
                IniToken token {TOKEN_WORD};
                token.value.assign(word);
                tokens.push(std::move(token));
            }
            if (assign)
            {
                tokens.push({ TOKEN_ASSIGN });
                word = assign + 1;
                goto BEGIN;
            }
        }

        if (nl)
        {
            tokens.push({ TOKEN_NEWLINE });
            word = nl + 1;
            goto BEGIN;
        }
    }

    /*
    for (auto& token : tokens._Get_container()) // Only works on MSVC I guess, don't worry it's just to debug...
    {
        const char* type;
        switch (token.type)
        {
            case TOKEN_ASSIGN: type = "TOKEN_ASSIGN"; break;
            case TOKEN_NEWLINE: type = "TOKEN_NEWLINE"; break;
            case TOKEN_SECTION: type = "TOKEN_SECTION"; break;
            case TOKEN_WORD: type = "TOKEN_WORD"; break;
            default: type = "TOKEN_NONE"; break;
        }

        LOG("Got token '%s': '%s'", type, token.value.c_str());
    }
    
    LOG("");
    */

    IniTokenType current_type = TOKEN_NEWLINE;
    string current_section;
    string current_key;

    while (!tokens.empty())
    {
        IniToken& token = tokens.front();
        
        switch (current_type)
        {
        case TOKEN_SECTION:
            // Should except TOKEN_NEWLINE, otherwise drop error
            if (token.type != TOKEN_NEWLINE)
                break;
            
            current_type = TOKEN_NEWLINE;
            break;
        case TOKEN_WORD:
            // TOKEN_WORD is only current_type when parsing the variable name, so it should only expect TOKEN_ASSIGN
            if (token.type != TOKEN_ASSIGN)
                break;
            
            current_type = TOKEN_ASSIGN;
            break;
        case TOKEN_ASSIGN:
            if (token.type == TOKEN_WORD)
            {
                string& val = sections[current_section][current_key];
                if (!val.empty())
                    val.push_back(' ');
                val.append(std::move(token.value));
            }
            else if (token.type == TOKEN_NEWLINE)
                current_type = TOKEN_NEWLINE;
            else
                ;// Drop error...
            break;
        default:
            switch (token.type)
            {
            case TOKEN_SECTION:
                current_section = std::move(token.value);
                break;
            case TOKEN_ASSIGN:
                // TOKEN_ASSIGN is not expected here, so drop error
                break;
            case TOKEN_WORD:
                current_key = std::move(token.value);
            default:
                current_type = token.type;
                break;
            }
            break;
        }

        tokens.pop();
    }

    return true;
}

bool IniFile::save(const char* path)
{
    string text;
    text.reserve(24);

    for (auto& section : sections)
    {
        text.push_back('[');
        text.append(section.first);
        text.append("]\n\n");

        for (auto& vars : section.second)
        {
            text.append(vars.first);
            text.append(" = ");

            // Check if variable contains spaces
            if (vars.second.find(' ') != string::npos)
            {
                text.push_back('\"');
                text.append(vars.second);
                text.push_back('\"');
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