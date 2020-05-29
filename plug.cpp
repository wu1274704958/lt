#include <json.hpp>
#include <fileop.hpp>
#include <serialization.hpp>

#include <dbg.hpp>
#include <scomm.hpp>
#include "client/verify.hpp"

namespace fs = std::filesystem;

token::Token gen_insert_token(int, std::string, std::string, std::string);
void copy_other(fs::path& conf_p, fs::path& out, wws::Json& cnf);

int main(int argc, char** argv)
{
    VERIFICATION
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

    copy_other(conf,out,cnf);

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

    std::string ty_name = "L"; ty_name += cnf.get<std::string>("name"); ty_name += ";";
    for (int i = 0; i < ty_name.size(); ++i)
    {
        if (ty_name[i] == '.')
            ty_name[i] = '/';
    }

    int64_t idx_ = get_invoke_mothed(ts.tokens, ty_name, cnf.get<std::string>("method"), "(Landroid/util/SparseArray;ILorg/json/JSONObject;Landroid/app/Activity;)", idx);
    if (dbg(idx_) != -1)
    {
        std::cout << "already call this plug !!" << std::endl;
        return 1;
    }
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
        " sget-object v0, Lane/com/nativelib/ExternalCall;->callbacks:Landroid/util/SparseArray;\n" << 
        " move/from16 v1, p2\n" <<
        " invoke-static {v0, v1, v2, v3}, L" << name << ";" << "->" << method << "(Landroid/util/SparseArray;ILorg/json/JSONObject;Landroid/app/Activity;)V\n" <<
        " :" << tag << '\0';
    ss.flush();
    return token::Token(std::string(ss.str()), '\n', '\n');
}

void copy_other(fs::path& conf_p,fs::path& out, wws::Json& cnf)
{
    if (dbg(cnf.has_key("layout")))
    {
        auto layout = cnf.get<std::string>("layout");
        auto src = fs::absolute(conf_p).parent_path();
        src /= layout;
        auto dst = out / "res/layout";
        dbg(fs::exists(src));
        if (fs::exists(src))
        {
            fs::copy(src,dst,fs::copy_options::overwrite_existing | fs::copy_options::recursive);
        }
    }

    if (dbg(cnf.has_key("drawable")))
    {
        auto draw = cnf.get<std::string>("drawable");
        auto src = fs::absolute(conf_p).parent_path();
        src /= draw;
        auto dst = out / "res/drawable";
        dbg(fs::exists(src));
        if (fs::exists(src))
        {
            fs::copy(src, dst, fs::copy_options::overwrite_existing | fs::copy_options::recursive);
        }
    }
}