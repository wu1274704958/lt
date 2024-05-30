import os
import json
import sys
import subprocess

# 配置文件路径
config_file = os.path.join(os.path.dirname(__file__), 'file_mappings.json')

def load_mappings():
    if os.path.exists(config_file):
        with open(config_file, 'r') as f:
            return json.load(f)
    return {}

def save_mappings(mappings):
    with open(config_file, 'w') as f:
        json.dump(mappings, f, indent=4)

def try_get_mappinged_b_path(conf,path_a:str):
    for k , v in conf.items():
        if path_a.startswith(k):
            return v,k
    return None

def common_suffix(str1:str, str2:str):
    # 确保两个字符串都不为空
    if not str1 or not str2:
        return None

    # 找到最小长度
    lena = len(str1)
    lenb = len(str2)
    min_len = min(lena, lenb)
    # 遍历两个字符串
    for i in range(min_len):
        if str1[lena - 1 - i] != str2[lenb - 1 - i]:
            # 如果找到第一个不匹配的字符，返回之前的部分
            if i == 0:
                return None
            return i
    # 如果两个字符串都相同，或者其中一个字符串是另一个字符串的前缀，返回其中较短的字符串
    return min_len

def remove_filename(path:str):
    idx = path.rfind('\\')
    if idx == None:
        return path
    return path[:idx]

def main(file_a, file_b=None):
    mappings = load_mappings()
    
    if file_b != None:
        idx = common_suffix(file_a, file_b)
        if idx != None:
            k = file_a[:len(file_a) - idx]
            v = file_b[:len(file_b) - idx]
            if len(k) > 0 and len(v) > 0:
                mappings[k] = v
                save_mappings(mappings)
                print(f"Mapping saved: {k} -> {v}")
    else:
        # 尝试找到与 file_a 匹配的记录
        file_b,key = try_get_mappinged_b_path(mappings,file_a)
        if not file_b:
            print(f"No previous record found for '{file_a}'. Please provide FileB path.")
            sys.exit(1)
        else:
            file_b = file_b + file_a[len(key):]
            print(f"find file b path '{file_b}'")
    
    # 调用 bcomp 比较文件
    
    subprocess.Popen(['bcomp', file_a, file_b])

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python xxx.py <fileA> [<fileB>]")
        sys.exit(1)

    file_a = sys.argv[1]
    file_b = sys.argv[2] if len(sys.argv) > 2 else None

    main(file_a, file_b)
