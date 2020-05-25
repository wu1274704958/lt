#include <json.hpp>
#include <tools/thread_pool.h>
#include <filesystem>
#include <dbg.hpp>
#include "FindPath.hpp"
#include <fileop.hpp>
#include "client/verify.hpp"

namespace fs = std::filesystem;
using namespace std;

void fill_aid_need(vector<string>&,int);
void fill_assets_dir(vector<string>&,fs::path& root);

int main(int argc,char **argv)
{
    VERIFICATION
    if(argc < 5)
    {
        std::cerr << "Bad args!" << std::endl;
        std::cerr << "aid assets_path v2_path out_path" << std::endl;
        return -1;
    }
    string aid_s(argv[1]);
    int aid = 1000000000 + wws::parser<int>(aid_s);
    fs::path ass_path(argv[2]);
    fs::path v2_path(argv[3]);
    fs::path out_path(argv[4]);

    if(!fs::exists(ass_path) || !fs::exists(v2_path))
        std::cerr << "assets_path or v2_path not exists!" << std::endl;
    if(fs::exists(out_path))
        fs::remove(out_path);
    
    dbg(aid);
    dbg(ass_path);

    std::vector<string> file_ls;
    fill_aid_need(file_ls,aid);
    fill_assets_dir(file_ls,ass_path);

    auto v2_data = wws::read_from_file<1024>(v2_path,std::ios::binary);

    if(!v2_data)
    {
        std::cout << "v2 read file failed!!!" <<std::endl; 
        return -1;
    }

    wws::Json json(*v2_data);
    wws::Json out_json;

    for(auto& s : file_ls)
    {
        if(json.has_key(s))
        {
            out_json.put(s,json.get<string>(s));
        }else{
            std::cout << "not find key " << s << " in v2 !" <<std::endl; 
        }
    }
    
    auto out_str = out_json.to_string();
    wws::write_to_file(out_path,out_str,std::ios::binary);

    return 0;
}

void fill_aid_need(vector<string>& v,int aid)
{
    std::string prefix("cq");
    prefix += wws::to_string(aid);
    prefix += "/";
    v.push_back(prefix + "ui_android.dat"   );
    v.push_back(prefix + "res.dat"          );
    v.push_back(prefix + "ui_xml.dat"       );
    v.push_back(prefix + "ui_ios.dat"       );
    v.push_back(prefix + "WebMain.swf"      );
    v.push_back(prefix + "ui_xml_ios.dat"   );
}

void fill_assets_dir(vector<string>& v,fs::path& root)
{
    wws::enum_path(root,[&v](const fs::path& p)
    {
        string path = fs::absolute(p).generic_string();
        v.push_back( path.substr(path.find("assets/")));
    });
}