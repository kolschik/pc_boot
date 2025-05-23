

#include "string.h"
#include "stdint.h"
#include "errno.h"
#include <iostream>//директива препроцесора
#include <fstream>
#include "serial.h"
#include <unistd.h> // для Unix систем
#include <chrono>
#include "api.h"

using namespace std;

int wait_answer(uint32_t *id, uint8_t *array, uint32_t timeout);
int send_command(const char *s, uint32_t size, uint32_t timeout = 100);
/*
uint8_t *malloc_array_flash (name_mc name) {
    uint32_t mc_support_count = sizeof(mc_base_name)/sizeof(mc_base_name[0]);
    int i = 0;
    for (; i < mc_support_count; i++) {
        if (mc_base_name[i].cod_name == name) {
            break;
        }
    }

    if (i >= mc_support_count) {
        return nullptr;
    }



    uint32_t len = 0;
    uint32_t index = static_cast<uint32_t>(name);
    len += mc_map_base[index].number_bytes_backup_boot_cfg;
    len += mc_map_base[index].number_bytes_boot;
    len += mc_map_base[index].number_bytes_boot_cfg;


    return p_flash;
}

name_mc get_type_mc (char *string_name) {
    /// Проходимся по всей базе ключ-значение.
    for (uint32_t l = 0; l < sizeof(mc_base_name); l++) {
        int r;
        r = strcmp(mc_base_name[l].stirng_name, string_name);
        if (r == 0) {
            return mc_base_name[l].cod_name;
        }
    }
    return name_mc::none;
}
*/
int read_bin_file (char *path, uint8_t *flash_mc, uint32_t *size) {
    int ch;
    uint32_t l = 0;
    ifstream f(path);
    while ((ch = f.get()) >= 0) {
        flash_mc[l] = (uint8_t)ch;
        l++;
    }
    f.close();
    if (l != 0) {
        *size = l;
        return 0;
    } else {
        return -1;
    }
}
/*

uint16_t get_crc16_block (uint8_t *addr_block, uint32_t len) {
    integrity_control c(pkt_check_type::crc16);
    for (uint32_t l = 0; l < len; l++) {
        c.add_item(addr_block[l]);
    }

    uint16_t crc16;
    crc16 = c.get_control_value();
    return crc16;
}

void add_crc16_to_block (uint8_t *p_start_block, uint16_t crc16, uint32_t len_block) {
    p_start_block[len_block - 2] = (uint8_t)crc16;
    p_start_block[len_block - 1] = (uint8_t)(crc16 >> 8);
}
*/
int write_final_bin (char *path, uint8_t *flash_buf, uint32_t len) {

    ofstream out(path, ios::binary);

    if (out.bad()) {
        return -1;
    }

    out.write((char *)flash_buf, len);
    out.close();

    return 0;
}


uint8_t key[] = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                  0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };
uint8_t out[] = { 0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c, 0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8 };


uint8_t in[]  = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a };



/*
    printf("ECB encrypt: ");

    if (0 == memcmp((char*) out, (char*) in, 16)) {
        printf("SUCCESS!\n");
	return(0);
    } else {
        printf("FAILURE!\n");
	return(1);
    }
*/
serial::Serial s;

