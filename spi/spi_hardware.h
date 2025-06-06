#ifndef SPI_HARDWARE_H_
#define SPI_HARDWARE_H_

#include <cstdio>
#include <unistd.h>
#include <stdint.h>

typedef struct chip_select_configuration {
    bool CPOL;
    bool NCPHA;
    bool CSAAT;
    uint32_t BITS;
    uint32_t DLYBCT;
    uint32_t DLYBS;
    uint32_t baudrate;
} cs_cfg_t;

typedef struct spi_hardware_configuration {
    cs_cfg_t cs_cfg[4];
} spi_hardware_cfg_t;

class SpiHardwareIf {
public:
    virtual int init(spi_hardware_cfg_t *cfg) = 0;
    virtual int trans(uint8_t cs, uint8_t *inbuf, size_t size, uint8_t *outbuf) = 0;
    virtual ~SpiHardwareIf() {}
private:

};

#endif /*SPI_HARDWARE_H_*/
