#include <scomm.hpp>
#include <filesystem>
#include <token_stream.hpp>
#include "client/verify.hpp"

namespace fs = std::filesystem;
using namespace token;

bool go_link(std::vector<Token>& ts);

int main(int argc, char** argv) 
{
    VERIFICATION
    if(argc == 1)
        return -1;

    fs::path root(argv[1]);

    if(!fs::exists(root))
        return -1;

    fs::path manifest = root;
    manifest /= "AndroidManifest.xml";

    if(!fs::exists(manifest))
        return -1;

    auto manifest_s = manifest.string();

    std::ifstream ifs(manifest_s.c_str(),std::ios::binary);
    TokenStream<std::ifstream> tss(std::move(ifs));

    tss.analyse();

    auto pkg = get_pkg_name(tss.tokens);

    for(auto& c : pkg) 
    {
        if(c == '.') c = '/';
    }

    auto entry = root;
    entry /= "smali";
    entry /= pkg;
    entry /= "AppEntry.smali";

    auto entry_s = dbg(entry.string());

    std::ifstream ifs2(entry_s.c_str(),std::ios::binary);
    TokenStream<std::ifstream> tss2(std::move(ifs2));

    tss2.analyse();

    if(go_link(tss2.tokens))
    {
        std::ofstream ofs(entry_s.c_str(),std::ios::binary);
        tss2.save(ofs);
    }

    return 0;
}

bool go_link(std::vector<Token>& ts)
{
    int64_t idx;
    if((idx = get_mothed_prologue(ts,"onRequestPermissionsResult",false,"(I[Ljava/lang/String;[I)")) >= 0)
    {
        dbg(idx);
        dbg(ts[idx].to_string());
        ts.insert(ts.begin() + idx,
            Token("invoke-static {p1,p2,p3}, Lane/com/nativelib/QuickSdk;->onRequestPermissionsResult(I[Ljava/lang/String;[I)V",' ','\n'));
        return true;
    }
    return false;
}