int main (int argc, char *argv[]) {
    int r;

    if (argc != 4) {
        cout << "Wrong number of parameters. 4 parameters are required: " << endl;
        cout << "1. name tty" << endl;
        cout << "2. Path to bin file (<<.bin>> file)." << endl;
        cout << "3. offset." << endl;
        cout << endl;
        return EINVAL;
    }

    uint32_t page_size = 1024, flash_size = 16384;

    uint32_t offset;
    r = sscanf(argv[3], "%x", &offset);

    if (r==0){
        return EINVAL;
    }

    if (offset < (0x08000000 + 4096)){
        return EINVAL;
    }

    s.setPort(argv[1]);
    s.setBaudrate(115200);

    s.open();

    if (!s.isOpen()) {
        printf("no tty %s\r\n", argv[1]);
        return EINVAL;
    }

    printf("open tty ok\r\n");

    const char s_Open[3] = {"O\r"};

    if (send_command(s_Open, sizeof(s_Open) - 1)) {
        printf("no open can pipe \r\n");
        return EINVAL;
    }

    printf("can bus open \r\n");

    // detect
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
        if ((r = wait_answer(&id_rcv.raw, answer, 100)) < 0){
            continue;
        }
        if ((id_rcv.raw != id.raw) || (r != 3)){
            continue;
        }
        printf("device detected = %c%c%c\r\n", answer[0], answer[1], answer[2]); 
        break;
    }

    // unlock
    if (1){
        boot_id_t id = {0};
        id.com.address = 0;
        id.com.command = boot_code_unlock;

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
        if ((r = wait_answer(&id_rcv.raw, answer, 1000)) < 0){
            printf("command unlock timeout \r\n");
            return EINVAL;
        }
        if ((id_rcv.raw != id.raw) || (answer[0] != 0)){
            printf("unlock = %x, pld = %x \r\n", id.raw, answer[0]);
            return EINVAL;
        }
        printf("unlock succeced \r\n"); 
    }

   
   // struct AES_ctx ctx;
    //AES_init_ctx(&ctx, key);

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

    uint32_t page_count = (flash_size - offset) / page_size;
    printf("page erase count = %d \r\n", page_count);
    uint32_t error_count = 0;

    for (uint32_t i=0; i<page_count; ){
        if (error_count >= 5){
            printf("very big error \r\n");
            return EINVAL;
        }
        boot_id_t id = {0};
        id.com.address = offset+i*page_size;
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
        if ((r = wait_answer(&id_rcv.raw, answer, 1000)) < 0){
            error_count++;
            continue;
        }
        if ((id_rcv.raw != id.raw) || (answer[0] != 0)){
            printf("erase = %x, pld = %x \r\n", id.raw, answer[0]);
            error_count++;
            continue;
        }
        printf(".");
        fflush(stdout);
        i+=1;
    }
    printf("\r\n"); 
    printf("erase succeced \r\n"); 

    error_count = 0;
    for (uint32_t i=0; i<file_size; ){
        if (error_count >= 5){
            printf("very big error \r\n");
            return EINVAL;
        }
        uint32_t *l_byte, *h_byte;
        l_byte = (uint32_t *)&in_flash[i];
        h_byte = l_byte + 1;

        boot_id_t id = {0};
        id.com.address = offset+i;
        id.com.command = boot_code_write;

        char out_buf[64] = {0};
        //T 000050b8 8 0800 6c5d 0800 6c5d
        snprintf(out_buf, sizeof(out_buf), "T%08x8%08x%08x\r", id.raw, *h_byte, *l_byte);
        uint32_t size = strlen(out_buf);      

        if (send_command(out_buf, size)) {
            printf("command not accept \r\n");
            return EINVAL;
        }
        uint8_t answer[8]; 

        boot_id_t id_rcv = {0};
        if ((r = wait_answer(&id_rcv.raw, answer, 1000)) < 0){
            error_count++;
            continue;
        }
        if ((id_rcv.raw != id.raw) || (answer[0] != 0)){
            printf("rcv = %x, pld = %x \r\n", id.raw, answer[0]);
            error_count++;
            continue;
        }
        error_count = 0;
      
        i+=8;
        printf(".");
        fflush(stdout);  
    }
    printf("\r\nboot succeced \r\n");     
    return 0;
}



int send_command(const char *str, uint32_t size, uint32_t timeout){
    uint32_t send_byte = s.write((uint8_t *)str, size);

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
        uint32_t count_byte_packet = s.read(buf_in_serial_data, 2048);

        if ((count_byte_packet == 1) && (buf_in_serial_data[0] == 0x0d)){
            return 0;
        }
    }
    return ETIMEDOUT;
}

