#include "spi_sama5d3.h"

#include <sys/mman.h>
#include <hw/inout.h>
#include <sys/neutrino.h>
#include <sys/siginfo.h>
#include <sched.h>
#include <errno.h>
#include <cstdlib>
#include <cstring>
#include <pthread.h>

#include "at91sama5d3_dma_devctl.h"
#include "at91sama5d3_dmac.h"
#include "at91sama5d3_pio.h"
#include "at91sama5d3_pmc.h"
#include "at91sama5d3_spi.h"
#include "at91sama5d3_tc.h"
#include "at91sama5d3_pdc.h"
#include "at91sam_spi.h"

#define SAMA5D3_SPI_INTERRUPT 6
#define SAMA5D3_DMA_WITH_INT_BLOCKING 0
#define SAMA5D3_DMA_SET_WITH_COPY 0

static SpiSama5d3 *spi;

int SpiSama5d3::init(spi_hardware_cfg_t *cfg)
{
    int rv;
    spi = this;
    // Launch ThreadCtl to allow operations with I/O
    if (ThreadCtl(_NTO_TCTL_IO, 0) == -1) {
        return errno;
    }
    if (cfg == NULL) {
        return EINVAL;
    }
    // Conenct channel for messages exchange
    chid = ChannelCreate_r(0);
    coid = ConnectAttach_r(0, 0, chid, 0, 0);
    // Set high priority
    /*
    struct sched_param param;
    int policy;
    pthread_getschedparam(pthread_self(), &policy, &param);
    param.sched_priority = 60;
    if ((rv = pthread_setschedparam(pthread_self(), policy, &param)) != EOK) {
    	err_errno (__func__, rv, "pthread_setschedparam failed");
    }*/
    // Create and attach channel for message exchange
    pbase = AT91SAMA5D3_SPI_BASE1;

    InterruptEnable();
    pio_init();
    spi_init();
    // CS initialization from device configuration
    spi_cs_init(cfg);
    delay(20);
    return 0;
}

// PIO
int SpiSama5d3::pio_init()
{
    uintptr_t pio_base = NULL;
    if (pio_base != NULL) {
        munmap_device_io(AT91SAMA5D3_PIO_PIOC, AT91SAMA5D3_PIO_SIZE);
    }
    pio_base = mmap_device_io(AT91SAMA5D3_PIO_SIZE, AT91SAMA5D3_PIO_PIOC);
    // Disable write protection
    out32(pio_base + 0xE4, 0x50494F00);
    // Disable PIO, enable work with peripheral
    volatile uint32_t reg = (0x1 << 22) | (0x1 << 23) | (0x1 << 24) |
                            (0x1 << 25) | (0x1 << 26) | (0x1 << 27) |
                            (0x1 << 28);
    out32(pio_base + AT91SAMA5D3_PIO_PSR, reg);
    // Peripheral A
    reg = in32(pio_base + AT91SAMA5D3_PIO_ABCDSR1);
    reg &= ~((0x1 << 22) | (0x1 << 23) | (0x1 << 24) | (0x1 << 25) |
             (0x1 << 26) | (0x1 << 27) | (0x1 << 28));
    out32(pio_base + AT91SAMA5D3_PIO_ABCDSR1, reg);
    reg = in32(pio_base + AT91SAMA5D3_PIO_ABCDSR2);
    reg &= ~((0x1 << 22) | (0x1 << 23) | (0x1 << 24) | (0x1 << 25) |
             (0x1 << 26) | (0x1 << 27) | (0x1 << 28));
    out32(pio_base + AT91SAMA5D3_PIO_ABCDSR2, reg);
    // Enable outputs for CS pins
    reg = (0x1 << 25) | (0x1 << 26) | (0x1 << 27) | (0x1 << 28);
    out32(pio_base + AT91SAMA5D3_PIO_OER, reg);
    // Pull-up for MISO, MOSI, CLK
    reg = (0x1 << 22) | (0x1 << 23) | (0x1 << 24);
    out32(pio_base + AT91SAMA5D3_PIO_PUER, reg);
    return 0;
}

const struct sigevent *sama5d3_spi_handler(void *area, int size)
{
    uint32_t sr;
    uint32_t data;
    sr = in32(spi->vbase + AT91SAMA5D3_SPI_SR);
    if (sr & AT91SAMA5D3_SPI_SR_RDRF) {
        data = in32(spi->vbase + AT91SAMA5D3_SPI_RDR);
        if (spi->xfer_rx_len < spi->rx_len) {
            spi->rx_ptr[spi->xfer_rx_len] = data;
            spi->xfer_rx_len++;
        }
    }
    if (sr & AT91SAMA5D3_SPI_SR_TDRE) {
        if (spi->xfer_tx_len < spi->tx_len) {
            out32(spi->vbase + AT91SAMA5D3_SPI_TDR,
                  spi->tx_ptr[spi->xfer_tx_len]);
            spi->xfer_tx_len++;
        }
    }
    if (spi->xfer_rx_len >= spi->rx_len) {
        out32(spi->vbase + AT91SAMA5D3_SPI_IDR,
              AT91SAMA5D3_SPI_SR_RDRF | AT91SAMA5D3_SPI_SR_TDRE);
        return (&spi->spi_interrupt_event);
    }
    return NULL;
}

