#include <json.hpp>
#include <fileop.hpp>
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
    std::string k = argv[2],v = argv[3];

    char type = 'i';
    if(argc >= 5)
    {
        type = argv[4][0];
    }
        

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
    
    if(json.has_key(k))
    {
        if(type == 's')
            json.put(k,v);
        else
            json.put(k,wws::parser<int>(v));

        std::string ou = json.to_string();

        wws::write_to_file(f,ou,std::ios::binary);

    }else{
        std::cout << "not find key "<< k <<"!!!" <<std::endl; 
        return -1;
    }
}

