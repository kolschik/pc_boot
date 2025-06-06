/*
 * Copyright (C) 2006 Microchip Technology Inc. and its subsidiaries
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef AT91SAMA5D3_SPI_H_
#define AT91SAMA5D3_SPI_H_

/*
 * SPI base
 */
#define AT91SAMA5D3_SPI_BASE0       0xF0004000UL
#define AT91SAMA5D3_SPI_BASE1       0xF8008000UL
#define AT91SAMA5D3_SPI_BASE_CLK    133333333UL
#define AT91SAMA5D3_SPI_SIZE        0x100
#define AT91SAMA5D3_ID_SPI0         24
#define AT91SAMA5D3_ID_SPI1         25

/* 
 * SPI registers offsets and values
 */
#define AT91SAMA5D3_SPI_CR      0x00
#define AT91SAMA5D3_SPI_MR      0x04
#define AT91SAMA5D3_SPI_RDR     0x08
#define AT91SAMA5D3_SPI_TDR     0x0C
#define AT91SAMA5D3_SPI_SR      0x10
#define AT91SAMA5D3_SPI_IER     0x14
#define AT91SAMA5D3_SPI_IDR     0x18
#define AT91SAMA5D3_SPI_IMR     0x1C
#define AT91SAMA5D3_SPI_CSR(x) (0x30 + 4 * (x))

/* SPI_CR : (SPI Offset: 0x0) SPI Control Register */ 
#define AT91SAMA5D3_SPI_CR_SPIEN             (0x1UL <<  0)
#define AT91SAMA5D3_SPI_CR_SPIDIS            (0x1UL <<  1)
#define AT91SAMA5D3_SPI_CR_SWRST             (0x1UL <<  7)
#define AT91SAMA5D3_SPI_CR_LASTXFER          (0x1UL << 24)

/* SPI_MR : (SPI Offset: 0x4) SPI Mode Register */ 
#define AT91SAMA5D3_SPI_MR_MSTR              (0x1UL <<  0)
#define AT91SAMA5D3_SPI_MR_PS                (0x1UL <<  1)
    #define AT91SAMA5D3_SPI_MR_PS_FIXED      (0x0UL <<  1)
    #define AT91SAMA5D3_SPI_MR_PS_VARIABLE   (0x1UL <<  1)
#define AT91SAMA5D3_SPI_MR_PCSDEC            (0x1UL <<  2)
#define AT91SAMA5D3_SPI_MR_MODFDIS           (0x1UL <<  4)
#define AT91SAMA5D3_SPI_MR_WDRBT             (0x1UL <<  5)
    #define AT91SAMA5D3_SPI_MR_WDRBT_OFF     (0x0UL <<  5)
    #define AT91SAMA5D3_SPI_MR_WDRBT_ON      (0x1UL <<  5)
#define AT91SAMA5D3_SPI_MR_LLB               (0x1UL <<  7)
#define AT91SAMA5D3_SPI_MR_PCS(x)            (x << 16)
#define AT91SAMA5D3_SPI_MR_DLYBCS(x)         (x << 24)

/* SPI_RDR : (SPI Offset: 0x8) Receive Data Register */ 
#define AT91SAMA5D3_SPI_RDR_RD(x)            (x << 0)
#define AT91SAMA5D3_SPI_RDR_RPCS(x)          (x << 16)

/* SPI_TDR : (SPI Offset: 0xC) Transmit Data Register */ 
#define AT91SAMA5D3_SPI_TDR_TD(x)            (x << 0)
#define AT91SAMA5D3_SPI_TDR_TPCS(x)          (x << 16)
#define AT91SAMA5D3_SPI_TDR_LASTXFER         (0x1UL << 24)

/* SPI_SR : (SPI Offset: 0x10) Status Register */ 
#define AT91SAMA5D3_SPI_SR_RDRF              (0x1UL <<  0)
#define AT91SAMA5D3_SPI_SR_TDRE              (0x1UL <<  1)
#define AT91SAMA5D3_SPI_SR_MODF              (0x1UL <<  2)
#define AT91SAMA5D3_SPI_SR_OVRES             (0x1UL <<  3)
#define AT91SAMA5D3_SPI_SR_NSSR              (0x1UL <<  8)
#define AT91SAMA5D3_SPI_SR_TXEMPTY           (0x1UL <<  9)
#define AT91SAMA5D3_SPI_SR_UNDES             (0x1UL << 10)
#define AT91SAMA5D3_SPI_SR_SPIENS            (0x1UL << 16)

/* SPI_IER : (SPI Offset: 0x14) Interrupt Enable Register */ 
/* SPI_IDR : (SPI Offset: 0x18) Interrupt Disable Register */
/* SPI_IMR : (SPI Offset: 0x1c) Interrupt Mask Register */
#define AT91SAMA5D3_SPI_IR_RDRF              (0x1UL <<  0)
#define AT91SAMA5D3_SPI_IR_TDRE              (0x1UL <<  1)
#define AT91SAMA5D3_SPI_IR_MODF              (0x1UL <<  2)
#define AT91SAMA5D3_SPI_IR_OVRES             (0x1UL <<  3)
#define AT91SAMA5D3_SPI_IR_NSSR              (0x1UL <<  8)
#define AT91SAMA5D3_SPI_IR_TXEMPTY           (0x1UL <<  9)
#define AT91SAMA5D3_SPI_IR_UNDES             (0x1UL << 10)

/* SPI_CSR : (SPI Offset: 0x30) Chip Select Register */
#define AT91SAMA5D3_SPI_CSR_CPOL             (0x1UL << 0)
#define AT91SAMA5D3_SPI_CSR_NCPHA            (0x1UL << 1)
#define AT91SAMA5D3_SPI_CSR_CSNAAT           (0x1UL << 2)
#define AT91SAMA5D3_SPI_CSR_CSAAT            (0x1UL << 3)
#define AT91SAMA5D3_SPI_CSR_BITS             (0xFUL << 4)
    #define AT91SAMA5D3_SPI_CSR_BITS_8       (0x0UL << 4)
    #define AT91SAMA5D3_SPI_CSR_BITS_9       (0x1UL << 4)
    #define AT91SAMA5D3_SPI_CSR_BITS_10      (0x2UL << 4)
    #define AT91SAMA5D3_SPI_CSR_BITS_11      (0x3UL << 4)
    #define AT91SAMA5D3_SPI_CSR_BITS_12      (0x4UL << 4)
    #define AT91SAMA5D3_SPI_CSR_BITS_13      (0x5UL << 4)
    #define AT91SAMA5D3_SPI_CSR_BITS_14      (0x6UL << 4)
    #define AT91SAMA5D3_SPI_CSR_BITS_15      (0x7UL << 4)
    #define AT91SAMA5D3_SPI_CSR_BITS_16      (0x8UL << 4)
#define AT91SAMA5D3_SPI_CSR_SCBR(x)          (x << 8)
#define AT91SAMA5D3_SPI_CSR_DLYBS(x)         (x << 16)
#define AT91SAMA5D3_SPI_CSR_DLYBCT(x)        (x << 24)

#define AT91SAMA5D3_SPI_WP_KEY               0x53504900

#endif
