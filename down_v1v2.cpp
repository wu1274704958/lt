#include <json.hpp>
#include <tools/thread_pool.h>
#include <filesystem>
#include <dbg.hpp>
#include "FindPath.hpp"
#include <fileop.hpp>
#include "curl.hpp"
#include "client/verify.hpp"

namespace fs = std::filesystem;
using namespace std;


int main(int argc,char **argv)
{
    VERIFICATION
    if(argc < 3)
    {
        std::cerr << "Bad args!" << std::endl;
        std::cerr << "cdn out_path" << std::endl;
        return -1;
    }
    
    auto cdn = argv[1];
    auto out_path = fs::path(argv[2]);

    wws::thread_pool tp(2);

    tp.add_task([cdn,&out_path](){
        download(cdn,"v1.dat",out_path);
    });
    tp.add_task([cdn,&out_path](){
        download(cdn,"v2.dat","v2_new.dat",out_path);
    });
    tp.add_task([cdn,&out_path](){
        download(cdn,"v2.json",out_path);
    });
    while(tp.has_not_dispatched()){ this_thread::sleep_for(std::chrono::milliseconds(2));}
    tp.wait_all();
    
    return 0;
}
