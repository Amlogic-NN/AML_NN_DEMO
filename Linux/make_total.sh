#!/bin/bash

copy_files() {
    source_directory="$1"
    target_directory="$2"

    # 遍历源目录下的文件
    for filepath in "$source_directory"/*; do
        filename=$(basename "$filepath")

        # 使用cut命令进行字符串切割
        board_name=$(echo "$filename" | cut -d'_' -f2 | tr '[:lower:]' '[:upper:]')
        demo_name=$(echo "$filename" | cut -d'_' -f3- | rev | cut -d'_' -f2- | rev)

        # 构建目标目录路径
        target_subdirectory="$target_directory/$board_name/$demo_name"

        # 创建目标目录（如果不存在）
        mkdir -p "$target_subdirectory"

        # 构建目标文件路径
        target_filepath="$target_subdirectory/$filename"

        # 拷贝文件到目标目录
        cp "$filepath" "$target_filepath"
		echo "已拷贝: $filepath 到 $target_filepath"
    done
}



./make_nnsdk_32.sh
copy_files "build/package" "test_case"

./make_nnsdk_64.sh
copy_files "build/package" "test_case"