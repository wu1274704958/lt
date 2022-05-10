#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <optional>
#include "FindPath.hpp"
#include <format>
#include <regex>
#include <algorithm>
#include <serialization.hpp>

namespace fs = std::filesystem;

std::vector<std::string> stringSplit(const std::string& str, char delim);

class Dir
{
public:
	std::unordered_map<std::string,std::vector<fs::path>> map;
	fs::path root;

	fs::path GetRelativeToRoot(const fs::path& p) const 
	{
		return fs::relative(p,root);
	}

	std::optional<std::vector<const std::vector<fs::path>*>> find(const std::string& key,bool allLower = false) const
	{
		std::vector<const std::vector<fs::path>*> res;
		for(auto& paths : map)
		{
			if (allLower)
			{
				std::string low_src = paths.first;
				std::transform(paths.first.begin(),paths.first.end(),low_src.begin(),::tolower);
				std::string low_2 = key;
				std::transform(key.begin(), key.end(), low_2.begin(), ::tolower);
				if(low_src.find(low_2) != -1)
					res.push_back(&paths.second);
			}else
			if(paths.first.find(key) != -1)
			{
				res.push_back(&paths.second);
			}
		}
		if(res.empty()) return std::nullopt;
		return std::make_optional( std::move(res));
	}

	std::optional<std::vector<const std::vector<fs::path>*>> find_with_regex(const std::string& key, bool allLower = false) const
	{
		std::vector<const std::vector<fs::path>*> res;
		for (auto& paths : map)
		{
			try
			{
				std::regex reg(key, allLower ? std::regex::icase | std::regex::ECMAScript | std::regex_constants::collate : 
					std::regex::ECMAScript | std::regex_constants::collate);
			
				if (std::regex_search(paths.first.begin(),paths.first.end(),reg))
				{
					res.push_back(&paths.second);
				}
			}
			catch (...) { return std::nullopt;}
		}
		if (res.empty()) return std::nullopt;
		return std::make_optional(std::move(res));
	}

	const fs::path* find(const std::string& key,const fs::path& relativePath) const
	{
		if(map.contains(key))
		{
			const auto& it = map.find(key);
			for(const auto& p : it->second)
			{
				if(GetRelativeToRoot(p) == relativePath)
					return &p;
			}
		}
		return nullptr;
	}

	static std:: optional<Dir> Load(const char* root)
	{
		fs::path r(root);
		if(fs::exists(r) && fs::is_directory(r) )
		{
			std::unordered_map<std::string, std::vector<fs::path>> map;
			wws::enum_path(r,[&map](const fs::path& path)
			{
				auto name = path.filename().string();
				auto it = map.find(name);
				if(it != map.end())
				{
					it->second.push_back(path);
				}else
				{
					map.insert(std::make_pair( name,std::vector<fs::path>{ path }));
				}
			});
			Dir res;
			res.map = std::move(map);
			res.root = std::move(r);
			return std::make_optional(std::move(res));
		}
		return std::nullopt;
	}
};
int calcNum(const std::optional<std::vector<const std::vector<fs::path>*>>& _1)
{
	int res = 0;
	for (auto it : _1.value())
	{
		res += it->size();
	}
	return  res;
}
const std::vector<const std::vector<fs::path>*>& findMore(const std::optional<std::vector<const std::vector<fs::path> *>>& _1,
	const std::optional<std::vector<const std::vector<fs::path>*>>& _2,int &from)
{
	if(!_1)
	{
		from = 2;
		return _2.value();
	}
	if(!_2) 
	{
		from = 1;
		return _1.value();
	}
	int n1 = calcNum(_1);
	int n2 = calcNum(_2);
	from = n1 > n2 ? 1 : 2;
	return n1 > n2 ? _1.value() : _2.value();
}

void showSelect(const Dir& d1, const Dir& d2, const std::vector<const std::vector<fs::path>*>& sel, int from,int num,int &needRefresh,int rowNum = 10);
const std::vector<const fs::path*> scanf_arr(const std::vector<const fs::path*>& paths);

