#include "uart_api.h"
/*
class can_api : public boot_api{
public:
    can_api(serial::Serial *s);
*/
int uart_api::open(){
    return 0;
}

int uart_api::start(){
    char out_buf[64] = {0};
    uint32_t dummy = 0;
    snprintf(out_buf, sizeof(out_buf), "S0000000000000000\r");
    uint32_t size = strlen(out_buf);      

    if (send_command(out_buf, size)) {
        printf("command start not accept \r\n");
        return EINVAL;
    }
 
    uint8_t answer[8];    
    if (wait_answer(answer, 1000) < 0){
        return EINVAL;
    }
    if ((answer[0] != 0)){
        printf("start not accept\r\n");
        return EINVAL;
    }

    return 0;
}

int uart_api::write(uint32_t offset, uint8_t *data, uint32_t l){
    uint32_t error_count = 0;
    prepare_print(l);
    for (uint32_t i=0; i<l;){
        if (error_count >= 5){
            printf("very big error \r\n");
            return EINVAL;
        }

        uint32_t *world;
        world = (uint32_t *)&data[i];

        char out_buf[64] = {0};
        snprintf(out_buf, sizeof(out_buf), "W%08x%08x\r", offset + i, *world);
        uint32_t size = strlen(out_buf);      

        if (send_command(out_buf, size)) {
            printf("write not accept \r\n");
            return EINVAL;
        }
        uint8_t answer[32]; 
        if (wait_answer(answer, 3000) < 0){
            error_count++;
            continue;
        }

        if ((answer[0] != 0)){
            printf("erase not accept\r\n");
            error_count++;
            continue;
        }

        error_count = 0;
      
        i+=4;
        point_print(i);
    }

    return 0;
}



int uart_api::verify(uint32_t offset, uint8_t *data, uint32_t l){
    prepare_print(l);

    uint32_t error_count = 0;
    for (uint32_t i=0; i<l;){
        if (error_count >= 5){
            printf("very big error \r\n");
            return EINVAL;
        }

        char out_buf[64] = {0};
        snprintf(out_buf, sizeof(out_buf), "R%08x00000000\r", offset + i);
        uint32_t size = strlen(out_buf);

        if (send_command(out_buf, size)) {
            printf("read not accept \r\n");
            return EINVAL;
        }
        uint8_t answer[32]; 
        if (wait_answer(answer, 3000) < 0){
            error_count++;
            continue;
        }
  

        uint32_t *d_world, *s_world;

        s_world = (uint32_t *)&data[i];
        d_world = (uint32_t *)&answer;
        if (*d_world != *s_world){
            printf("verify error, offset %x, cpu = %x, file = %x\r\n", offset + i, *d_world, *s_world);
            return EINVAL;
        }

        error_count = 0;
      
        i+=4;
        point_print(i);
    }

    return 0;
}

int uart_api::erase(uint32_t start, uint32_t page_cnt, uint32_t page_size){
    prepare_print(page_cnt*page_size);    
    uint32_t error_count = 0;
    for (uint32_t i=0; i<page_cnt;) {
        if (error_count >= 5){
            printf("very big error \r\n");
            return EINVAL;
        }

        char out_buf[64] = {0};

        snprintf(out_buf, sizeof(out_buf), "E%08x00000000\r", start + page_size * i);
        uint32_t size = strlen(out_buf);      

        if (send_command(out_buf, size)) {
            printf("erase not accept \r\n");
            return EINVAL;
        }

        uint8_t answer[8];    
        if (wait_answer(answer, 3000) < 0){
            error_count++;
            continue;
        }

        if ((answer[0] != 0)){
            printf("erase not accept\r\n");
            error_count++;
            continue;
        }

        point_print(i*page_size);
        i+=1;
        error_count = 0;        
    }

    return 0;
}

int uart_api::lock(uint8_t lock){
    char out_buf[64] = {0};
    uint32_t dummy = 0;
    snprintf(out_buf, sizeof(out_buf), "L000000000000000%d\r", lock);
    uint32_t size = strlen(out_buf);      

    if (send_command(out_buf, size)) {
        printf("command unlock not accept \r\n");
        return EINVAL;
    }
 
    uint8_t answer[8];    
    if (wait_answer(answer, 1000) < 0){
        return EINVAL;
    }
    if ((answer[0] != 0)){
        printf("unlock not accept\r\n");
        return EINVAL;
    }

    return 0;
}

int uart_api::detect(){
    int r = 0;
    while (1){
        char out_buf[64] = {0};
        uint32_t dummy = 0;
        snprintf(out_buf, sizeof(out_buf), "K0000000000000000\r");
        uint32_t size = strlen(out_buf);      

        if (send_command(out_buf, size)) {
            printf("command detect not accept \r\n");
            return EINVAL;
        }

        uint8_t answer[32];
      
        if ((r = wait_answer(answer, 100)) < 0){
            continue;
        }
        if (r != 3){
            continue;
        }
        printf("device detected = %c%c%c\r\n", answer[0], answer[1], answer[2]); 
        break;
    }
    return 0;
}

int uart_api::send_command(const char *str, uint32_t size, uint32_t timeout){
    uint32_t send_byte = s->write((uint8_t *)str, size);

    if (send_byte != size){
        return EFAULT;
    }

    return 0;
}



int uart_api::wait_answer(uint8_t *array, uint32_t timeout){

    uint8_t buf_in_serial_data[256] = {0};

    auto cur_time = std::chrono::system_clock::now();
    auto end_time = cur_time + std::chrono::milliseconds(timeout);

    int rv = -ETIMEDOUT;
    uint32_t count_byte_packet = 0;
    while (cur_time < end_time){ 
        cur_time = std::chrono::system_clock::now();            
        count_byte_packet = s->read(buf_in_serial_data, 256);

        if (count_byte_packet == 0){
            continue;
        }
        rv = 0;
        break;
    }
    if (rv == -ETIMEDOUT){
        return rv;
    }

    rv = -EFAULT;
    switch (count_byte_packet) {
    case 2:  
        if ((buf_in_serial_data[1] == '\r') && ((buf_in_serial_data[0] == 'Y') || (buf_in_serial_data[0] == 'N'))){
            *array = 1;
            if (buf_in_serial_data[0] == 'Y') {
                *array = 0;
            }
            return 1;
        }
        break;

    case 4:
        if (buf_in_serial_data[3] == '\r'){
            memcpy(array, buf_in_serial_data, 3);
            return 3;
        }
        break;
    case 10:
        if ((buf_in_serial_data[9] == '\r') && (buf_in_serial_data[0] == 'R')){
            uint8_t *buf_p = &buf_in_serial_data[1];
            for (int i=3; i>=0; i--){
                char char_id[3] = {0};
                memcpy(char_id, buf_p, 2); 
                buf_p += 2;
                uint32_t buf;
                sscanf(char_id, "%x", &buf);
                array[i] = buf & 0xff;
            }
            return 8;
        }
        break;

    default:
        rv = -EFAULT;
    }

    return rv;
}


uart_api::uart_api(serial::Serial *s) : boot_api(s) {}