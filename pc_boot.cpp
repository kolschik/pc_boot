

#include "string.h"
#include "stdint.h"
#include "errno.h"
#include <iostream>//директива препроцесора
#include <fstream>
#include "serial.h"
#include <unistd.h> // для Unix систем

#include "uart_api.h"
#include "can_api.h"

using namespace std;

int wait_answer(uint32_t *id, uint8_t *array, uint32_t timeout);
int send_command(const char *s, uint32_t size, uint32_t timeout = 100);
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

int write_final_bin (char *path, uint8_t *flash_buf, uint32_t len) {

    ofstream out(path, ios::binary);

    if (out.bad()) {
        return -1;
    }

    out.write((char *)flash_buf, len);
    out.close();

    return 0;
}

int main (int argc, char *argv[]) {
    int r;
    int debug = 0;
    if (argc != 5) {
        cout << "Wrong number of parameters. 4 parameters are required: " << endl;
        cout << "1. protocol can, uart" << endl;
        cout << "2. name tty" << endl;
        cout << "3. Path to bin file (<<.bin>> file)." << endl;
        cout << "4. offset." << endl;
        cout << endl;
        return EINVAL;
    }

    uint32_t page_size = 128, flash_size = 16384;

    uint32_t offset;
    r = sscanf(argv[4], "%x", &offset);

    if (r==0){
        return EINVAL;
    }

    if (offset < (0x08000000 + 3 * 1024)){
        return EINVAL;
    }


    uint32_t max_size = 1024*1024;
    uint32_t file_size = 0;

    uint8_t *in_flash = nullptr;
    in_flash = new uint8_t[max_size];
    memset(in_flash, 0xFF, max_size);

    /// Копируем bootloader.bin.
    r = read_bin_file(argv[3], in_flash, &file_size);
    if (r){
        cout << "File " << argv[3] << " does not exist!\n" << endl;
        return EINVAL;
    }
    if (debug)  printf("filesize = %d\r\n", file_size);

    serial::Serial *ser = new serial::Serial();

    ser->setPort(argv[2]);
    ser->setBaudrate(115200);

    ser->open();

    if (!ser->isOpen()) {
        printf("no tty %s\r\n", argv[2]);
        return EINVAL;
    }

    if (debug) printf("open tty ok\r\n");


    boot_api * api = nullptr;
    if (strcmp(argv[1], "uart") == 0){
        api = new uart_api(ser);
    } else if (strcmp(argv[1], "can") == 0) {
        api = new can_api(ser);
    } else {
        printf("not valid protocol %s\r\n", argv[1]);
        return EINVAL;
    }

    if (api->open()) {
        printf("no open pipe\r\n");        
        return EINVAL;
    }
    if (debug) printf("bus open \r\n");


    if (api->detect()){
        printf("no detect device\r\n");        
        return EINVAL;
    }

    if (api->lock(1)){
        return EINVAL;
    }
    if (debug) printf("unlock succeced \r\n"); 


    uint32_t page_count = (flash_size - (offset - 0x08000000)) / page_size;
    if (debug) printf("page erase count = %d \r\n", page_count);

    if (api->erase(offset, page_count, page_size)){
        printf("erase failed \r\n");
        return EINVAL;
    }

 
    printf("\r\n"); 
    printf("erase succeced \r\n"); 

    if (api->write(offset, in_flash, file_size)){
        printf("write error \r\n");
    }

    printf("\r\nwrite succeced \r\n");

    if (api->lock(0)){
        return EINVAL;
    }
    if (debug) printf("lock succeced \r\n");     

    if (api->verify(offset, in_flash, file_size)){
        printf("verify error \r\n");
    }
    printf("\r\nverify completed \r\n");

    api->start();
    return 0;
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