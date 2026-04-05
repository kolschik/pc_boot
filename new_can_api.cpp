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


        send(static_cast<uint32_t>(cmd_list::BL_ACK), buf, 0, 0);

        {
            if(send(static_cast<uint32_t>(cmd_list::GET_CMD_COMMAND), buf, 0)) {

                continue;
            }
            int rcv_len = read(static_cast<uint32_t>(cmd_list::GET_CMD_COMMAND), buf);
            if (rcv_len < 0) {
                printf("command get not accept \r\n");
                return EINVAL;
            }
        }

        {
            if(send(static_cast<uint32_t>(cmd_list::MAP_COMMAND), buf, 0)) {
                printf("command get map not accept \r\n");
                return EINVAL;
            }

            int rcv_len = read(static_cast<uint32_t>(cmd_list::MAP_COMMAND), buf, 8);
            if (rcv_len < 0) {
                printf("command map not accept \r\n");
                return EINVAL;
            }
            sector_size = buf[6] << 8 | buf[7];
            flash_size = (buf[5] + buf[4]) * sector_size;
            offset_comp = buf[4] * sector_size + 0x08000000;

            printf("\ndevice detected = %c%c%c%c, flash size = %d bytes, sector_size = %d b offset = 0x%x \r\n", buf[0], 
                buf[1], buf[2], buf[3], flash_size, sector_size, offset_comp); 

        }
        break;
    }
    return 0;
}


int new_can_api::send_command(const char *str, uint32_t size, uint32_t timeout){
   // s->flushInput();
   uint8_t cymb;
   uint8_t read_cnt=s->read(&cymb,1);
   if(read_cnt == 1){
    printf("alarma %x\r\n", cymb);
   }
    uint32_t send_byte = s->write((uint8_t *)str, size);

    if (send_byte != size){
        return EFAULT;
    }



    auto cur_time = std::chrono::system_clock::now();
    auto t_stop = cur_time + std::chrono::milliseconds(timeout);
    auto end_time = t_stop;
    int rv = ETIMEDOUT;
    while (cur_time < end_time){ 
        cur_time = std::chrono::system_clock::now();  
        uint8_t  c;       
        uint32_t count_byte_packet = s->read(&c, 1);
        if (count_byte_packet != 1){
            continue;
        }
        if (c == 0x0d){
            return 0;
        }
    }

    return ETIMEDOUT;
}

