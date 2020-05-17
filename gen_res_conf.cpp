#include <json.hpp>
#include <tools/thread_pool.h>
#include <filesystem>
#include <dbg.hpp>
#include "FindPath.hpp"
#include <fileop.hpp>

namespace fs = std::filesystem;
using namespace std;

void fill_assets_dir(vector<string>&,fs::path& root);

int main(int argc,char **argv)
{

    if(argc < 3)
    {
        std::cerr << "Bad args!" << std::endl;
        std::cerr << "assets_path out_path" << std::endl;
        return -1;
    }
    fs::path ass_path(argv[1]);
    fs::path out_path(argv[2]);

    if(!fs::exists(ass_path) )
        std::cerr << "assets_path not exists!" << std::endl;
    if(fs::exists(out_path))
        fs::remove(out_path);
    
    dbg(ass_path);

    std::vector<string> file_ls;
    fill_assets_dir(file_ls,ass_path);

    std::string res;

    for(auto& s : file_ls)
    {
        res += s;
        res += '\n';
    }

    wws::write_to_file(out_path,res,std::ios::binary);

    return 0;
}

void fill_assets_dir(vector<string>& v,fs::path& root)
{
    wws::enum_path(root,[&v](const fs::path& p)
    {
        string path = fs::absolute(p).generic_string();
        v.push_back( path.substr(path.find("assets/")));
    });
}