int wait_answer(uint32_t *id, uint8_t *array, uint32_t timeout){
    uint8_t buf_in_serial_data[256] = {0};

    auto cur_time = std::chrono::system_clock::now();
    auto t_stop = cur_time + std::chrono::milliseconds(timeout);
    auto end_time = t_stop;
    int rv = -ETIMEDOUT;
    uint32_t count_byte_packet = 0;
    while (cur_time < end_time){ 
        cur_time = std::chrono::system_clock::now();            
        count_byte_packet = s.read(buf_in_serial_data, 256);

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
/*
    if (file_size & 0xff){
        file_size &= ~0xff;
        file_size += 256;
    }
    printf("aligned 256 = %d\r\n", file_size);    


    uint32_t cpy_size = 512;
    for (;cpy_size < file_size; cpy_size += 16){
        uint32_t iter_size = 16;
        if ((file_size - cpy_size) < 16){
            iter_size = file_size - cpy_size;
        }
        uint8_t cr_buf[16];
        memset(cr_buf, 0xff, sizeof(cr_buf));
        memcpy(cr_buf, &in_flash[cpy_size], iter_size);


        //AES_ECB_encrypt(&ctx, cr_buf);  
        memcpy(&out_flash[cpy_size], cr_buf, 16);              

    }
    /// Записываем итоговый bin-файл.
    r = write_final_bin(argv[2], out_flash, cpy_size);

    if (r == 0) {
        cout << "The bin file was created!" << endl;
    } else {
        cout << "The bin file was not created!" << endl;
    }
    */
/*
    uint16_t crc16;

    /// Копируем bootloader.bin.
    r = read_bin_file(argv[2], &flash_buf[mc_map_base[(uint32_t)mc_name].p_boot]);
    if (r == 0) {
        cout << "File " << argv[2] << " was copied." << endl;
    } else {
        cout << "File " << argv[2] << " does not exist!" << endl;
        return EINVAL;
    }


    /// 2 байта приходится на CRC16 блока и в CRC16 соответственно не входит.
    crc16 = get_crc16_block(&flash_buf[mc_map_base[(uint32_t)mc_name].p_boot],
                            mc_map_base[(uint32_t)mc_name].number_bytes_boot - 2);


    add_crc16_to_block(&flash_buf[mc_map_base[(uint32_t)mc_name].p_boot],
                       crc16,
                       mc_map_base[(uint32_t)mc_name].number_bytes_boot);

    /// Копируем BootLoaderData.
    read_bin_file(argv[3], &flash_buf[mc_map_base[(uint32_t)mc_name].p_boot_cfg]);
    if (r == 0) {
        cout << "File " << argv[3] << " was copied." << endl;
    } else {
        cout << "File " << argv[3] << " does not exist!" << endl;
        return EINVAL;
    }

    crc16 = get_crc16_block(&flash_buf[mc_map_base[(uint32_t)mc_name].p_boot_cfg],
                            mc_map_base[(uint32_t)mc_name].number_bytes_boot_cfg - 2);

    add_crc16_to_block(&flash_buf[mc_map_base[(uint32_t)mc_name].p_boot_cfg],
                       crc16,
                       mc_map_base[(uint32_t)mc_name].number_bytes_boot_cfg);

    /// Копируем BackupBootLoaderData.
    read_bin_file(argv[3],
                  &flash_buf[mc_map_base[(uint32_t)mc_name].p_backup_boot_cfg]);

    /// Повторно CRC16 не пересчитываем, поскольку он одинаков. Просто сверху напишем его.
    add_crc16_to_block(&flash_buf[mc_map_base[(uint32_t)mc_name].p_backup_boot_cfg],
                       crc16,
                       mc_map_base[(uint32_t)mc_name].number_bytes_backup_boot_cfg);

    /// Записываем итоговый bin-файл.
    r = write_final_bin(argv[4], flash_buf, mc_name);

    if (r == 0) {
        cout << "The bin file was created!" << endl;
    } else {
        cout << "The bin file was not created!" << endl;
    }
*/


/*

#include "pc_vin.h"

#if defined( MODULE_PC_VIN_ENABLED ) && defined( MODULE_VIN_BASE_ENABLED ) && \
        defined( MODULE_CPP_INTEGRITY_CONTROL_ENABLED ) && defined( MODULE_FSM_ENABLED )

#include <unistd.h>
#include <chrono>
#include <thread>

namespace vin {

pc_vin::pc_vin () {
}

pc_vin::~pc_vin () {
    this->s.close();
}

result pc_vin::set_serial_port (std::string p_name, uint32_t baudr) {
    this->s.setPort(p_name);
    this->s.setBaudrate(baudr);

    this->s.open();

    if (!this->s.isOpen()) {
        return result::err_portnotredy;
    }

    return result::ok;
}

result pc_vin::change_baud (uint32_t baudrade) {
    this->s.setBaudrate(baudrade);
    return result::ok;
}

result pc_vin::receive_packet (uint8_t *packet, uint32_t max_packet_len, uint8_t *address, answer *answer,
                               uint32_t *received_len, pkt_check_type type, uint32_t timeout) {
    /// Забираем то, что уже в буффере.
    this->s.setTimeout(0, 0, 0, 0, 0);
    this->decoder.reset();
    this->decoder.setTypeIntegrityControl(type);

    auto cur_time = std::chrono::system_clock::now();
    auto t_stop = cur_time + std::chrono::milliseconds(timeout);

    auto end_time = t_stop;
    std::vector<uint8_t> input_packet;
    while (cur_time < end_time) {
        cur_time = std::chrono::system_clock::now();
        uint32_t count_byte_packet = this->s.read(this->buf_in_serial_data, 2048);
        if (count_byte_packet == 0) {
            continue;
        }

        input_packet = this->decoder.getRealData(this->buf_in_serial_data, count_byte_packet);
        if (!input_packet.empty()) {
            break;
        }
    };

    if (input_packet.size() < 2) {
        return result::err_reseivelen;
    }

    if (address) {
        *address = input_packet.at(0);
    }

    if (answer) {
        *answer = (vin::answer)input_packet.at(1);
    }

    unsigned int count_byte_packet;
    count_byte_packet = input_packet.size();

    *received_len = count_byte_packet - 2;

    if ((count_byte_packet - 2) <= max_packet_len) {
        uint8_t *p = packet;
        for (uint32_t l = 2; l < count_byte_packet; l++) {
            *p = input_packet.at(l);
            p++;
        }
        return result::ok;
    } else {
        return result::err_maxlen;
    }
}

result pc_vin::receive_packet (uint8_t *packet, uint32_t max_packet_len, uint8_t *address,
                               answer *answer, pkt_check_type type, uint32_t timeout) {
    uint32_t received_len = 0;
    return this->receive_packet(packet, max_packet_len, address, answer, &received_len, type, timeout);
}

result pc_vin::send_packet (uint8_t address,
                            uint8_t command,
                            uint8_t *packet,
                            uint32_t packet_len,
                            pkt_check_type control_type) {
    std::vector<uint8_t> b;

    if (packet != nullptr) {
        for (uint32_t l = 0; l < packet_len; l++) {
            b.push_back(packet[l]);
        }
    }

    std::vector<uint8_t> sendData = vin::getVinPackage(address, command, b, control_type);

    uint32_t coutByte = sendData.size();
    uint8_t *dataWrite = new uint8_t[coutByte];
    uint32_t i = 0;
    for (uint8_t byteSendPacked : sendData) {
        dataWrite[i++] = byteSendPacked;
    }

    size_t testByte;
    testByte = this->s.write(dataWrite, coutByte);

    delete[] dataWrite;

    if (testByte != coutByte) {
        return result::err_send;
    }

    return result::ok;
}

result pc_vin::send_packet_with_answer (uint8_t address,
                                        uint8_t command,
                                        answer *answer,
                                        uint8_t *send_packet_out,
                                        uint32_t send_packet_out_len,
                                        uint8_t *receive_packet,
                                        uint32_t max_receive_packet_len,
                                        uint32_t *received_len,
                                        uint32_t timeout,
                                        pkt_check_type control_type) {

    result r;
    r = this->send_packet(address, command, send_packet_out, send_packet_out_len, control_type);
    if (r != result::ok) return r;

    uint8_t in_addr;
    r = this->receive_packet(receive_packet, max_receive_packet_len, &in_addr, answer, received_len, control_type,
                             timeout);
    if (r != result::ok) return r;

    if (address != in_addr) {
        return result::err_reseivecommand;
    }

    return result::ok;
}

result pc_vin::send_packet_with_answer (uint8_t address,
                                        uint8_t command,
                                        answer *answer,
                                        uint8_t *send_packet_out,
                                        uint32_t send_packet_out_len,
                                        uint8_t *receive_packet,
                                        uint32_t max_receive_packet_len,
                                        uint32_t timeout,
                                        pkt_check_type control_type) {
    uint32_t received_len = 0;
    return this->send_packet_with_answer(address,
                                         command,
                                         answer,
                                         send_packet_out,
                                         send_packet_out_len,
                                         receive_packet,
                                         max_receive_packet_len,
                                         &received_len,
                                         timeout,
                                         control_type);
}
}

#endif

*/