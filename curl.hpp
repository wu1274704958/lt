#pragma once

#include <json.hpp>
#include <tools/thread_pool.h>
#include <filesystem>
#include <dbg.hpp>
#include "FindPath.hpp"
#include <fileop.hpp>


void download(std::string cdn,std::string src_name,std::filesystem::path p)
{
    auto save = p / src_name;
    auto sp = save.parent_path();
    if(!std::filesystem::exists(sp))
        std::filesystem::create_directory(sp);
    std::string cmd("curl ");
    cmd += "-o ";
    cmd += std::filesystem::absolute(save).generic_string();
    cmd += " ";
    cmd += cdn;
    cmd += '/';
    cmd += src_name;
    dbg(cmd);
    system(cmd.c_str());
}

void download(std::string cdn,std::string src_name,std::string dst_name,std::filesystem::path p)
{
    auto save = p / dst_name;
    auto sp = save.parent_path();
    if(!std::filesystem::exists(sp))
        std::filesystem::create_directory(sp);
    std::string cmd("curl ");
    cmd += "-o ";
    cmd += std::filesystem::absolute(save).generic_string();
    cmd += " ";
    cmd += cdn;
    cmd += '/';
    cmd += src_name;
    dbg(cmd);
    system(cmd.c_str());
}