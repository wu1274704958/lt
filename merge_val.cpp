#include <iostream>
#include <filesystem>
#include <token_stream.hpp>
#include <print_stl.hpp>
#include <dbg.hpp>
#include <fstream>
#include "client/verify.hpp"

using namespace token;
namespace fs = std::filesystem;

bool need_merge(std::string& a,std::string& b);
bool merge(fs::path& a,fs::path& b);

int main(int argc, char** argv) {
    VERIFICATION
    if(argc < 2)
        return -1;

    fs::path root(argv[1]);
    if(!fs::exists(root) || !fs::is_directory(root))
        return -1;
    dbg("go");
    std::vector<fs::path> wait_rm;
    try{
        for(auto& ch : fs::directory_iterator(root))
        {
            if(fs::is_directory(ch))
            {
                auto pt = (fs::path)ch;
                auto fn = pt.filename();
                auto name = fn.generic_string(); 
                //dbg(name);
                TokenStream<std::string> temp(name);
                temp.analyse();
                if(temp.tokens.empty())
                    continue;
                if(temp.tokens[0].body == "values")
                {
                    std::vector<std::pair<std::string,fs::path>> childern;
                    for(auto& val_unc : fs::directory_iterator(ch))
                    {
                        auto tpt = (fs::path)val_unc;
                        auto tfn = tpt.filename();
                        auto tname = tfn.generic_string(); 
                        childern.push_back({std::move(tname),val_unc});
                    }
                    
                    for(int i = 0;i < childern.size();++i)
                    {
                        for(int j = i + 1; j < childern.size();++j)
                        {
                            if(need_merge(childern[i].first,childern[j].first))
                            {
                                bool f =false;
                                if(childern[i].first.size() > childern[j].first.size())
                                {
                                    f = merge(childern[j].second,childern[i].second);
                                    if(f)
                                        wait_rm.push_back(childern[i].second);
                                }else{
                                    f = merge(childern[i].second,childern[j].second);
                                    if(f)
                                        wait_rm.push_back(childern[j].second);
                                }
                            }
                        }
                    }
                }
            }
        }
    }catch(std::exception e)
    {
        dbg(e.what());
    }
    for(auto& a : wait_rm)
        fs::remove(a);
    return 0;
}


bool need_merge(std::string& a,std::string& b)
{
    if(a == b)
        return false;
    TokenStream<std::string> at(a);
    TokenStream<std::string> bt(b);

    at.analyse();
    bt.analyse();

    auto& ats = at.tokens;
    auto& bts = bt.tokens;

   
    
    if(ats.empty() || bts.empty())
        return false;
    int al2 = ats.size() - 2;
    int bl2 = bts.size() - 2;
    int al1 = ats.size() - 1;
    int bl1 = bts.size() - 1;


    if( ats[al1].body == "xml" && bts[bl1].body == "xml" &&
         (( ats[al2].per == '(' && ats[al2].back == ')' ) ||  ( bts[bl2].per == '(' && bts[bl2].back == ')' ))
          )
    {
        int i = 0;
        while(i < ats.size() || i < bts.size())
        {
            if(ats[i].per == '(' || bts[i].per == '(')
                break;
            if(ats[i].body != bts[i].body)
                return false;
            ++i;
        }
        return true;        
    }
    return false;
}

bool merge(fs::path& a,fs::path& b)
{
    if(!fs::exists(a) || !fs::exists(b))
        return false;
    std::string cnt;

    std::ifstream af(a.generic_string());
    std::ifstream bf(b.generic_string());

    TokenStream<std::ifstream> at(std::move(af));
    TokenStream<std::ifstream> bt(std::move(bf));
    dbg(a.generic_string());
    at.analyse();
    dbg("analyse 1");
    bt.analyse();
    dbg("analyse 2");

    auto& ats = at.tokens;
    auto& bts = bt.tokens;

    int in_res = 0;
    for(int i = 0;i < bts.size();++i)
    {
        if(bts[i].body == "resources" && bts[i].per == '<')
            ++in_res;
        if(in_res == 1)
        {
            while(bts[i].back != '>')
            {
                ++i;
            }
            ++i;
            ++in_res;
        }
        if(in_res == 2)
        {
            while(i < bts.size())
            {
                if(bts[i].per == '<' && bts[i].back == '/' && bts[i + 1].body == "resources")
                    break;
                if(bts[i].per != Token::None)
                    cnt += bts[i].per;
                if(!bts[i].body.empty())
                    cnt += bts[i].body;
                if(bts[i].back != Token::None)
                    cnt += bts[i].back;
                ++i;
            }
            break;
        }
    }

    int insert = -1;

    for(int i = 0;i < ats.size();++i)
    {
        if(ats[i].per == '<' && ats[i].back == '/' && ats[i + 1].body == "resources")
        {
            insert = i;
            break;
        }
    }
    if(!cnt.empty() && insert > 0)
    {
        ats.insert(ats.begin() + insert,Token(cnt,' ','\n'));
        auto str = a.generic_string();
        dbg(str);
        at.save(str,true);
        dbg("merge success!!!");
        return true;
    }else
    {
        dbg("merge failed!!!");
        return false;
    }
}