#pragma once
#include "boot_api.h"
#include <chrono>


class uart_api : public boot_api{
public:
    uart_api(serial::Serial *s);

    int open();
    int write(uint32_t offset, uint8_t *data, uint32_t l);
    int verify(uint32_t offset, uint8_t *data, uint32_t l);
    int erase(uint32_t start, uint32_t page_cnt, uint32_t page_size);
    int lock(uint8_t lock);
    int detect();
    int start();  
private:
    int send_command(const char *str, uint32_t size, uint32_t timeout = 1000);
    int wait_answer(uint8_t *array, uint32_t timeout = 1000);
};