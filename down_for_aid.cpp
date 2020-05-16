#include <json.hpp>
#include <tools/thread_pool.h>
#include <filesystem>
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
        std::cerr << "aid cdn out_path" << std::endl;
        return -1;
    }
    string aid_s(argv[1]);
    int aid = 1000000000 + wws::parser<int>(aid_s);
    auto cdn = argv[2];
    auto out_path = fs::path(argv[3]);
    
    string cq = "cq";
    cq += wws::to_string(aid);

    download(cdn,cq + "/ui_android.dat",out_path);
    download(cdn,cq + "/res.dat"       ,out_path);
    download(cdn,cq + "/ui_xml.dat"    ,out_path);
    download(cdn,cq + "/ui_ios.dat"    ,out_path);
    download(cdn,cq + "/WebMain.swf"   ,out_path);
    download(cdn,cq + "/ui_xml_ios.dat",out_path);
    return 0;
}
