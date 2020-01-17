#include "token_stream.hpp"
#include "serialization.hpp"

using namespace token;
using namespace wws;

int main(int argc,char**argv)
{
    //if(argc == 1) return -1;

    std::ifstream f(argv[1],std::ios::binary);

    TokenStream<std::ifstream> t(std::move(f));

    t.analyse(true);

    auto &ts = t.tokens;

    bool in = false;
    for(int i = 0;i < ts.size();++i)
    {
        auto& it = ts[i];

        if(it.body == "CFBundleVersion")
        {
            in = true;
            continue;
        }

        if(in && it.body == "string" && it.back == '>')
        {
            int a = parser<int>(ts[i + 1].body);
            ++a;
            ts[i + 1].body = to_string<int>(a);
            break;
        }
    }
    std::string out(argv[1]);
    t.save(out,true);

    return 0;
}