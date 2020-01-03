#include <iostream>
#include <filesystem>
#include <dbg.hpp>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    if(argc <= 2) return -1;
    fs::path out(argv[2]);
    fs::path root(argv[1]);

    if(!fs::exists(out))
        fs::create_directory(out);
    if(fs::exists(root))
    {
        try{
        for(auto& ch : fs::directory_iterator(root))
        {
            if(fs::is_directory(ch))
            {
                fs::path& p = (fs::path)ch;
                fs::path unuse = p;
                unuse.append(".DS_Store");
                if(fs::exists(unuse))
                    fs::remove(unuse);
                fs::path tmp = out;
                fs::path fn = p.filename();
                tmp.append(fn.c_str());
                fs::create_directory(tmp);

                fs::copy(ch,tmp,fs::copy_options::overwrite_existing);
            }else if(fs::is_regular_file(ch)){
                fs::path& p = (fs::path)ch;
                if(p.extension() == ".png" || p.extension() == ".PNG")
                {
                    fs::path fn = p.stem();
                    fs::path tmp = out;
                    tmp.append(fn.c_str());
                    if(!fs::exists(tmp))
                    {
                        fs::create_directory(tmp);
                        fs::path ne0 = tmp;
                        fs::path ne1 = tmp;
                        fs::path ne2 = tmp;
                        ne0.append("00.png");
                        ne1.append("01.png");
                        ne2.append("02.png");
                        fs::copy_file(p,ne0);
                        fs::copy_file(p,ne1);
                        fs::copy_file(p,ne2);
                    }
                }   
            }
        }
        }catch(std::exception e)
        {
            dbg(e.what());
        }
        
    }
}
