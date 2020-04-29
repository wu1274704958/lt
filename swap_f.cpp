#include <iostream>
#include <filesystem>
#include <token_stream.hpp>
#include <print_stl.hpp>
#include <dbg.hpp>
#include <fstream>
#include <strstream>

using namespace token;
namespace fs = std::filesystem;
// Lcom/facebook/common
template <char S = '.'>
void swap(fs::path& f,const char* type);


int main(int argc, char** argv) {

    if(argc <= 2)
    {
        std::cout << "Not enough args!" << std::endl;
        return -1;
    }
    try{
        fs::path f(argv[1]);
        swap(f,argv[2]);
    }catch(std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}

template <char S>
void swap(fs::path& f,const char* type)
{
    std::string name = f.filename().generic_string();
    name += S;
    name += type;

    fs::path src = f.parent_path() / name.c_str();
    if(fs::exists(src))
    {
        if(fs::exists(f))
        {
            fs::remove(f);
        }
        fs::copy_file(src,f);
    }else
        throw std::runtime_error("The src file is not exists!!!");
    
}