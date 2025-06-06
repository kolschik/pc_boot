#include "spi.h"

#include "spi_sama5d3.h"
#include <cerrno> 

Spi::Spi(const std::string &hardware_type, spi_hardware_cfg_t *spi_cfg)
{
    if (hardware_type == "SAMA5D3") {
        m_spi_hardware = new SpiSama5d3();
    }
    else if (hardware_type == "SAM9260") {
        m_spi_hardware = NULL;
    }
    if (m_spi_hardware == NULL) {
        std::fprintf(stderr, "SPI hardware interface is not allocated\n");
        std::exit(ENOENT);
    }
    int rv = m_spi_hardware->init(spi_cfg);
    if (rv != 0) {
        std::fprintf(stderr, "SPI hardware interface init failed\n");
        std::exit(rv);
    }
}

int Spi::transfer(uint8_t cs, uint8_t *inbuf, size_t size, uint8_t *outbuf)
{
    if (m_spi_hardware == NULL) {
        return -ENOENT;
    }
    if (inbuf == NULL || outbuf == NULL) {
        return -EINVAL;
    }
    return m_spi_hardware->trans(cs, inbuf, size, outbuf);
}
