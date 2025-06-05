#ifndef BOOT_API_HEADER
#define BOOT_API_HEADER
#include <stdint.h>
#include <string.h>
#include <stdio.h>

class boot_api {
public:
    //boot_api(serial::Serial *s) : s(s) {}
    virtual int open() = 0;
    virtual int detect() = 0;    
    virtual int write(uint32_t offset, uint8_t *data, uint32_t l) = 0;
    virtual int verify(uint32_t offset, uint8_t *data, uint32_t l) = 0;
    virtual int erase(uint32_t start, uint32_t page_cnt, uint32_t page_size) = 0;
    virtual int lock(uint8_t lock = 0) = 0;
    virtual int start() = 0;    
protected:


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
    static const uint32_t point_cnt = 20;
    char buf[point_cnt + 1];
    uint32_t error_count;
    //serial::Serial *s = nullptr;

    uint32_t point_coast;
    uint32_t point;
    uint32_t point_pos;
};

#endif