int main(int argc,char **argv)
{
	if(argc < 3) 
		return -1;

	auto d1 = Dir::Load(argv[1]);
	auto d2 = Dir::Load(argv[2]);
	int needRefresh = 0, mode = 1;

	if(!d1 || !d2) 
		return -2;
	while (true)
	{
		if((needRefresh & 1) == 1)
			d1 = Dir::Load(argv[1]);
		if ((needRefresh & 2) == 2)
			d2 = Dir::Load(argv[2]);
		needRefresh = 0;
		std::string input;
		std::getline(std::cin,input);
		if(input.empty())
			continue;
		std::vector<std::string> arr = stringSplit(input, ' ');
		if(arr.size() == 1 && arr[0] == "exit" )
			break;
		if(arr.size() == 1 && arr[0] == "refresh")
		{
			needRefresh = 1 | 2;
			continue;
		}
		if(arr.size() == 2 && arr[0] == "mode")
		{
			try
			{
				mode = wws::parser<int>(arr[1]);
				std::cout << "set success!!!" << std::endl;
			}
			catch (...){}
			continue;
		}
		int from = 0;
		std::optional<std::vector<const std::vector<fs::path>*>> d1_res = std::nullopt;
		
		
		std::optional<std::vector<const std::vector<fs::path>*>> d2_res = std::nullopt; 
		if ((mode & 2) == 2)
		{
			d1_res = d1->find_with_regex(input, (mode & 1) == 1);
			d2_res = d2->find_with_regex(input, (mode & 1) == 1);
		}else
		{
			d1_res = d1->find(input, (mode & 1) == 1);
			d2_res = d2->find(input, (mode & 1) == 1);
		}
		if(!d1_res && !d2_res)
		{
			std::cout << "Not Found!!"<<std::endl;
			continue;
		}
		const std::vector<const std::vector<fs::path>*>& sel = findMore(d1_res,d2_res,from);
		showSelect(d1.value(),d2.value(),sel, from, calcNum(sel),needRefresh);
	}
	
	
	return 0;
}

void Copy(const Dir& d1, const Dir& d2, const fs::path& sel, int from, int& needRefresh);
void showSelect(const Dir& d1, const Dir& d2, const std::vector<const std::vector<fs::path>*>& sel,int from,int num, int& needRefresh,
	int rowNum)
{
	int pageIdx = 0,rowIdx = 0,arrIdx = 0,pathIdx = 0,realIdx = 0;
	const Dir& sel_dir = from == 1 ? d1 : d2;
	std::cout << std::format("page{}:\n",pageIdx);
	std::vector<const fs::path*> tmp;
	for(auto arr : sel)
	{
		for(auto& path : *arr)
		{
			tmp.push_back(&path);
			auto relat = sel_dir.GetRelativeToRoot(path);
			std::cout << std::format("{:^3} {}\n",rowIdx,relat.string());

			++rowIdx;
			++pathIdx;
			++realIdx;
			
			if(rowIdx >= rowNum || realIdx >= num)
			{
				auto sel_arr = scanf_arr(tmp);
				for(auto p : sel_arr)
				{
					Copy(d1,d2,*p,from,needRefresh);
				}
				if(realIdx >= num) break;
				tmp.clear();
				rowIdx = 0;
				++pageIdx;
				std::cout << std::format("page{}:\n", pageIdx);
			}
		}
		++arrIdx;
		pathIdx = 0;
	}
	std::cout << "end" << std::endl;
}

std::vector<std::string> stringSplit(const std::string& str, char delim) {
	std::string s;
	s.append(1, delim);
	std::regex reg(s);
	std::vector<std::string> elems(std::sregex_token_iterator(str.begin(), str.end(), reg, -1),
		std::sregex_token_iterator());
	return elems;
}

const std::vector<const fs::path*> scanf_arr(const std::vector<const fs::path*>& paths)
{
	std::vector<const fs::path*> res;
	std::string in;
	std::getline(std::cin,in);
	if(in.empty())
		return  res;
	std::vector<std::string> ss = stringSplit(in,' ');
	if(ss.size() >= 1 && ss[0] == "*")
		return paths;
	for(auto& s : ss)
	{
		try{
			int v = std::stoi(s.c_str());
			if(v >= 0 && v < paths.size())
				res.push_back(paths[v]);
		}catch (...)
		{}
	}
	return  res;
}

void Copy(const Dir& d1, const Dir& d2,const fs::path& sel, int from , int& needRefresh)
{
	const Dir& sel_dir = from == 1 ? d1 : d2;
	const Dir& oth = from == 1 ? d2 : d1;

	auto relative = sel_dir.GetRelativeToRoot(sel);
	auto name = sel.filename().string();
	auto oth_path = oth.find(name,relative);
	std::string src,dst;
	if(!oth_path)
	{
		auto to = oth.root;
		to /= relative;
		fs::copy_file(sel,to, fs::copy_options::overwrite_existing);
		src = sel.string();dst = to.string();
		std::cout << std::format("{} -> \n{}\n",src.c_str(),dst.c_str());
		needRefresh = from == 1 ? 2 : 1;
	}else
	{
		auto t1 = fs::last_write_time(sel);
		auto t2 = fs::last_write_time(*oth_path);
		if(t1 > t2)
		{
			fs::copy_file(sel,*oth_path,fs::copy_options::overwrite_existing);
			src = sel.string(); dst = oth_path->string();
			std::cout << std::format("{} -> \n{}\n", src.c_str(), dst.c_str());
		}else
		{
			fs::copy_file(*oth_path, sel, fs::copy_options::overwrite_existing);
			dst = sel.string(); src = oth_path->string();
			std::cout << std::format("{} -> \n{}\n", src.c_str(), dst.c_str());
		}
	}

}