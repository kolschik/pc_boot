#pragma once
#include "boot_api.h"
#include <chrono>


class new_can_api : public boot_api{
public:
    new_can_api(serial::Serial *s);

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
    int send(uint32_t id, void *data, int len, bool nead_answer = 1);  
    int read(uint32_t id, void *p, int len=-1);
    int wait_ack(uint32_t id, uint32_t timeout);
    enum class cmd_list : uint8_t{
        GET_CMD_COMMAND = 0x00U,   // Get CMD command
        GET_VER_COMMAND = 0x01U,  // Get Version command
        GET_ID_COMMAND =0x02U,  // Get ID command
        DATA_PAYLOAD =0x04U,  // Get ID command
        RMEM_COMMAND=0x11U,  // Read Memory command
        BL_NAK = 0x1FU,
        GO_COMMAND=0x21U,  // Go command
        WMEM_COMMAND=0x31U,  // Write Memory command
        EMEM_COMMAND=0x43U,  // Erase Memory command
        MAP_COMMAND=0x52U,  // Erase Memory command
        WP_COMMAND=0x63U,  // Write Protect command
        WU_COMMAND=0x73U,  // Write Unprotect command
        BL_ACK = 0x79U,

        RP_COMMAND=0x82U,  // Readout Protect command
        RU_COMMAND=0x92U // Readout Unprotect command
    };
    uint32_t BOOT_ID = 0x01;
};