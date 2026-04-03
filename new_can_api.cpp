#include "new_can_api.h"


int new_can_api::open() {
    const char s_Open[3] = {"O\r"};

    if (send_command(s_Open, sizeof(s_Open) - 1)) {
        return EINVAL;
    }

    return 0;
}

/*
    while (1){
        if (bl_connect()) {
#if defined QNX            
            delay(10);
#endif            
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
*/
int new_can_api::detect() {
    uint32_t iter=0;
    while (1){
        char buf[256];
        iter ^= 1;
        if (iter){
        printf("x");
        }else {
        printf("+");
        }

        fflush(stdout);


        if(send(static_cast<uint32_t>(cmd_list::GET_CMD_COMMAND), buf, 0)) {
            continue;
        }

/*
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
        if ((id_rcv.raw != id.raw) || (rv != 8)){
            continue;
        }
        flash_size = answer[6] * 1024;
        sector_size = answer[4] | ((uint16_t) answer[5] << 8);
        printf("device detected = %c%c%c, flash size = %d bytes, sector_size = %d b\r\n", answer[0], 
            answer[1], answer[2], flash_size, sector_size); 
        printf(" = %c%c%c\r\n", answer[0], answer[1], answer[2]); 
        break;
        */
    }
    return 0;
}


int new_can_api::send_command(const char *str, uint32_t size, uint32_t timeout){

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

int new_can_api::wait_answer(uint32_t *id, uint8_t *array, uint32_t timeout){

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
        if ((num < '1') || (num > '8')){
            return -EINVAL;   
        }
        num = num - '0';

        uint8_t *buf_p = &buf_in_serial_data[10];
        for (int i=0; i<num; i++){
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

int new_can_api::lock(uint8_t lock){
    return 0;
}



int new_can_api::erase(uint32_t offset, uint32_t page_cnt, uint32_t page_size) {

    uint8_t data_buf[6];
    data_buf[0] = (offset >> 24) & 0xff;
    data_buf[1] = (offset >> 16) & 0xff;
    data_buf[2] = (offset >> 8) & 0xff;
    data_buf[3] = (offset >> 0) & 0xff;

    data_buf[4] = (page_cnt >> 8) & 0xff;
    data_buf[5] = (page_cnt >> 0) & 0xff;

    if (send(static_cast<uint32_t>(cmd_list::EMEM_COMMAND), data_buf, sizeof(data_buf))) {
        printf("erase not accept \r\n");
        return EINVAL;
    }

    // TODO wait Ack

    return 0;
}


int new_can_api::write(uint32_t offset, uint8_t *data, uint32_t l){
    offset = offset - 0x08000000;
    uint32_t error_count = 0;
   
    prepare_print(l);    
    for (uint32_t i=0; i<l; ){
        if (error_count >= 5){
            printf("very big error \r\n");
            return EINVAL;
        }

    
        int chank_size = 256;
        if ((l-i) < chank_size) {
            chank_size = l-i;
        }
        uint8_t data_buf[5];
        data_buf[0] = chank_size;
        data_buf[1] = (offset >> 24) & 0xff;
        data_buf[2] = (offset >> 16) & 0xff;
        data_buf[3] = (offset >> 8) & 0xff;
        data_buf[4] = (offset >> 0) & 0xff;
      
        if (send(static_cast<uint32_t>(cmd_list::WMEM_COMMAND)  , data_buf, sizeof(data_buf))) {
            printf("read not accept \r\n");
            return EINVAL;
        }

        uint8_t chank_buf[256];
        memcpy(chank_buf, data, chank_size);
    
        if (send(static_cast<uint32_t>(cmd_list::DATA_PAYLOAD), data_buf, chank_size)) {
            error_count++;
            continue;
        }
        error_count = 0;
      
        // TODO Wait ack
        i+=chank_size;
        point_print(i);
    }
    return 0;
}

int new_can_api::verify(uint32_t offset, uint8_t *data, uint32_t l) {
    prepare_print(l);

    uint32_t error_count = 0;
    for (uint32_t i=0; i<l;){
        if (error_count >= 5){
            printf("very big error \r\n");
            return EINVAL;
        }
        int chank_size = 256;
        if ((l-i) < chank_size) {
            chank_size = l-i;
        }
        uint8_t data_buf[5];
        data_buf[0] = chank_size;
        data_buf[1] = (offset >> 24) & 0xff;
        data_buf[2] = (offset >> 16) & 0xff;
        data_buf[3] = (offset >> 8) & 0xff;
        data_buf[4] = (offset >> 0) & 0xff;

        if (send(static_cast<uint32_t>(cmd_list::RMEM_COMMAND), data_buf, sizeof(data_buf))) {
            printf("read not accept \r\n");
            return EINVAL;
        }


        uint8_t answer[256]; 
        // TODO wait data


        if (memcmp(answer, data, chank_size)){
            printf("verify error, offset %x, cpu = %x, file = %x\r\n", offset + i, *answer, *data);
            return EINVAL;
        }

        error_count = 0;
      
        i+=chank_size;
        point_print(i);
    }

    return 0;
}

int new_can_api::start(){
    return 0;
}

new_can_api::new_can_api(serial::Serial *s) : boot_api(s) {}


int new_can_api::send(uint32_t id, void *p, int len){
    auto data = reinterpret_cast<uint8_t*>(p);
    while (len >= 0) {
        uint8_t chank_len = 8;
        if (len < chank_len) {
            len = chank_len;
        }

        uint8_t data_buf[8] = {0};
        memcpy(data_buf, data, chank_len);
 
        char out_buf[64] = {0};
        snprintf(out_buf, sizeof(out_buf), "T%08x%01d%02x%02x%02x%02x%02x%02x%02x%02x\r", id, chank_len,
            data_buf[0], data_buf[1], data_buf[2], data_buf[3],
            data_buf[4], data_buf[5], data_buf[6], data_buf[7]);
        uint32_t size = strlen(out_buf);      

        if (send_command(out_buf, size)) {
            printf("command id %02x not accept \r\n", id & 0xff);
            return EINVAL;
        }

        uint8_t answer[8];
        uint32_t id_rcv = UINT32_MAX;
        int rv;      
        if ((rv = wait_answer(&id_rcv, answer, 100)) < 0){
            continue;
        }
        if ((id_rcv != id) || (rv != 1) || (answer[0] != static_cast<uint32_t>(cmd_list::BL_ACK))){
            continue;
        }

        len = len - chank_len;
        data += chank_len;
    }

    return 0;
}


int new_can_api::read(uint32_t id, void *p, int len){
    auto data = reinterpret_cast<uint8_t*>(p);
    while (len >= 0) {
        uint8_t chank_len = 8;
        if (len < chank_len) {
            len = chank_len;
        }

        uint8_t data_buf[8] = {0};
        memcpy(data_buf, data, chank_len);
 
        char out_buf[64] = {0};
        snprintf(out_buf, sizeof(out_buf), "T%08x%01d%02x%02x%02x%02x%02x%02x%02x%02x\r", id, chank_len,
            data_buf[0], data_buf[1], data_buf[2], data_buf[3],
            data_buf[4], data_buf[5], data_buf[6], data_buf[7]);
        uint32_t size = strlen(out_buf);      

        if (send_command(out_buf, size)) {
            printf("command id %02x not accept \r\n", id & 0xff);
            return EINVAL;
        }

        uint8_t answer[8];
        uint32_t id_rcv = UINT32_MAX;
        int rv;      
        if ((rv = wait_answer(&id_rcv, answer, 100)) < 0){
            continue;
        }
        if ((id_rcv != id) || (rv != 1) || (answer[0] != static_cast<uint32_t>(cmd_list::BL_ACK))){
            continue;
        }

        len = len - chank_len;
        data += chank_len;
    }

    return 0;
}
/*

    uint32_t max_size = 1024*1024;
    uint32_t file_size = 0;

    uint8_t *in_flash = nullptr;
    in_flash = new uint8_t[max_size];
    memset(in_flash, 0xFF, max_size);

    /// Копируем bootloader.bin.
    r = read_bin_file(argv[2], in_flash, &file_size);
    if (r){
        cout << "File " << argv[2] << " does not exist!\n" << endl;
        return EINVAL;
    }
    printf("filesize = %d\r\n", file_size);
    offset = offset - 0x08000000;



    printf("\r\nboot succeced \r\n");     
    return 0;


    */