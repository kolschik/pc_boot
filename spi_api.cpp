#include "spi_api.h"
#include <unistd.h> 

int spi_api::open() {
    return 0;
}

int spi_api::detect() {
    while (1){
        if (bl_connect()) {
            usleep(10000);
            continue;
        }
        uint8_t command_buf[256];
        if (bl_get_command(command_buf)){
            continue;
        }

        uint8_t ver_bl_1 = command_buf[0];
        uint8_t ver_bl_2;
        if (bl_get_version(&ver_bl_2)){
            continue;
        }

        if (ver_bl_1 != ver_bl_2){
            printf("version command and version bl different \r\n");
            return EINVAL;
        }

        uint16_t id;
        if (bl_get_id(&id)){
            continue;
        }

        printf("found mcu id %x \r\n", id);
    }
    return 0;
}


int spi_api::lock(uint8_t lock){
    return 0;
}

int spi_api::erase(uint32_t start, uint32_t page_cnt, uint32_t page_size) {
    int rv;
    // Send start of frame (0x5A) + Erase Memory command frame (0x44 0xBB)
    if ((rv = send_command(cmd_list::EMEM_COMMAND)) != 0) return rv;

    // Send data frame: nb (2 Bytes), the number of pages or sectors to be erased + checksum (1 Byte)
    uint8_t data_frame[3];        
    data_frame[0] = (uint8_t) (page_cnt >> 8) & 0xFFU;
    data_frame[1] = (uint8_t) page_cnt & 0xFFU;
    data_frame[2] = data_frame[0] ^ data_frame[1];
    if ((rv = transfer(data_frame, buf, sizeof(data_frame))) != 0) return rv;    

    uint32_t timeout = page_cnt & 0xff00 ? 50000 : 50;

    if ((page_cnt & 0xf000) == 0){
        if (wait_for_ack()) return EFAULT;
        data_frame[0] = (uint8_t) (start >> 8) & 0xFFU;
        data_frame[1] = (uint8_t) start & 0xFFU;
        data_frame[2] = data_frame[0] ^ data_frame[1];
    }

    // Receive data frame
    if (wait_for_ack(50000)) return EFAULT;

    return 0;
}


int spi_api::write(uint32_t offset, uint8_t *data, uint32_t l){
    uint32_t error_count = 0;
    prepare_print(l);
    uint32_t i = 0;
    for (i=0; i<l;){
        if (error_count >= 5){
            printf("very big error \r\n");
            return EINVAL;
        }
        const uint16_t len_on_iter = 256;
        uint16_t len_send = len_on_iter;
        if ((l - i) < 256) {
            len_send = l-i;
        }
        if (bl_write(offset+i, &data[i], len_send)) {
            error_count++;
            continue;
        }

        error_count = 0;
        i+=len_on_iter;
        point_print(i);
    }

    return 0;
}



int spi_api::verify(uint32_t offset, uint8_t *data, uint32_t l){
    prepare_print(l);
    uint32_t error_count = 0;
    uint32_t i;
    for (i=0; i<l;){
        if (error_count >= 5){
            printf("very big error \r\n");
            return EINVAL;
        }

        const uint16_t len_on_iter = 256;
        uint16_t len_send = len_on_iter;
        if ((l - i) < 256) {
            len_send = l-i;
        }        
        uint8_t read_buf[len_on_iter];
        if (bl_read(offset+i, read_buf, len_send)) {
            error_count++;
            continue;
        }

        if (memcmp(read_buf, &data[i], len_send)) {
            printf("verify error, offset %x \r\n", offset + i);
            return EINVAL;
        }
        error_count = 0;
        i+=len_on_iter;
        point_print(i);
    }
    return 0;
}



int spi_api::start(){
    int rv;
    // Send start of frame (0x5A) + Go command frame (0x21 0xDE)
    if ((rv = send_command(cmd_list::GO_COMMAND)) != 0) return rv;
    if ((rv = send_addr(0x08000000) != 0)) return rv;

    return 0;
}


int spi_api::bl_connect(){
    uint8_t resp;
    uint8_t ack = BL_ACK;
    uint32_t ack_received = 0U;
    int rv;


    // Send synchronization Byte

    if ((rv = transfer(&sync_byte, buf, sizeof(sync_byte))) != 0) return rv;
    if ((rv = transfer(&dummy, buf, sizeof(dummy))) != 0) return rv;
    if ((rv = transfer(&dummy, &resp, sizeof(dummy))) != 0) return rv;

    if(resp == BL_ACK) {
        // Received ACK: send ACK
        if ((rv = transfer(&ack, buf, sizeof(ack))) != 0) return rv;
        ack_received = 1U;
    }

    return ack_received ? 0 : 1;
}

void spi_api::clr_buf(){
    memset(buf, 0, sizeof(buf));
}

int spi_api::bl_get_command(uint8_t *pData){
    int rv;
    // Send start of frame (0x5A) + GET command frame (0x00 0xFF)
    if ((rv = send_command(cmd_list::GET_CMD_COMMAND)) != 0) return rv;
    // Receive data frame
    if ((rv = receive_data(pData)) < 0) return -rv;

    return 0;
}

int spi_api::bl_get_version(uint8_t *ver){
    int rv;

    // Send start of frame (0x5A) + Get Version command frame (0x01 0xEE)
    if ((rv = send_command(cmd_list::GET_VER_COMMAND)) != 0) return rv;

    // Receive data frame
    if ((rv = transfer(&dummy, buf, sizeof(dummy))) != 0) return rv;
    if ((rv = transfer(&dummy,ver, sizeof(dummy))) != 0) return rv;

    // Wait for ACK or NACK frame
    if (wait_for_ack()) return EFAULT;

    return 0;
}


