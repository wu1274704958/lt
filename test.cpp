#include <comm.hpp>
#include <dbg.hpp>
#include <gray.hpp>
#include <fstream>
#include <zlib.h>

namespace wws{

template <typename T>
T read(std::ifstream& in)
{
    T res = 0;
    in.read(reinterpret_cast<char *>(&res),sizeof(T));
    if(wws::big_endian())
    {
        res = wws::reverse_byte(res);
    }
    return res;
}

std::pair<std::unique_ptr<char[]>,size_t> read_arr(std::ifstream& in)
{
	if (in.good())
	{
		constexpr size_t MAX = 1024 * 50;
		size_t max_size = MAX;
		size_t size = 0;
		char *data = new char[MAX];
		while (!in.eof())
		{
			char buf[MAX] = { 0 };
			in.read(buf, wws::arrLen(buf));
			size_t t_s = in.gcount();
			if(t_s <= 0)
				break;
			if(size + t_s > max_size)
			{
				data = reinterpret_cast<char*>(std::realloc(data,max_size + MAX));
				if(data) max_size += MAX;
				else throw std::runtime_error("realloc failed!");
			}
			std::memcpy(data + size,buf,t_s);
			size += t_s;
		}
        return std::make_pair(std::unique_ptr<char[]>(data),size);
    }
    return std::make_pair(nullptr,0);
}

std::pair<std::unique_ptr<char[]>,size_t> uncompress(std::pair<std::unique_ptr<char[]>,size_t>& src)
{
    unsigned long dst_len = 0;
    auto ret = ::uncompress(nullptr,&dst_len,reinterpret_cast<unsigned char*>( src.first.get() ),src.second);
    dbg(ret == Z_BUF_ERROR);
    if(dbg(ret == Z_DATA_ERROR))
        return std::make_pair(nullptr,0); 
    unsigned char * d = new unsigned char[dbg(dst_len)];
    ret = ::uncompress(d,&dst_len,reinterpret_cast<unsigned char*>( src.first.get() ),src.second);
    if(ret == Z_OK)
    {
        return std::make_pair(std::unique_ptr<char[]>(reinterpret_cast<char *>(d)),dst_len);
    }else
        delete [] d;
    return std::make_pair(nullptr,0);
}

}


using namespace wws;

int main()
{

    std::ifstream in("C:\\Users\\admin\\Desktop\\t2\\v2_new.dat",std::ios::binary);
    
    if(in.good())
    {
        auto data = read_arr(in);
        dbg(data.second);
        auto uncompressed = wws::uncompress(data);
        
        printBin(uncompressed.second);
        dbg(uncompressed.second);
    }
    else{
        dbg("open failed!!");
    }

    return 0;
}