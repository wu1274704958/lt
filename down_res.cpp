#include <json.hpp>
#include <tools/thread_pool.h>
#include <filesystem>
#include <print_stl.hpp>
#include <dbg.hpp>
#include "FindPath.hpp"
#include <fileop.hpp>
#include "curl.hpp"

namespace fs = std::filesystem;
using namespace std;


int main(int argc,char **argv)
{

    if(argc < 4)
    {
        std::cerr << "Bad args!" << std::endl;
        std::cerr << "cdn config out_path" << std::endl;
        return -1;
    }
    
    auto cdn = argv[1];
    auto conf = fs::path(argv[2]);
    auto out_path = fs::path(argv[3]);

    auto config_str = wws::read_from_file<1024>(conf,std::ios::binary);
    vector<string> configs;

    if(config_str)
    {
        string& s = *config_str;
        int b = 0;
        for(int i = 0;i < s.size();++i)
        {
            if(s[i] == '\n')
            {
                configs.push_back(s.substr(b,i - b));
                if(configs.back().back() == '\r')
                    configs.back().pop_back();
                b = i + 1;
            }
        }
    }else
    {
        std::cerr << "read config failed!" <<std::endl;
        return -1;
    }
    if(configs.empty())
    {
        std::cerr << "Not one!" <<std::endl;
        return -1;
    }


    wws::thread_pool tp(6);
    int alloct_size = 0;
    int unit_size = configs.size() / 6; 


    for(int i = 0;i < 6;++i)
    {
        int b = alloct_size;
        int e = alloct_size + unit_size;
        tp.add_task([b,e,&configs,cdn,&out_path](){
            for(int i = b;i < e;++i)
            {
                download(cdn,configs[i],out_path);
            }
        }); 
        alloct_size = e;
    }
    if(alloct_size < configs.size())
    {
        int b = alloct_size;
        int e = configs.size();
        tp.add_task([b,e,&configs,cdn,&out_path](){
            for(int i = b;i < e;++i)
            {
                download(cdn,configs[i],out_path);
            }
        });
    }

    while(tp.has_not_dispatched()){ this_thread::sleep_for(std::chrono::milliseconds(2));}
    tp.wait_all();
    return 0;

}
    
    