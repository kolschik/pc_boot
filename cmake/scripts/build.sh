#!/bin/bash

build_dir=build
project_root=$(pwd)
toolchain=$project_root/cmake/toolchains/linux/linux-x86-toolchain.cmake
platform="LINUX-X86"

bold=$(tput bold)
normal=$(tput sgr0)

function show_help() {
    echo "Description:"
    echo "  Build project for selected platform."
    echo
    echo "Usage:"
    echo "  ./$(basename $0) platform"
    echo
    echo "Available platforms:"
    echo "  ap05"
    echo "  ap06"
    echo "  qnx-x86"
    echo "  linux-x86"
    echo "  beaglebone-black"
    echo "  radxa-cm5"
    echo
    echo "Options"
    echo "  -h, --help        show this help message and exit"
    echo "      --no-cache    disable cmake cache, rebuild project from scratch"
}

function clean_build() {
    if [ -d "$build_dir" ]; then
        rm -rf "${build_dir:?}/"*
    else
        mkdir $build_dir
    fi
}

function build() {
    case "${1,,}" in
      ap05)
        platform="AP05"
        source cmake/scripts/qnx632-env.sh
        toolchain=$project_root/cmake/toolchains/qnx/ap05-toolchain.cmake
        ;;
      ap06)
        platform="AP06"
        source cmake/scripts/qnx650-env.sh
        toolchain=$project_root/cmake/toolchains/qnx/ap06-toolchain.cmake
        ;;
      qnx-x86)
        platform="QNX-X86"
        source cmake/scripts/qnx650-env.sh
        toolchain=$project_root/cmake/toolchains/qnx/qnx-x86-toolchain.cmake
        ;;
      linux-x86)
        platform="LINUX-X86"
        toolchain=$project_root/cmake/toolchains/linux/linux-x86-toolchain.cmake
        ;;
      beaglebone-black)
        platform="BEAGLEBONE-BLACK"
        toolchain=$project_root/cmake/toolchains/linux/linux-beaglebone-black-toolchain.cmake
        ;;
      radxa-cm5)
        platform="RADXA-CM5"
        toolchain=$project_root/cmake/toolchains/linux/linux-radxa-cm5-toolchain.cmake
        ;;
      *)
        echo "Unknown platform. Using $platform instead."
        ;;
    esac

    echo "${bold}Building for ${platform}${normal}"
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$toolchain -B $build_dir "$project_root"
    cmake --build $build_dir -j $(nproc)
}

if (( $# == 1 )); then
    build $1
elif (( $# == 2 )); then
    if [ "$2" == "--no-cache" ]; then
        clean_build
    fi
    build $1 $2
else
    show_help
    exit 1
fi
