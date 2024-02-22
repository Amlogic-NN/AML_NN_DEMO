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

<<<<<<< HEAD:Linux/scripts/make_nnsdk_32.sh
cmake -DCMAKE_TOOLCHAIN_FILE=arm32_toolchain.cmake .. -DARM32=ON
=======
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm32_toolchain.cmake .. -DARM32=ON
>>>>>>> 9ffa7e3676e67b32f78835561f2ee3f698007d50:Linux/make_total.sh

make -j24

mkdir package
find "source" -type f -name "aml_*_32" -exec cp {} "package" \;

copy_files "package" "../../out"
