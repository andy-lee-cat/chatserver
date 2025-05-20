import os
import sys

# 支持的文件后缀
VALID_EXTS = {'.cpp', '.hpp', '.txt'}

def count_lines_in_file(file_path):
    count = 0
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            for line in f:
                if line.strip():  # 非空行才计数
                    count += 1
    except Exception as e:
        print(f"Error reading {file_path}: {e}")
    return count

def count_lines_in_dir(root_dir):
    total_lines = 0
    file_count = 0
    for dirpath, _, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename == 'json.hpp':
                continue
            ext = os.path.splitext(filename)[1]
            if ext.lower() in VALID_EXTS:
                filepath = os.path.join(dirpath, filename)
                lines = count_lines_in_file(filepath)
                total_lines += lines
                file_count += 1
    return total_lines, file_count

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("用法: python count_code_lines.py <目录路径>")
        sys.exit(1)

    directory = sys.argv[1]
    if not os.path.isdir(directory):
        print(f"无效目录: {directory}")
        sys.exit(1)

    total, files = count_lines_in_dir(directory)
    print(f"共处理文件数: {files}")
    print(f"总非空代码行数: {total}")
