#pragma once
#include <stdint.h>
#include "serial.h"

class boot_api {
public:
    boot_api(serial::Serial *s) : s(s) {}
    virtual int open() = 0;
    virtual int detect() = 0;    
    virtual int write(uint32_t offset, uint8_t *data, uint32_t l) = 0;
    virtual int verify(uint32_t offset, uint8_t *data, uint32_t l) = 0;
    virtual int erase(uint32_t start, uint32_t page_cnt, uint32_t page_size) = 0;
    virtual int lock(uint8_t lock = 0) = 0;
    virtual int start() = 0;    
protected:
    virtual int send_command(const char *str, uint32_t size, uint32_t timeout) = 0;

    void prepare_print(uint32_t l){
        point_coast = l / point_cnt;
        point = 0;
        point_pos = 0;
        memset(buf, '.', point_cnt);
        buf[point_cnt] = 0;
        printf("%s", buf);
    }

    void point_print(uint32_t i){
        if (i > point){
            buf[point_pos++] = '=';
            printf("\r%s", buf);
            fflush(stdout);            
            point += point_coast;
        }
    }

protected:
    static constexpr uint32_t point_cnt = 20;
    char buf[point_cnt + 1];
    uint32_t error_count = 0;
    serial::Serial *s = nullptr;

    uint32_t point_coast = 0;
    uint32_t point = 0;
    uint32_t point_pos = 0;
};