#include <scomm.hpp>
#include <filesystem>
#include <token_stream.hpp>
#include "client/verify.hpp"

namespace fs = std::filesystem;
using namespace token;

bool go_link(std::vector<Token>& ts,std::string& name,std::string& args );

int main(int argc, char** argv)
{
    VERIFICATION
    if (argc < 4)
    {
        std::cerr << "out_pkg method_name method_args" << std::endl;
        return -1;
    }
        
    fs::path root(argv[1]);
    std::string name(argv[2]);
    std::string args(argv[3]);

    if (!fs::exists(root))
        return -1;

    fs::path manifest = root;
    manifest /= "AndroidManifest.xml";

    if (!fs::exists(manifest))
        return -1;

    auto manifest_s = manifest.string();

    std::ifstream ifs(manifest_s.c_str(), std::ios::binary);
    TokenStream<std::ifstream> tss(std::move(ifs));

    tss.analyse();

    auto pkg = get_pkg_name(tss.tokens);

    for (auto& c : pkg)
    {
        if (c == '.') c = '/';
    }

    auto entry = root;
    entry /= "smali";
    entry /= pkg;
    entry /= "AppEntry.smali";

    auto entry_s = dbg(entry.string());

    std::ifstream ifs2(entry_s.c_str(), std::ios::binary);
    TokenStream<std::ifstream> tss2(std::move(ifs2));

    tss2.analyse();

    if (go_link(tss2.tokens,name,args))
    {
        std::ofstream ofs(entry_s.c_str(), std::ios::binary);
        tss2.save(ofs);
    }
    else {
        std::cerr << "link failed !!!" << std::endl;
    }

    return 0;
}

std::string gen_invoke_args(int ac)
{
    std::string res("{");
    for (int i = 0; i < ac; ++i)
    {
        res += 'p';
        res += wws::to_string(i);
        if(i < ac - 1)
            res += ", ";
    }
    res += '}';
    return res;
}

bool go_link(std::vector<Token>& ts, std::string& name, std::string& args)
{
    
    auto b = get_mothed_prologue(ts, name, false, args);
    auto nb = get_invoke_mothed(ts, "Landroid/app/Activity;", name, args,b);
    if (nb >= 0)
    {
        while (ts[nb].back != '\n') { ++nb; }
        b = nb + 1;
    }
    args.insert(1, "Landroid/app/Activity;");
    int ac = args_count(args);
    dbg(ac);
   
    std::string ias = gen_invoke_args(ac);
    if (b >= 0)
    {

        std::strstream ss;

        ss << "invoke-static " << ias << ", " << "Lane/com/nativelib/QuickSdk;->" << name << args << "V" << '\0';
        ss.flush();
        
        dbg(b);
        dbg(ts[b].to_string());
        ts.insert(ts.begin() + b,
            Token(ss.str(), ' ', '\n'));
        return true;
    }
    else    
        return false;


    return true;
}