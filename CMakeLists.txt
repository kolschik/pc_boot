cmake_minimum_required(VERSION 3.22)

project(pc_boot C CXX)


include_directories(
    .
    serial/include/serial
    serial/include    
#    tiny-aes-c
    )

add_executable(pc_boot 
#    "tiny-aes-c/aes.c"
    "pc_boot.cpp"
    serial/src/serial.cc
    serial/src/impl/unix.cc
)