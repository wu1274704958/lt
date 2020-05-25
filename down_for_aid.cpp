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

    vector<string> ns;
    ns.push_back(cq + "/ui_android.dat");
    ns.push_back(cq + "/res.dat"       );
    ns.push_back(cq + "/ui_xml.dat"    );
    ns.push_back(cq + "/ui_ios.dat"    );
    ns.push_back(cq + "/WebMain.swf"   );
    ns.push_back(cq + "/ui_xml_ios.dat");

    wws::thread_pool tp(6);
    
    for(auto& n: ns)
    {
        tp.add_task([cdn,&n,&out_path](){
            download(cdn,n,out_path);
        });
    }

    while(tp.has_not_dispatched()){ this_thread::sleep_for(std::chrono::milliseconds(2));}
    tp.wait_all();

    return 0;
}
