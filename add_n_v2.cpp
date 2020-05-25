#include <json.hpp>
#include <fileop.hpp>
#include "client/verify.hpp"

namespace fs = std::filesystem;

int main(int argc, char** argv) 
{
    VERIFICATION
    if(argc <= 1)
    {
        std::cout << "bad args!!!" <<std::endl; 
        return -1;
    }

    fs::path f(argv[1]);
    
    if(!fs::exists(f))
    {
        std::cout << "bad file path!!!" <<std::endl; 
        return -1;
    }
    
    auto res = wws::read_from_file<1024>(f,std::ios::binary);

    if(!res)
    {
        std::cout << "read file failed!!!" <<std::endl; 
        return -1;
    }

    wws::Json json(*res);
    
    json.put("is_nv2",1);
    auto str = json.to_string();
    
    wws::write_to_file(f,str,std::ios::binary);

    return 0;
}

