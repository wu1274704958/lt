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