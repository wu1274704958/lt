#include <json.hpp>
#include <fileop.hpp>
#include <scomm.hpp>
#include "client/verify.hpp"

namespace fs = std::filesystem;

int main(int argc, char** argv) 
{
    VERIFICATION
    if(argc <= 3)
    {
        std::cout << "bad args!!!" <<std::endl; 
        return -1;
    }

    fs::path f(argv[1]);
    std::string old_id = argv[2],new_id = argv[3];

    if(!fs::exists(f))
    {
        std::cout << "bad file path!!!" <<std::endl; 
        return -1;
    }
    
    ::replace(f,old_id,new_id);
    return 0;
   
}