int spi_api::bl_get_id(uint16_t *id) {
    int rv;
    // Send start of frame (0x5A) + Get ID command frame (0x02 0xFD)
    if ((rv = send_command(cmd_list::GET_ID_COMMAND)) != 0) return rv;

    uint8_t rx_buf[512];
    if ((rv = receive_data(rx_buf)) < 0) return -rv;

    if(rv != 2) {
        return EINVAL;
    }

    *id = (rx_buf[0] << 8) | rx_buf[1];
    return 0;
}

int spi_api::send_addr(uint32_t addr){
    uint8_t addr_frame[5];
    addr_frame[0] = (addr >> 24) & 0xFFU;
    addr_frame[1] = (addr >> 16) & 0xFFU;
    addr_frame[2] = (addr >> 8) & 0xFFU;
    addr_frame[3] = addr & 0xFFU;

    addr_frame[4] = xor_checksum(addr_frame, sizeof(addr));

    if (transfer(addr_frame, buf, sizeof(addr_frame))) return EFAULT;
    if (wait_for_ack()) return EFAULT;  
    return 0;  
}


int spi_api::bl_write(uint32_t addr, uint8_t *pData, uint16_t len){
    int rv;

    if ((len == 0) || (len > 256)) {
        return EINVAL;
    }

    // Send start of frame (0x5A) + wmem command frame (0x02 0xFD)
    if ((rv = send_command(cmd_list::WMEM_COMMAND)) != 0) return rv;
    if ((rv = send_addr(addr) != 0)) return rv;

    uint8_t data_frame[257];
    data_frame[0] = len - 1;
    memcpy(&data_frame[1], pData, len);

    if ((rv = transfer(data_frame, buf, len)) != 0) return rv;
    if (wait_for_ack()) return EFAULT;

    return 0;
}


int spi_api::bl_read(uint32_t addr, uint8_t *pData, uint16_t len){
    int rv;

    if ((len == 0) || (len > 256)) {
        return EINVAL;
    }

    // Send start of frame (0x5A) + Read Memory command frame (0x11 0xEE)
    if ((rv = send_command(cmd_list::RMEM_COMMAND)) != 0) return rv;
    if ((rv = send_addr(addr) != 0)) return rv;

    // Send data frame: number of Bytes to be read (1 Byte) + checksum (1 Byte)
    uint8_t nob_frame[2];
    nob_frame[0] = len - 1U;
    nob_frame[1] = nob_frame[0] ^ 0xFFU;
    if ((rv = transfer(nob_frame, buf, sizeof(nob_frame))) != 0) return rv;
    if (wait_for_ack()) return EFAULT;

    if ((rv = transfer(&dummy, buf, sizeof(dummy))) != 0) return rv;
    clr_buf();
    if ((rv = transfer(buf, pData, len)) != 0) return rv;

    return 0;
}

int spi_api::receive_data(uint8_t *pData){
    int rv;
    uint8_t rx_number_of_bytes;
    if ((rv = transfer(&dummy, buf, sizeof(dummy))) != 0) return -rv;
    if ((rv = transfer(&dummy, &rx_number_of_bytes, sizeof(dummy))) != 0) return -rv;
    clr_buf();
    if ((rv = transfer(buf, pData, 1U + (uint16_t)rx_number_of_bytes)) != 0) return -rv;

    if (wait_for_ack()) return -EFAULT;
    
    return 1U + (uint16_t)rx_number_of_bytes;
}

int spi_api::send_command(cmd_list command){
    uint8_t cmd_frame[3];
    cmd_frame[0] = BL_SPI_SOF;
    cmd_frame[1] = static_cast<uint8_t>(command);
    cmd_frame[2] = cmd_frame[1] ^ 0xFFU; // Command XOR checksum
    int rv;

    if ((rv = transfer(cmd_frame, buf, sizeof(cmd_frame))) != 0) return rv;

    // Wait for ACK or NACK frame
    if (wait_for_ack()) return EFAULT;

    return 0;
}


int spi_api::wait_for_ack(uint32_t timeout) {
    int rv;

    if ((rv = transfer(&dummy, buf, sizeof(dummy))) != 0) return rv;

    while(1) {
        uint8_t resp;
        if ((rv = transfer(&dummy, &resp, sizeof(resp))) != 0) return rv;  
        rv  = 0;

        if (timeout-- == 0){
            return ETIMEDOUT;
        }

        if(resp == BL_ACK) {   
            rv = 0;
            break;
        } else if (resp == BL_NAK) {
            rv = ECANCELED;
            break;
        }
        usleep(1000);
    }

    if (transfer(&ack, buf, sizeof(ack)) != 0) return EFAULT;
    return rv;
}

uint8_t spi_api::xor_checksum(const uint8_t pData[], uint16_t len) {
    uint8_t sum = 0;

    for (uint16_t i = 0; i < len; i++) {
        sum ^= pData[i];
    }

    return sum;
}

int spi_api::transfer(const uint8_t *inbuf, uint8_t *outbuf, size_t size){
    return 0;
}

spi_api::spi_api() : 
    dummy (0),    
    sync_byte(BL_SPI_SOF),
    ack(BL_ACK)
{}