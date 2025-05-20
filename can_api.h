#pragma once
#include "boot_api.h"
#include <chrono>

typedef enum {
    boot_code_none,
    boot_code_boot,    
    boot_code_name,
    boot_code_lock,
    boot_code_unlock,    
    boot_code_read,
    boot_code_write,
    boot_code_erase,
} boot_code_t;

typedef union {
    struct {
        uint32_t address : 20;
        boot_code_t command : 4;
        uint32_t priority : 5;
        uint32_t not_use : 3;
    }com;
    uint32_t raw;
    /* data */
}boot_id_t;

class can_api : public boot_api{
public:
    can_api(serial::Serial *s);

    int open();
    int write(uint32_t offset, uint8_t *data, uint32_t l);
    int verify(uint32_t offset, uint8_t *data, uint32_t l);
    int erase(uint32_t start, uint32_t page_cnt, uint32_t page_size);
    int lock(uint8_t lock);
    int detect();
    int start();  
private:
    int send_command(const char *str, uint32_t size, uint32_t timeout = 1000);
    int wait_answer(uint32_t *id, uint8_t *array, uint32_t timeout = 1000);    
};