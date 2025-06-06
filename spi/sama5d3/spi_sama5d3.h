#ifndef SPI_SAMA5D3_H_
#define SPI_SAMA5D3_H_

#include <cstdio>
#include <stdint.h>
#include "spi_hardware.h"
#include <sys/siginfo.h>

class SpiSama5d3 : public SpiHardwareIf {
public:
    int init(spi_hardware_cfg_t *cfg);
    int trans(uint8_t cs, uint8_t *inbuf, size_t size, uint8_t *outbuf);
private:
    int spi_init();
    int pio_init();
    int spi_cs_init(spi_hardware_cfg_t *cfg);
    int spi_xfer(uint32_t *tx, uint32_t *rx, uint32_t len);
    int spi_wait();

    uint32_t get_master_clock();

public:
    uintptr_t vbase;
    uint32_t pbase;
    uint32_t *tx_ptr;
    uint32_t *rx_ptr;
    uint32_t tx_len;
    uint32_t rx_len;
    uint32_t xfer_tx_len;
    uint32_t xfer_rx_len;
    int spi_iid;
    struct sigevent spi_interrupt_event;
    int chid;
    int coid;
    uint32_t inbuffer[1024];
    uint32_t outbuffer[1024];
};

#endif /*SPI_SAMA5D3_H_*/
