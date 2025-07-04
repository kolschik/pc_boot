cmake_minimum_required(VERSION 3.22)

project(nmea C CXX)


include_directories(
    .
    serial/include/serial
    serial/include  
    nmea2k      
#    tiny-aes-c
    )

add_executable(nmea 
#    "tiny-aes-c/aes.c"
    "pc_boot.cpp"
    serial/src/serial.cc
    serial/src/impl/unix.cc
    nmea2k/nmea2k.c
)