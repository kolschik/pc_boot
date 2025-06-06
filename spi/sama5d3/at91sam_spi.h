#ifndef AT91SAM_SPI_H_
#define AT91SAM_SPI_H_


#define AT91SAM_SPI_BASE0              0xfffc8000UL
#define AT91SAM_SPI_BASE1              0xfffcc000UL
#define AT91SAM_SPI_IRQ0               12
#define AT91SAM_SPI_IRQ1               13
#define AT91SAM_SPI_BASE_CLK           133333333UL        /* 100 MHz */

#define AT91SAM_SPI_SIZE                0x200
/*
 * Register offsets from register base
 */
#define AT91SAM_SPI_CR_OFF              0x00
#define AT91SAM_SPI_MR_OFF              0x04
#define AT91SAM_SPI_RDR_OFF             0x08
#define AT91SAM_SPI_TDR_OFF             0x0C
#define AT91SAM_SPI_SR_OFF              0x10
#define AT91SAM_SPI_IER_OFF             0x14
#define AT91SAM_SPI_IDR_OFF             0x18
#define AT91SAM_SPI_IMR_OFF             0x1C
#define AT91SAM_SPI_CSRx_OFF(x)         (0x30 + (x * 4))
#define AT91SAM_SPI_CSR0_OFF            0x30
#define AT91SAM_SPI_CSR1_OFF            0x34
#define AT91SAM_SPI_CSR2_OFF            0x38
#define AT91SAM_SPI_CSR3_OFF            0x3c


/*
 * SPI Control Register bits (SPI_CR)
 */
#define AT91SAM_SPI_CR_LASTXFER         (1UL << 24)
#define AT91SAM_SPI_CR_SWRST            (1UL << 7)
#define AT91SAM_SPI_CR_SPIDIS           (1UL << 1)
#define AT91SAM_SPI_CR_SPIEN            (1UL << 0)

/*
 * SPI Mode Register bits (SPI_MR)
 */
#define AT91SAM_SPI_MR_DLYBCS(val)      ((val & 0xff) << 24)
#define AT91SAM_SPI_MR_PCS(val)         ((val & 0xf)  << 16)
#define AT91SAM_SPI_MR_LLB              (1UL << 7)
#define AT91SAM_SPI_MR_MODFDIS          (1UL << 4)
#define AT91SAM_SPI_MR_PCSDEC           (1UL << 2)
#define AT91SAM_SPI_MR_PS               (1UL << 1)
#define AT91SAM_SPI_MR_MSTR             (1UL << 0)

/*
 * SPI Receive Data Register bits (SPI_RDR)
#define AT91SAM_SPI_RDR_PCS(val)        ((val & 0xf) << 16)
#define AT91SAM_SPI_RDR_RD              (0xffffUL << 0)
*/

/*
 * SPI Transmit Data Register bits (SPI_TDR)
 */
#define AT91SAM_SPI_TDR_LASTXFER        (1UL << 24)
#define AT91SAM_SPI_TDR_PCS(val)        ((val & 0xf) << 16)
#define AT91SAM_SPI_TDR_TD              (1UL << )

/*
 * SPI Status Register bits (SPI_SR)
 * Also used for SPI Interrupt Enable, Disable & Mask Registers (SPI_IER
 * , SPI_IDR & SPI_IMR).
 * 
 * AT91SAM_SPI_SR_SPENS bit field is only used for the SPI_SR.
 */
#define AT91SAM_SPI_SR_SPENS            (1UL << 16)
#define AT91SAM_SPI_SR_TXEMPTY          (1UL << 9)
#define AT91SAM_SPI_SR_NSSR             (1UL << 8)
#define AT91SAM_SPI_SR_TXBUFE           (1UL << 7)
#define AT91SAM_SPI_SR_RXBUFF           (1UL << 6)
#define AT91SAM_SPI_SR_ENDTX            (1UL << 5)
#define AT91SAM_SPI_SR_ENDRX            (1UL << 4)
#define AT91SAM_SPI_SR_OVRES            (1UL << 3)
#define AT91SAM_SPI_SR_MODF             (1UL << 2)
#define AT91SAM_SPI_SR_TDRE             (1UL << 1)
#define AT91SAM_SPI_SR_RDRF             (1UL << 0)

/*
 * SPI Chip Select Register bits (SPI_CSR)
 */
#define AT91SAM_SPI_CSR_DLYBCT(val)     (((val) & 0xff) << 24)
#define AT91SAM_SPI_CSR_DLYBS(val)      (((val) & 0xff) << 16)
#define AT91SAM_SPI_CSR_SCBR(val)       (((val) & 0xff) << 8)
#define AT91SAM_SPI_CSR_BITS(val)       (((val) & 0xf)  << 4)
#define AT91SAM_SPI_CSR_CSAAT           (1UL << 3)
#define AT91SAM_SPI_CSR_NCPHA           (1UL << 1)
#define AT91SAM_SPI_CSR_CPOL            (1UL << 0)


#define AT91SAM_PDC_OFFSET 	0x100

#define AT91SAM_PDC_SIZE	0x28

#define AT91SAM_PDC_RPR 	0x00
#define AT91SAM_PDC_RCR 	0x04
#define AT91SAM_PDC_TPR 	0x08
#define AT91SAM_PDC_TCR 	0x0C
#define AT91SAM_PDC_RNPR 	0x10
#define AT91SAM_PDC_RNCR 	0x14
#define AT91SAM_PDC_TNPR 	0x18
#define AT91SAM_PDC_TNCR 	0x1C

#define AT91SAM_PDC_PTCR 	0x20
#define AT91SAM_PDC_PTSR 	0x24

#define AT91SAM_PDC_TXTEN  (1<<8)
#define AT91SAM_PDC_TXTDIS  (1<<9)

#define AT91SAM_PDC_RXTEN  (1<<0)
#define AT91SAM_PDC_RXTDIS  (1<<1)


#endif /*AT91SAM_SPI_H_*/
