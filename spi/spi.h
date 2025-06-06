#ifndef SPI_H_
#define SPI_H_

#include "spi_hardware.h"
#include <string>

class Spi {
public:
    Spi(const std::string &hardware_type, spi_hardware_cfg_t *spi_cfg);
    int transfer(uint8_t cs, uint8_t *inbuf, size_t size, uint8_t *outbuf);
private:
    SpiHardwareIf *m_spi_hardware;
};

#endif /*SPI_H_*/
