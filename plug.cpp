#include <json.hpp>
#include <fileop.hpp>
#include <serialization.hpp>

#include <dbg.hpp>
#include <scomm.hpp>

namespace fs = std::filesystem;

token::Token gen_insert_token(int, std::string, std::string, std::string);

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cout << "bad args!!! out_pkg config " << std::endl;
        return -1;
    }

    fs::path out(argv[1]);
    fs::path conf(argv[2]);

    if (!fs::exists(out) || !fs::exists(conf))
    {
        std::cout << "out pkg or config is not exist! " << std::endl;
        return -1;
    }

    auto conf_str = wws::read_from_file<1024>(conf, std::ios::binary);
    if (!conf_str)
    {
        std::cout << "read config failed! " << std::endl;
        return -1;
    }
   
    wws::Json cnf(conf_str.value());

    auto cmd_code = cnf.get<int>("cmd_code");
    
    std::string cmd("0x");
    cmd += to_hex(cmd_code);
    dbg(cmd);

    auto smali_dir = fs::absolute(conf).parent_path();
    smali_dir /= cnf.get<std::string>("smali");

    auto dst = out / "smali";

    fs::copy(smali_dir, dst, fs::copy_options::overwrite_existing | fs::copy_options::recursive);

    fs::path extern_call = dst / "ane";
    extern_call /= "com"; extern_call /= "nativelib"; extern_call /= "ExternalCall.smali";

    if(!fs::exists(extern_call))
    {
        std::cout << "ExternalCall.smali not found!  " << std::endl;
        return -1;
    }
    auto extern_call_p = extern_call.generic_string();
    std::ifstream ifs(extern_call_p, std::ios::binary);

    token::TokenStream<std::ifstream> ts(std::move(ifs));

    ts.analyse();

    auto [count,b] = get_tag_max(ts.tokens, "call", false, "(IILorg/json/JSONObject;Landroid/webkit/ValueCallback;)", "cond");

    dbg(count);
    dbg(b);

    auto tok = gen_insert_token(count + 1, cnf.get<std::string>("name"), cnf.get<std::string>("method"), cmd);

    int64_t idx  = get_invoke_mothed(ts.tokens, "Landroid/util/SparseArray;", "put", "(ILjava/lang/Object;)", b);
    dbg(idx);

    while (ts.tokens[idx].back != '\n') { ++idx; }

    ts.tokens.insert(ts.tokens.begin() + (idx + 1), std::move(tok));

    ts.save(extern_call_p, true);

    return 0;
}

token::Token gen_insert_token(int count, std::string name , std::string method, std::string cmd)
{
    std::string tag = "cond_";
    tag += to_hex(count);

    for (int i = 0; i < name.size(); ++i)
    {
        if (name[i] == '.')
            name[i] = '/';
    }

    std::strstream ss;

    ss << 
        " const/16 v4," << cmd << "\n" <<
        " move/from16 v5, p1\n" <<
        " if-ne v4, v5, :" << tag << "\n" <<
        " move-object/from16 v2, p3\n" <<
        " sget-object v3, Lane/com/nativelib/ExternalCall;->context:Landroid/app/Activity;\n" <<
        " invoke-static {v0, v1, v2, v3}, L" << name << ";" << "->" << method << "(Landroid/util/SparseArray;ILorg/json/JSONObject;Landroid/app/Activity;)V\n" <<
        " :" << tag << '\0';
    ss.flush();
    return token::Token(std::string(ss.str()), '\n', '\n');
}