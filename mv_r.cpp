#include <iostream>
#include <filesystem>
#include <token_stream.hpp>
#include <print_stl.hpp>
#include <dbg.hpp>
#include <fstream>
#include <strstream>
#include "client/verify.hpp"

using namespace token;
namespace fs = std::filesystem;
// Lcom/facebook/common

std::tuple<fs::path,fs::path> parser_sd(fs::path& root,const char* src,const char *dst);
std::vector<fs::path> all_r_files(fs::path& root);
std::vector<fs::path> copy_to(std::vector<fs::path>& srcs,fs::path dst);
std::string get_smali_pkg(std::string ori);
void replace(fs::path& file,std::string f,std::string r);

int main(int argc, char** argv) {
    VERIFICATION
    if(argc < 4)
        return -1;

    fs::path root(argv[1]);
    if(!fs::exists(root) || !fs::is_directory(root))
        return -1;

    dbg("go");

    try{

        auto [src,dst] = parser_sd(root,argv[2],argv[3]);
        dbg(src.generic_string());
        dbg(dst.generic_string());

        auto src_rs = all_r_files(src);
        auto dst_rs = copy_to(src_rs,dst);

        auto src_pkg = get_smali_pkg(argv[2]);
        auto dst_pkg = get_smali_pkg(argv[3]);
        
        dbg(src_pkg);
        dbg(dst_pkg);

       
        for(auto& ch : dst_rs)
        {
            dbg(ch.generic_string());
            replace(ch,src_pkg,dst_pkg);
        }
        
    }catch(std::exception e)
    {
        dbg(e.what());
    }
    
    return 0;
}

std::tuple<fs::path,fs::path> parser_sd(fs::path& root,const char* src,const char *dst)
{
    TokenStream<std::string> src_t(src);
    TokenStream<std::string> dst_t(dst);

    src_t.analyse();
    dst_t.analyse();

    auto& sts = src_t.tokens;
    auto& dts = dst_t.tokens;
    
    fs::path src_r = root;
    fs::path dst_r = root;
    
    for(auto& t : sts)
    {
        if(!t.body.empty())
            src_r.append(t.body);
    }

    for(auto& t : dts)
    {
        if(!t.body.empty())
            dst_r.append(t.body);
    }

    if(!fs::exists(src_r))
        throw std::runtime_error("src not exists!!!");
    if(!fs::exists(dst_r))
    {
        if(!fs::create_directory(dst_r))
            throw std::runtime_error("dst create failed!!!");
    }

    return std::make_tuple(src_r,dst_r);
}

std::vector<fs::path> all_r_files(fs::path& root)
{
    std::vector<fs::path> res;
    for(auto& ch : fs::directory_iterator(root))
    {
        if(ch.is_regular_file())
        {
            auto fn = ((fs::path)ch).filename().generic_string();
            if(fn.size() >= 2 && fn[0] == 'R' && (fn[1] == '.' || fn[1] == '$')) 
                res.push_back(ch);
        }
    }
    return res;
}

std::vector<fs::path> copy_to(std::vector<fs::path>& srcs,fs::path dst)
{
    std::vector<fs::path> res;
    for(auto& ch : srcs)
    {
        auto fn = ch.filename().generic_string();
        fs::path temp = dst;
        temp.append(fn);

        fs::copy_file(ch,temp,fs::copy_options::overwrite_existing);
        res.push_back(temp);
    }
    return res;
}

std::string get_smali_pkg(std::string ori)
{
    std::string res;
    if(ori.empty())
        return res;
    
    res += 'L';
    for(auto c : ori)
    {
        if(c == '.')
            c = '/';
        res += c;
    }
    return res;
}

void replace(fs::path& file,std::string f,std::string r)
{
    if(fs::exists(file))
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