// SPI
int SpiSama5d3::spi_init()
{
    if (vbase != NULL) {
        munmap_device_io(pbase, AT91SAMA5D3_SPI_SIZE);
    }
    vbase = mmap_device_io(AT91SAMA5D3_SPI_SIZE, pbase);
    // Disable SPI and restart controller (twice, like Atmel suggest!)
    out32(vbase + AT91SAMA5D3_SPI_CR, AT91SAMA5D3_SPI_CR_SPIDIS);
    out32(vbase + AT91SAMA5D3_SPI_CR, AT91SAMA5D3_SPI_CR_SWRST);
    out32(vbase + AT91SAMA5D3_SPI_CR, AT91SAMA5D3_SPI_CR_SWRST);
    // Disable register write protection and all interrupts
    out32(vbase + 0xE4, AT91SAMA5D3_SPI_WP_KEY);
    out32(vbase + AT91SAMA5D3_SPI_IDR, 0xFFFFFFFF);
    // Master mode, variable CS, use external chip select decoder
    uint32_t mode = 0x00;
    mode = AT91SAMA5D3_SPI_MR_MSTR | AT91SAMA5D3_SPI_MR_PS_VARIABLE |
           AT91SAMA5D3_SPI_MR_PCSDEC | AT91SAMA5D3_SPI_MR_DLYBCS(20) |
           AT91SAMA5D3_SPI_MR_WDRBT;
    out32(vbase + AT91SAMA5D3_SPI_MR, mode);

    struct sched_param param;
    int policy;
    pthread_getschedparam(pthread_self(), &policy, &param);
    SIGEV_PULSE_INIT(&spi_interrupt_event, coid,
        param.sched_priority + 1, SAMA5D3_SPI_INTERRUPT, 0);

    spi_iid = InterruptAttach_r(AT91SAMA5D3_ID_SPI1, sama5d3_spi_handler, spi, 0, 0);

    if (spi_iid < 0) {
        return -spi_iid;
    }

    return 0;
}

uint32_t SpiSama5d3::get_master_clock()
{
    return 100000000;
}

int SpiSama5d3::spi_cs_init(spi_hardware_cfg_t *cfg)
{
    int i;
    for (i = 0; i < 4; i++) {
        uint32_t reg = AT91SAM_SPI_CSR_SCBR((get_master_clock() / cfg->cs_cfg[i].baudrate));
        if (cfg->cs_cfg[i].CPOL) {
            reg |= AT91SAM_SPI_CSR_CPOL;
        }
        if (cfg->cs_cfg[i].NCPHA) {
            reg |= AT91SAM_SPI_CSR_NCPHA;
        }
        if (cfg->cs_cfg[i].CSAAT) {
            reg |= AT91SAM_SPI_CSR_CSAAT;
        }
        reg |= AT91SAM_SPI_CSR_BITS(cfg->cs_cfg[i].BITS);
        reg |= AT91SAM_SPI_CSR_DLYBCT(cfg->cs_cfg[i].DLYBCT);
        reg |= AT91SAM_SPI_CSR_DLYBS(cfg->cs_cfg[i].DLYBS);
        out32(vbase + AT91SAMA5D3_SPI_CSR(i), reg);
    }
    return 0;
}

// Transfer functions
// Without DMA
int SpiSama5d3::spi_xfer(uint32_t *tx, uint32_t *rx, uint32_t len)
{
    int rv = 0;
    tx_ptr = tx;
    tx_len = len;
    rx_ptr = rx;
    rx_len = len;
    xfer_rx_len = 0;
    xfer_tx_len = 0;

    out32(vbase + AT91SAMA5D3_SPI_CR, AT91SAMA5D3_SPI_CR_SPIEN);
    in32(vbase + AT91SAMA5D3_SPI_RDR);
    out32(vbase + AT91SAMA5D3_SPI_IER, AT91SAMA5D3_SPI_SR_RDRF | AT91SAMA5D3_SPI_SR_TDRE);

    if (spi_wait()) {
        rv = -1;
        //err_errno(__func__, EPROTO, "Exchange failed");
        return EPROTO;
    } else {
        rv = len;
    }

    out32(vbase + AT91SAMA5D3_SPI_IDR, AT91SAMA5D3_SPI_SR_RDRF);
    out32(vbase + AT91SAMA5D3_SPI_CR, AT91SAMA5D3_SPI_CR_SPIDIS);

    return rv;
}

int SpiSama5d3::spi_wait()
{
    int rv;
    struct _pulse pulse;
    while (1) {
        rv = MsgReceive_r(chid, &pulse, sizeof(struct _pulse), NULL);
        if (rv < 0) {
            //err_errno(__func__, -rv, "MsgReceive failed");
            return rv;
        }
        if (rv == 0){
            if (pulse.code == SAMA5D3_SPI_INTERRUPT) {
                return EOK;
            }
        }
    }

    return -1;
}

int SpiSama5d3::trans(uint8_t cs, uint8_t *inbuf, size_t size, uint8_t *outbuf)
{
    size_t i;
    for (i = 0; i < size - 1; i++) {
        inbuffer[i] = SPI_PDC_TRANS_BYTE_NO_LST(cs, inbuf[i]);
    }
    inbuffer[size - 1] = SPI_PDC_TRANS_BYTE(cs, inbuf[size - 1]);

    int rv = spi_xfer(inbuffer, outbuffer, size);
    if (rv < 0) {
        return rv;
    }
    for (i = 0; i < size; i++) {
        outbuf[i] = SPI_PDC_EXTRACT_BYTE(outbuffer[i]);
    }

    return size;
}
