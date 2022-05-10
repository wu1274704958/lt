#pragma once

#include <string>
#include <token_stream.hpp>
#include <dbg.hpp>
#include <strstream>
#include <serialization.hpp>

std::string get_pkg_name(std::vector<token::Token>& ts)
{
	bool in_manifest = false;
	bool catched = false;
	std::string pkg_name;
	for (int i = 0; i < ts.size(); ++i)
	{
		auto& t = ts[i];
		if (t.body == "manifest" && t.per == '<')
		{
			in_manifest = true;
		}
		if (in_manifest && t.back == '>')
		{
			in_manifest = false;
		}

		if (in_manifest && t.body == "package" && t.back == '=')
		{
			if (ts[i + 1].per == '"' && ts[i + 1].back == '"')
			{
				pkg_name = ts[i + 1].body;
				catched = true;
				dbg(pkg_name);
                break;
			}
		}
    }
    return pkg_name;
}

int64_t get_mothed_prologue(std::vector<token::Token>& ts,std::string mothed,bool is_state,std::string args)
{
    for(int i = 0;i < ts.size();++i)
    {
        auto& it = ts[i];
        if(it.per == '.' && it.body == "method" &&  
            (is_state ? ts[i + 2].body == "static" && ts[i + 3].body == mothed : ts[i + 2].body == mothed ))
        {
            std::string args_s;
            int b = is_state ? i + 4 : i + 3; 
            while (ts[b].per != '('){ ++b; }
            while (ts[b].back != ')' )
            {
                args_s += ts[b].to_string();
                ++b;
            }
            args_s += ts[b].to_string();

            if(args_s != args)
            {
                i = b;
                continue;
            }
            
            while(ts[b].body != "prologue" && (ts[b - 1].back != '.' || ts[b].per != '.')){++b;}
            
            while (ts[b].back != '\n')++b;
            
            if(ts[b + 1].body == "line" || ts[b+2].body == "line")
            {
                ++b;
                while (ts[b].back != '\n'){++b;}
                return b + 1;
                
            }else
            {
                return b + 1;
            }
        }
    }
    return -1;
}

std::tuple<std::string,int64_t> get_args_str(std::vector<token::Token>& ts,int b)
{
    std::string args_s;
    while (ts[b].per != '(') { ++b; }
    while (ts[b].back != ')')
    {
        args_s += ts[b].to_string();
        ++b;
    }
    args_s += ts[b].to_string();
    return { args_s,b };
}

std::tuple<std::string, int64_t> get_class_str(std::vector<token::Token>& ts, int b)
{
    std::string args_s;
    while (ts[b].body.empty() || ts[b].body[0] != 'L') { ++b; }
    args_s += ts[b].body;
    if (!ts[b].back_is_none())
        args_s += ts[b].back;
    ++b;
    while (ts[b].back != ';')
    {
        args_s += ts[b].to_string();
        ++b;
    }
    args_s += ts[b].to_string();
    return { args_s,b };
}

void rm_method(std::vector<token::Token>& ts,std::string name,std::string args)
{
    for(int i = 0;i < ts.size();++i)
    {
        auto& it = ts[i];
        
        //if(it.per == '.' && it.body == "method" && ts[i + 2].body == name && get_args_str(ts,i + 3) == args)
        {
            
        }
    }
}

void replace(std::filesystem::path& file,std::string f,std::string r)
{
    if(std::filesystem::exists(file))
    {
        auto file_path = file.generic_string();
        std::string str;
        {
            std::ifstream istr(file_path,std::ios::binary);
            dbg("read b");
            while(!istr.eof())
            {
                auto c = istr.get();
                if(c != 255 && c != -1)
                    str += c;
            }
            dbg("read e");
        }
        for(int i = 0;i < str.size();++i)
        {
            if(str[i] == f[0] && i + f.size() - 1 < str.size())
            {
                int j = 1;
                bool flag = true;
                for(;j < f.size();++j)
                {
                    if(str[i + j] != f[j])
                    {
                        flag = false;
                        break; 
                    }
                }
                if(flag)
                    str.replace(i,f.size(),r);
            }
        }
        if(!str.empty())
        {
            std::ofstream ostr(file_path,std::ios::binary);
            dbg("write b");
            ostr << str;
            ostr.flush();
            dbg("write e");
        }
    }
}

std::tuple<int,int64_t> get_tag_max(std::vector<token::Token>& ts, std::string mothed, bool is_state, std::string args,std::string tag)
{
    int b = get_mothed_prologue(ts, mothed, is_state, args);

    if (b >= 0)
    {
        int res = 0;
        for (int i = b; i < ts.size(); ++i)
        {
            auto& it = ts[i];

            if ((it.per == '.' && it.body == "end" && ts[i + 1].body == "method") || 
                (it.back == '.' && ts[i + 1].body == "end" && ts[i + 2].body == "method"))
            {
                break;
            }
            std::string* ftag = nullptr;
            if (it.per == ':' && it.body.find(tag) == 0)
            {
                ftag = &it.body;
            }
            if (it.back == ':' && ts[i + 1].body.find(tag) == 0)
            {
                ftag = &ts[i + 1].body;
            }
            if (ftag)
            {
                int n = wws::parser<int>(ftag->substr( ftag->find("_") + 1),16i64);
                if (n > res)
                    res = n;
            }
        }
        return std::make_tuple(res, b);
    }
    return std::make_tuple(-1, b);
}

std::string to_hex(int v)
{
    std::strstream ss;
    ss << std::hex << v << '\0';
    ss.flush();
    return std::string(ss.str());
}

int64_t get_invoke_mothed(std::vector<token::Token>& ts, std::string obj_name,std::string mothed, std::string args,int b)
{
    for (int i = b; i < ts.size(); ++i)
    {
        auto& it = ts[i];
        int res = 0;
        for (int i = b; i < ts.size(); ++i)
        {
            auto& it = ts[i];

            if ((it.per == '.' && it.body == "end" && ts[i + 1].body == "method") ||
                (it.back == '.' && ts[i + 1].body == "end" && ts[i + 2].body == "method"))
            {
                break;
            }
            if (it.body == "invoke" && it.back == '-')
            {
                auto [s,e] = get_class_str(ts, i);
                if (s == obj_name && ts[e + 2].body == mothed)
                {
                    auto [as, ae] = get_args_str(ts, e);
                    if (as == args)
                        return i;
                }
            }
        }
    }
    return -1;
}

/*
V   void
Z   boolean
B   byte
S   short
C   char
I   int
J   long
F   float
D   double
*/

inline int args_count(const std::string& args)
{
    int res = 0;
    int stage = 0;
    for (int i = 0; i < args.size(); ++i)
    {
        if (args[i] == 'Z' ||
            args[i] == 'B' ||
            args[i] == 'S' ||
            args[i] == 'C' ||
            args[i] == 'I' ||
            args[i] == 'J' ||
            args[i] == 'F' ||
            args[i] == 'D'
            )
        {
            ++res;
            stage = 0;
        }
        else if (args[i] == 'L')
        {
            ++stage;
        }
        else if (stage == 1 && args[i] == ';')
        {
            stage = 0;
            ++res;
        }
    }
    return res;
}