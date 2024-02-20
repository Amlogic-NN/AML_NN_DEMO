#!/bin/bash

copy_files() {
    source_directory="$1"
    target_directory="$2"

    for filepath in "$source_directory"/*; do
        filename=$(basename "$filepath")

        board_name=$(echo "$filename" | cut -d'_' -f2 | tr '[:lower:]' '[:upper:]')
        demo_name=$(echo "$filename" | cut -d'_' -f3- | rev | cut -d'_' -f2- | rev)

        target_subdirectory="$target_directory/$board_name/$demo_name"

        mkdir -p "$target_subdirectory"

        target_filepath="$target_subdirectory/$filename"

        cp "$filepath" "$target_filepath"
		echo "copy: $filepath to $target_filepath"
    done
}


if [ ! -d "./build/" ];then
mkdir ./build
fi

cd build/
rm -rf *

cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm32_toolchain.cmake .. -DARM32=ON

make -j24

mkdir package
find "source" -type f -name "aml_*_32" -exec cp {} "package" \;

copy_files "package" "../../out"
