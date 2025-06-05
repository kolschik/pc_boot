#ifndef SPI_API_HEADER
#define SPI_API_HEADER
#include "boot_api.h"
#include "stdint.h"
#include "errno.h"

class spi_api : public boot_api{
public:
    spi_api();

    int open();
    int write(uint32_t offset, uint8_t *data, uint32_t l);
    int verify(uint32_t offset, uint8_t *data, uint32_t l);
    int erase(uint32_t start, uint32_t page_cnt, uint32_t page_size);
    int lock(uint8_t lock);
    int detect();
    int start();  
private:
    int wait_answer(uint32_t *id, uint8_t *array, uint32_t timeout = 1000);

    int bl_connect();
    int bl_get_command(uint8_t *pData);    
    int bl_get_version(uint8_t *ver);
    int bl_get_id(uint16_t *id);
    int bl_write(uint32_t addr, uint8_t *pData, uint16_t len);
    int bl_read(uint32_t addr, uint8_t *pData, uint16_t len);
    void clr_buf();
    static uint8_t xor_checksum(const uint8_t pData[], uint16_t len);    
private:
    uint8_t buf[512];  

    enum class cmd_list : uint8_t{
        GET_CMD_COMMAND = 0x00U,   // Get CMD command
        GET_VER_COMMAND = 0x01U,  // Get Version command
        GET_ID_COMMAND =0x02U,  // Get ID command
        RMEM_COMMAND=0x11U,  // Read Memory command
        GO_COMMAND=0x21U,  // Go command
        WMEM_COMMAND=0x31U,  // Write Memory command
        EMEM_COMMAND=0x44U,  // Erase Memory command
        WP_COMMAND=0x63U,  // Write Protect command
        WU_COMMAND=0x73U,  // Write Unprotect command
        RP_COMMAND=0x82U,  // Readout Protect command
        RU_COMMAND=0x92U // Readout Unprotect command
    };
    static const uint8_t BL_SPI_SOF  = 0x5AU;
    static const uint8_t BL_ACK = 0x79U;
    static const uint8_t BL_NAK = 0x1FU;  

    const uint8_t dummy = 0;    
    const uint8_t sync_byte = BL_SPI_SOF; 
    const uint8_t ack = BL_ACK;
    
private:
    int send_command(cmd_list command);
    int receive_data(uint8_t *pData);
    int send_addr(uint32_t addr);
    int transfer(const uint8_t *inbuf, uint8_t *outbuf, size_t size);
    int wait_for_ack(uint32_t timeout = 50);

    uint32_t HAL_GetTick(){return 0;}
};

#endif