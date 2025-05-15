#pragma once

#include "stdint.h"
// get_device --> device_name


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


// write id (4 code 20 address)uint64_t 
// read uint32_t

// erase

