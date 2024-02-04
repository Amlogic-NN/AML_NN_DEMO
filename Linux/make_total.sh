#!/bin/bash

copy_files_by_suffix() {
    local source_dir="$1"
    local target_dir="$2"
    local suffix="$3"

    # 检索并拷贝文件
    for file_path in "$source_dir"/*$suffix; do
        if [ -f "$file_path" ]; then
            # 获取文件名和目录名
            file_name=$(basename "$file_path")
            directory_name=${file_name%$suffix}

            # 创建目标目录
            target_directory="$target_dir/$directory_name"
            mkdir -p "$target_directory"

            # 拷贝文件到目标目录
            cp "$file_path" "$target_directory/"
            echo "文件 $file_name 已拷贝到目录 $target_directory"
        fi
    done
}


./make_nnsdk_adla_32.sh
copy_files_by_suffix "build/package" "test_case/adla" "_adla_32"

./make_nnsdk_vsi_32.sh
copy_files_by_suffix "build/package" "test_case/vsi" "_vsi_32"

./make_nnsdk_vsi_64.sh
copy_files_by_suffix "build/package" "test_case/vsi" "_vsi_64"