int new_can_api::wait_answer(uint32_t *id, uint8_t *array, uint32_t timeout){


    int buf_idx = 0;
    auto cur_time = std::chrono::system_clock::now();
    auto t_stop = cur_time + std::chrono::milliseconds(timeout);
    auto end_time = t_stop;
    int rv = -ETIMEDOUT;
    uint8_t buf_in_serial_data[256] = {0};
    while (cur_time < end_time){ 
        cur_time = std::chrono::system_clock::now();   
        uint8_t c;         
        uint32_t count_byte_packet = s->read(&c, 1);

        if (count_byte_packet != 1){
            continue;
        }

        if (buf_idx >= sizeof(buf_in_serial_data)) {
            return -ENOMEM;
        }
        buf_in_serial_data[buf_idx] = c;
        buf_idx++;

        if ((c == 0xd) && (*buf_in_serial_data == 't') && (buf_idx >= 4)){
            buf_in_serial_data[buf_idx] = 0;

            char char_id[9] = {0};
            memcpy(char_id, &buf_in_serial_data[1], 3);
            sscanf(char_id, "%x", id);

            uint8_t num = buf_in_serial_data[4];
            if ((num < '0') || (num > '8')){
                return -EINVAL;   
            }
            num = num - '0';
    
            uint8_t *buf_p = &buf_in_serial_data[5];
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

        if (c == 0xd){
            buf_idx = 0;
            continue;;
        }
    }


    return rv;
}

int new_can_api::lock(uint8_t lock){
    return 0;
}



int new_can_api::erase(uint32_t offset, uint32_t page_cnt, uint32_t page_size) {

    if (page_cnt == 0){
        return EFAULT;
    }
    uint8_t data_buf[6];
    data_buf[0] = (offset >> 24) & 0xff;
    data_buf[1] = (offset >> 16) & 0xff;
    data_buf[2] = (offset >> 8) & 0xff;
    data_buf[3] = (offset >> 0) & 0xff;

    data_buf[4] = ((page_cnt - 1) >> 8) & 0xff;
    data_buf[5] = ((page_cnt - 1) >> 0) & 0xff;

    if (send(static_cast<uint32_t>(cmd_list::EMEM_COMMAND), data_buf, sizeof(data_buf))) {
        printf("erase not accept \r\n");
        return EINVAL;
    }

    for (uint32_t i=0; i<page_cnt; i++){
        if (wait_ack(static_cast<uint32_t>(cmd_list::EMEM_COMMAND), 5000)){
            printf("page erase fail \r\n");
            return EINVAL;
        }
        printf(".");
        fflush(stdout);
    }

    return 0;
}


int new_can_api::write(uint32_t offset, uint8_t *data, uint32_t l){
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
        data_buf[0] = (offset >> 24) & 0xff;
        data_buf[1] = (offset >> 16) & 0xff;
        data_buf[2] = (offset >> 8) & 0xff;
        data_buf[3] = (offset >> 0) & 0xff;

        data_buf[4] = chank_size-1;

        if (send(static_cast<uint32_t>(cmd_list::WMEM_COMMAND)  , data_buf, sizeof(data_buf))) {
            printf("write not accept \r\n");
            return EINVAL;
        }

        uint8_t chank_buf[256];
        memcpy(chank_buf, data, chank_size);

        if (send(static_cast<uint32_t>(cmd_list::DATA_PAYLOAD), chank_buf, chank_size)) {
            printf("write payload failed\r\n");
            return EINVAL;
        }
        error_count = 0;
      
        if (wait_ack(static_cast<uint32_t>(cmd_list::WMEM_COMMAND), 5000)){
            printf("write chank failed\r\n");
            return EINVAL;
        }
        i+=chank_size;
        offset += chank_size;
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


int new_can_api::send(uint32_t id, void *p, int len, bool nead_answer){
    auto data = reinterpret_cast<uint8_t*>(p);
    while (len >= 0) {
        uint8_t chank_len = 8;
        if (len < chank_len) {
            chank_len = len;
        }

        auto chat2uint = [](char*s, uint8_t dig){
            *s = (dig >> 4) + '0';
            if (*s > '9') {
                *s = *s - '0' - 10 + 'a';
            }
            s++;
            *s = (dig & 0xf) + '0';
            if (*s > '9') {
                *s = *s - '0' - 10 + 'a';
            }
            return;
        };

        uint8_t data_buf[8] = {0};
        memcpy(data_buf, data, chank_len);

        char payload_str[17] = {0};
        char *str_p = payload_str;
        for (int i=0; i<chank_len;i++){
            
            chat2uint(str_p, data_buf[i]);
            str_p += 2;
        }
        *str_p = 0;

        char out_buf[64] = {0};
        snprintf(out_buf, sizeof(out_buf), "t%03x%01d%s\r", id, chank_len, payload_str);
        uint32_t size = strlen(out_buf);      

        if (send_command(out_buf, size)) {
            printf("command id %02x not accept \r\n", id & 0xff);
            return EINVAL;
        }
        if (nead_answer == 0){
            return 0;
        }

        uint8_t answer[8];
        uint32_t id_rcv = UINT32_MAX;
        int rv;      

        if ((rv = wait_answer(&id_rcv, answer, 1000)) < 0){
            printf ("failed recieve ack send id %x, rv %d\r\n", id, rv);
            return -EFAULT;
        }


        if ((id_rcv != id) || (rv != 1) || (answer[0] != static_cast<uint32_t>(cmd_list::BL_ACK))){
            printf ("recieve noack id %x\r\n", id_rcv);
            return -EFAULT;
        }


        len = len - chank_len;
        data += chank_len;

        if (len == 0) {
            break;
        }
    }

    return 0;
}


int new_can_api::read(uint32_t id, void *p,  int len){
    auto data = reinterpret_cast<uint8_t*>(p);

    uint8_t answer[8];

    if (len == -1) {
        uint32_t id_rcv = UINT32_MAX;
        int rv;    
        if ((rv = wait_answer(&id_rcv, answer, 500)) < 0){
            return -ETIMEDOUT;
        }
        if ((id != id_rcv) || (rv != 1)) {
            printf ("failed recieve message len \r\n");
            return -ETIMEDOUT; 
        }

        len = answer[0];
    }
    int rv_len = 0;
    while (len > 0) {
        uint32_t id_rcv = UINT32_MAX;
        int rcv_len;  
        if ((rcv_len = wait_answer(&id_rcv, answer, 500)) < 0){
            printf ("id_rcv zalupa\r\n");
            return -ETIMEDOUT;
        }
  
        if (id != id_rcv) {
            printf ("id_rcv different id %d %d\r\n", id, id_rcv);
            return -EFAULT; 
        }

        uint8_t data_buf[8] = {0};
        memcpy(&data[rv_len], answer, rcv_len);
        rv_len += rcv_len;
        len = len - rcv_len;
    }


    int no_wait_ack = 0;
    if (no_wait_ack == 0) {
        if (wait_ack(id, 500)){
            return -EFAULT;
        }
    }

    return rv_len;
}

int new_can_api::wait_ack(uint32_t id, uint32_t timeout){
    uint32_t id_rcv = UINT32_MAX;
    int rv;
    uint8_t answer[8];
    if ((rv = wait_answer(&id_rcv, answer, timeout)) < 0){
        printf ("failed recieve ack read id %x\r\n", id_rcv);
        return -EFAULT;
    }
    if ((id == id_rcv) && (rv == 1) && (answer[0] == (uint8_t)cmd_list::BL_NAK)){
        printf ("recieve no_ack id %x\r\n", id_rcv);
        return -EBADMSG;
    }
    if ((id != id_rcv) || (rv != 1)) {
        printf ("failed recieve ack id %x\r\n", id_rcv);
        return -EBADMSG;
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