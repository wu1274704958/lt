#include <json.hpp>
#include <fileop.hpp>
#include "client/verify.hpp"
#include <filesystem>
#include "FindPath.hpp"
#include <tools/convert.h>


namespace fs = std::filesystem;

void go(std::vector<token::Token>& ts, std::string& key, std::string& name);

int main(int argc, char** argv)
{
    VERIFICATION
    if (argc < 4)
    {
        std::cout << "bad args!!!  out_dir key name" << std::endl;
        return -1;
    }

    fs::path out(argv[1]);
    std::string name_key(argv[2]);
    std::string new_name(argv[3]);

    new_name = cvt::l_utf8(new_name);

    if (!fs::exists(out))
    {
        std::cout << "out_dir not exists!!!" << std::endl;
        return -1;
    }

    fs::path res = out / "res";
    
    auto [find,chs] = wws::find_paths_ex(res, [](const fs::path& p)->bool {
        if (fs::is_directory(p))
        {
            auto n = p.filename().generic_string();
            if (n.find("values") == 0)
            {
                return true;
            }
        }
        return false;
    });

    if (!find || chs.empty())
    {
        std::cout << "not find values!!!" << std::endl;
        return -1;
    }

    for (auto& p : chs)
    {
        auto str_f = p / "strings.xml";
        if (fs::exists(str_f))
        {
            auto str_path = str_f.generic_string();
            
            std::ifstream f(str_path, std::ios::binary);
            token::TokenStream<std::ifstream> ts(std::move(f));
            ts.analyse();

            go(ts.tokens, name_key,new_name);

            ts.save(str_path, true);
        }
    }

    return 0;
}

void go(std::vector<token::Token>& ts, std::string& key, std::string& name)
{
    bool in_string = false;
    for(int i = 0;i < ts.size();++i)
    {
        auto& it = ts[i];
        if (it.body == "string" && it.per == '/' && it.back == '>')
        {
            in_string = false;
            continue;
        }
        if (it.body == "string" && it.per == '<' && it.back == ' ')
        {
            in_string = true;
            continue;
        }
        if (in_string && it.body == "name")
        {
            while (ts[i].per != '"') { ++i; }
            if (ts[i].body == key)
            {
                while (ts[i].back != '>') { ++i; }
                ++i;
                int e = i;
                while (ts[e].per != '<') { ++e; }
                ts.erase(ts.begin() + i, ts.begin() + e);
                ts.insert(ts.begin() + i, token::Token(name));
                break;
            }
        }
    }
}


#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <locale>
#include <codecvt>
#include <Windows.h>
#include <iostream>

namespace cvt {

    std::string unicode2utf8(const std::wstring& ws)
    {
        std::string ret;
        try {
            std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
            ret = wcv.to_bytes(ws);
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
        return ret;
    }

    std::wstring utf82unicode(const std::string& s)
    {
        std::wstring ret;
        try {
            std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
            ret = wcv.from_bytes(s);
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
        return ret;
    }

    std::string unicode2ansi(const std::wstring& ws)
    {
        int ansiiLen = WideCharToMultiByte(CP_ACP, 0, ws.c_str(), -1, nullptr, 0, nullptr, nullptr);
        char* pAssii = (char*)malloc(sizeof(char) * ansiiLen);
        WideCharToMultiByte(CP_ACP, 0, ws.c_str(), -1, pAssii, ansiiLen, nullptr, nullptr);
        std::string ret_str = pAssii;
        free(pAssii);
        return ret_str;
    }

    std::wstring ansi2unicode(const std::string& s)
    {
        int unicodeLen = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, nullptr, 0);
        wchar_t* pUnicode = (wchar_t*)malloc(sizeof(wchar_t) * unicodeLen);
        MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, pUnicode, unicodeLen);
        std::wstring ret_str = pUnicode;
        free(pUnicode);
        return ret_str;
    }

    std::string utf8_l(const std::string& str)
    {
        return unicode2ansi(utf82unicode(str));
    }

    std::string l_utf8(const std::string& str)
    {
        return unicode2utf8(ansi2unicode(str));
    }
}