#pragma once
#include "stdint.h"

typedef union {
    struct {
        uint32_t res : 1;
        uint32_t rtr : 1;
        uint32_t ide : 1; 
        uint32_t res2 : 5; 
        uint32_t dlc : 8;
        uint32_t fifo : 2;
        uint32_t res3 : 14;         
    };
    uint32_t flg; 
}can_pkt_flg_t;

__attribute__((aligned(4)))typedef struct {
    uint32_t id;
    can_pkt_flg_t flg;
    union
    {
        uint32_t data32[2];
        uint64_t data64;
        uint8_t data8[8];        
    };
}can_fifo_t;
