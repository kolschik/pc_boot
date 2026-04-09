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
    
    int get_flash(uint32_t *flash, uint32_t *page,  uint32_t *offset){
        *flash = flash_size;
         *page = sector_size;
         *offset = offset_comp;
         return 0;
        }
protected:


    void prepare_print(uint32_t l){   
        point_total = l;
        point = 0;
        point_pos = 0;
        snprintf(buf, sizeof(buf), "[  0%%] ");
        memset(&buf[6], '.', point_cnt);
        buf[point_cnt] = 0;
        printf("%s", buf);
    }

    void point_print(uint32_t writes){
        memset(&buf[6], '.', point_cnt);
        int point_fill = point_cnt  * writes / point_total;
        int pct = 100  * writes / point_total;
        snprintf(buf, sizeof(buf), "[%3d%%] ", pct);
        memset(&buf[6], '=', point_fill);
  
        printf("\r%s", buf);
        fflush(stdout);            
    }

protected:
    static constexpr uint32_t point_cnt = 20;
    char buf[point_cnt + 7];
    uint32_t error_count = 0;
    serial::Serial *s = nullptr;

    uint32_t point_total = 0;
    uint32_t point = 0;
    uint32_t point_pos = 0;
    uint32_t flash_size;
    uint32_t offset_comp = UINT32_MAX;
    uint32_t sector_size;
};