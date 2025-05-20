#include "can_api.h"


int can_api::open() {
    const char s_Open[3] = {"O\r"};

    if (send_command(s_Open, sizeof(s_Open) - 1)) {
        return EINVAL;
    }

    return 0;
}

int can_api::detect() {
    while (1){
        boot_id_t id = {0};
        id.com.address = 0;
        id.com.command = boot_code_name;

        char out_buf[64] = {0};
        uint32_t dummy = 0;
        snprintf(out_buf, sizeof(out_buf), "T%08x8%08x%08x\r", id.raw, dummy, dummy);
        uint32_t size = strlen(out_buf);      

        if (send_command(out_buf, size)) {
            printf("command detect not accept \r\n");
            return EINVAL;
        }
        uint8_t answer[8];
        boot_id_t id_rcv = {0};  
        int rv;      
        if ((rv = wait_answer(&id_rcv.raw, answer, 100)) < 0){
            continue;
        }
        if ((id_rcv.raw != id.raw) || (rv != 3)){
            continue;
        }
        printf("device detected = %c%c%c\r\n", answer[0], answer[1], answer[2]); 
        break;
    }
    return 0;
}


int can_api::send_command(const char *str, uint32_t size, uint32_t timeout){

    uint32_t send_byte = s->write((uint8_t *)str, size);

    if (send_byte != size){
        return EFAULT;
    }

    uint8_t buf_in_serial_data[2048] = {0};

    auto cur_time = std::chrono::system_clock::now();
    auto t_stop = cur_time + std::chrono::milliseconds(timeout);
    auto end_time = t_stop;
    int rv = ETIMEDOUT;
    while (cur_time < end_time){ 
        cur_time = std::chrono::system_clock::now();            
        uint32_t count_byte_packet = s->read(buf_in_serial_data, 2048);

        if ((count_byte_packet == 1) && (buf_in_serial_data[0] == 0x0d)){
            return 0;
        }
    }

    return ETIMEDOUT;
}

int can_api::wait_answer(uint32_t *id, uint8_t *array, uint32_t timeout){

    uint8_t buf_in_serial_data[256] = {0};

    auto cur_time = std::chrono::system_clock::now();
    auto t_stop = cur_time + std::chrono::milliseconds(timeout);
    auto end_time = t_stop;
    int rv = -ETIMEDOUT;
    uint32_t count_byte_packet = 0;
    while (cur_time < end_time){ 
        cur_time = std::chrono::system_clock::now();            
        count_byte_packet = s->read(buf_in_serial_data, sizeof(buf_in_serial_data));

        if (count_byte_packet == 0){
            continue;
        }

        if (*buf_in_serial_data == 'T'){
            rv = 0;
            break;
        }
    }

    if ((rv == 0) && (*buf_in_serial_data == 'T') && (count_byte_packet >= 10)){
        char char_id[9] = {0};
        memcpy(char_id, &buf_in_serial_data[1], 8);
        sscanf(char_id, "%x", id);

        uint8_t num = buf_in_serial_data[9];
        if ((num < '1') || (num > '9')){
            return -EINVAL;   
        }
        num = num - '0';

        uint8_t *buf_p = &buf_in_serial_data[10];
        for (int i=num-1; i>=0; i--){
            memcpy(char_id, buf_p, 2); 
            buf_p += 2;
            char_id[2] = 0;
            uint32_t buf;
            sscanf(char_id, "%x", &buf);
            array[i] = buf & 0xff;
        }  
        return num;
    }

    return rv;
}

int can_api::lock(uint8_t lock){
        boot_id_t id = {0};
        id.com.address = 0;

        id.com.command = boot_code_lock;
        if (lock == 1){
            id.com.command = boot_code_unlock;
        }

        char out_buf[64] = {0};
        uint32_t dummy = 0;
        snprintf(out_buf, sizeof(out_buf), "T%08x8%08x%08x\r", id.raw, dummy, dummy);
        uint32_t size = strlen(out_buf);      

        if (send_command(out_buf, size)) {
            printf("command unlock not accept \r\n");
            return EINVAL;
        }
        uint8_t answer[8];
        boot_id_t id_rcv = {0};
        int rv;
        if ((rv = wait_answer(&id_rcv.raw, answer, 1000)) < 0){
            printf("command unlock timeout \r\n");
            return EINVAL;
        }
        if ((id_rcv.raw != id.raw) || (answer[0] != 0)){
            printf("unlock = %x, pld = %x \r\n", id.raw, answer[0]);
            return EINVAL;
        }
        return 0;
}



int can_api::erase(uint32_t start, uint32_t page_cnt, uint32_t page_size) {

    start = start - 0x08000000;
    uint32_t error_count = 0;

    for (uint32_t i=0; i<page_cnt;){
        if (error_count >= 5){
            printf("very big error \r\n");
            return EINVAL;
        }
        boot_id_t id = {0};
        id.com.address = start+i*page_size;
        id.com.command = boot_code_erase;

        char out_buf[64] = {0};
        //T 000050b8 8 0800 6c5d 0800 6c5d
        uint32_t dummy = 0;
        snprintf(out_buf, sizeof(out_buf), "T%08x8%08x%08x\r", id.raw, dummy, dummy);
        uint32_t size = strlen(out_buf);      

        if (send_command(out_buf, size)) {
            printf("command not accept \r\n");
            return EINVAL;
        }

        uint8_t answer[8]; 
        boot_id_t id_rcv = {0};
        if (wait_answer(&id_rcv.raw, answer, 1000) < 0){
            error_count++;
            continue;
        }
        if ((id_rcv.raw != id.raw) || (answer[0] != 0)){
            printf("erase = %x, pld = %x \r\n", id.raw, answer[0]);
            error_count++;
            continue;
        }

        i+=1;
    }
    return 0;
}


int can_api::write(uint32_t offset, uint8_t *data, uint32_t l){
    return 0;
}
int can_api::verify(uint32_t offset, uint8_t *data, uint32_t l){
    return 0;
}
int can_api::start(){
    return 0;
}

can_api::can_api(serial::Serial *s) : boot_api(s) {}