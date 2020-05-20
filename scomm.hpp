#pragma once

#include <string>
#include <token_stream.hpp>
#include <dbg.hpp>

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

std::string get_args_str(std::vector<token::Token>& ts,int idx)
{
    return "";
}

void rm_method(std::vector<token::Token>& ts,std::string name,std::string args)
{
    for(int i = 0;i < ts.size();++i)
    {
        auto& it = ts[i];
        
        if(it.per == '.' && it.body == "method" && ts[i + 2].body == name && get_args_str(ts,i + 3) == args)
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