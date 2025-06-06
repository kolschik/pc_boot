/*
 * Copyright 2013, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may not
 * reproduce, modify or distribute this software except in compliance with the
 * License. You may obtain a copy of the License at:
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis, WITHOUT
 * WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as contributors under
 * the License or as licensors under other terms.  Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/
 * for other information.
 */

#ifndef AT91SAMA5D3_DMA_H_
#define AT91SAMA5D3_DMA_H_


/* Register access constants
 * ---------------------------------------------------------- */

#define DMAC_NUM_CONTROLLERS	2
#define DMAC_NUM_CHANNELS		8
#define DMAC_MAX_PERIPHERALS	256
#define MAX_DMA_CHANNELS		(DMAC_NUM_CHANNELS * DMAC_NUM_CONTROLLERS)

// IRQ IDs
#define AT91SAMA5D3_ID_DMAC0  30    /* DMA Controller 0 (DMAC0) */
#define AT91SAMA5D3_ID_DMAC1  31    /* DMA Controller 1 (DMAC1) */

#define AT91SAMA5D3_DMAC_BASE0  0xFFFFE600    /* (DMAC0 ) Base Address */
#define AT91SAMA5D3_DMAC_BASE1  0xFFFFE800    /* (DMAC1 ) Base Address */

#define AT91SAMA5D3_DMAC_SIZE   0x200

/* DMAC (DMA Controller) */
#define DMAC_GCFG			0x000	// DMAC Global Configuration Register
#define DMAC_EN				0x004	// DMAC Enable Register

// These are for peripheral transfers with software handshaking
// instead of hardware directly controlling transfers itself
#define DMAC_SREQ			0x008	// DMAC Software Single Request Register
#define DMAC_CREQ			0x00C	// DMAC Software Chunk Transfer Request Register
#define DMAC_LAST			0x010	// DMAC Software Last Transfer Flag Register

// DMAC Error, Chained Buffer Transfer Completed Interrupt and Buffer Transfer Completed
#define DMAC_EBCIER			0x018	// ...Interrupt Enable register.
#define DMAC_EBCIDR			0x01C	// ...Interrupt Disable register.
#define DMAC_EBCIMR			0x020	// ...Mask Register.
#define DMAC_EBCISR			0x024	// ...Status Register.

// DMAC Channel Handler
#define DMAC_CHER			0x028	// ...Enable Register
#define DMAC_CHDR			0x02C	// ...Disable Register
#define DMAC_CHSR			0x030	// ...Status Register

// Registers duplicated for each channel instance
#define DMAC_SADDR(ch_num)	(0x03C+(ch_num)*(0x28)+(0x0))	// DMAC Channel Source Address Register
#define DMAC_DADDR(ch_num)	(0x03C+(ch_num)*(0x28)+(0x4))	// DMAC Channel Destination Address Register
#define DMAC_DSCR(ch_num)	(0x03C+(ch_num)*(0x28)+(0x8))	// DMAC Channel Descriptor Address Register
#define DMAC_CTRLA(ch_num)	(0x03C+(ch_num)*(0x28)+(0xC))	// DMAC Channel Control A Register
#define DMAC_CTRLB(ch_num)	(0x03C+(ch_num)*(0x28)+(0x10))	// DMAC Channel Control B Register
#define DMAC_CFG(ch_num)	(0x03C+(ch_num)*(0x28)+(0x14))	// DMAC Channel Configuration Register
#define DMAC_SPIP(ch_num)	(0x03C+(ch_num)*(0x28)+(0x18))	// DMAC Channel Source Picture-in-Picture Configuration Register
#define DMAC_DPIP(ch_num)	(0x03C+(ch_num)*(0x28)+(0x1C))	// DMAC Channel Destination Picture-in-Picture Configuration Register

#define DMAC_WPMR			0x1E4	// DMAC Write Protect Mode Register
#define DMAC_WPSR			0x1E8	// DMAC Write Protect Status Register

#define DMAC_WP_KEY			0x444D4100	// Write protect control key


// DMA IRQ status bits
#define DMA_IRQ_CTRL_DICERR(x)	((1 << (x)) << 24)
#define DMA_IRQ_CTRL_ERR(x)		((1 << (x)) << 16)
#define DMA_IRQ_CTRL_CBTC(x)	((1 << (x)) << 8)
#define DMA_IRQ_CTRL_BTC(x)		((1 << (x)) << 0)

// DMAC CHER bits
#define DMAC_CHER_ENA(ch)	(1 << (ch))

// DMAC CHDR
#define DMAC_CHER_DIS(ch)	(1 << (ch))

/* Constants
 * ---------------------------------------------------------- */

/* -------- DMAC_GCFG -------- */
#define DMAC_GCFG_ARB_CFG 	    (0x0 << 4) /* Fixed priority arbitrator */
#define DMAC_GCFG_DICEN 	    (0x0 << 8) /* CRC disabled */

/* -------- DMAC_CTRLA  Channel Control A Register -------- */
#define DMAC_CTRLA_BTSIZE_Pos 	    0
#define DMAC_CTRLA_BTSIZE_Msk 	    (0xffff << DMAC_CTRLA_BTSIZE_Pos) /* Buffer Transfer Size */
#define DMAC_CTRLA_BTSIZE(value) 	((DMAC_CTRLA_BTSIZE_Msk & ((value) << DMAC_CTRLA_BTSIZE_Pos)))

#define   DMAC_CTRLA_SCSIZE_CHK_1 	(0x0 << 16) /* 1 data transferred */
#define   DMAC_CTRLA_SCSIZE_CHK_4 	(0x1 << 16) /* 4 data transferred */
#define   DMAC_CTRLA_SCSIZE_CHK_8 	(0x2 << 16) /* 8 data transferred */
#define   DMAC_CTRLA_SCSIZE_CHK_16 	(0x3 << 16) /* 16 data transferred */
#define   DMAC_CTRLA_SCSIZE_CHK_32 	(0x4 << 16) /* 32 data transferred */
#define   DMAC_CTRLA_SCSIZE_CHK_64 	(0x5 << 16) /* 64 data transferred */
#define   DMAC_CTRLA_SCSIZE_CHK_128	(0x6 << 16) /* 128 data transferred */
#define   DMAC_CTRLA_SCSIZE_CHK_256	(0x7 << 16) /* 256 data transferred */

#define   DMAC_CTRLA_DCSIZE_CHK_1 	(0x0 << 20) /* 1 data transferred */
#define   DMAC_CTRLA_DCSIZE_CHK_4 	(0x1 << 20) /* 4 data transferred */
#define   DMAC_CTRLA_DCSIZE_CHK_8 	(0x2 << 20) /* 8 data transferred */
#define   DMAC_CTRLA_DCSIZE_CHK_16 	(0x3 << 20) /* 16 data transferred */
#define   DMAC_CTRLA_DCSIZE_CHK_32 	(0x4 << 20) /* 32 data transferred */
#define   DMAC_CTRLA_DCSIZE_CHK_64 	(0x5 << 20) /* 64 data transferred */
#define   DMAC_CTRLA_DCSIZE_CHK_128 (0x6 << 20) /* 128 data transferred */
#define   DMAC_CTRLA_DCSIZE_CHK_256 (0x7 << 20) /* 256 data transferred */

#define   DMAC_CTRLA_SRC_WIDTH_BYTE 		(0x0 << 24) 	/* 8-bit width */
#define   DMAC_CTRLA_SRC_WIDTH_HALF_WORD 	(0x1 << 24) 	/* 16-bit width */
#define   DMAC_CTRLA_SRC_WIDTH_WORD 		(0x2 << 24) 	/* 32-bit width */
#define   DMAC_CTRLA_SRC_WIDTH_DWORD 		(0x3 << 24) 	/* 64-bit width */

#define   DMAC_CTRLA_DST_WIDTH_BYTE 		(0x0 << 28) 	/* 8-bit width */
#define   DMAC_CTRLA_DST_WIDTH_HALF_WORD 	(0x1 << 28) 	/* 16-bit width */
#define   DMAC_CTRLA_DST_WIDTH_WORD 		(0x2 << 28) 	/* 32-bit width */
#define   DMAC_CTRLA_DST_WIDTH_DWORD 		(0x3 << 28) 	/* 64-bit width */
#define DMAC_CTRLA_DONE 	(1 << 31)



/* -------- DMAC_CTRLB : DMAC Channel Control B Register -------- */

/* NOTE:
 * Peripherals they must use AHB2
 * Memory can be on and AHB */

#define   DMAC_CTRLB_SIF_AHB_IF0 	(0x0 << 0) 	/* AHB-Lite Interface 0 */
#define   DMAC_CTRLB_SIF_AHB_IF1 	(0x1 << 0) 	/* AHB-Lite Interface 1 */
#define   DMAC_CTRLB_SIF_AHB_IF2 	(0x2 << 0) 	/* AHB-Lite Interface 2 */

#define   DMAC_CTRLB_DIF_AHB_IF0 	(0x0 << 4) 	/* AHB-Lite Interface 0 */
#define   DMAC_CTRLB_DIF_AHB_IF1 	(0x1 << 4) 	/* AHB-Lite Interface 1 */
#define   DMAC_CTRLB_DIF_AHB_IF2 	(0x2 << 4) 	/* AHB-Lite Interface 2 */

#define   DMAC_CTRLB_SRC_DSCR_FETCH_FROM_MEM 	(0x0 << 16) /* Source address is updated when the descriptor is fetched from the memory. */
#define   DMAC_CTRLB_SRC_DSCR_FETCH_DISABLE 	(0x1 << 16) /* Buffer Descriptor Fetch operation is disabled for the source. */

#define   DMAC_CTRLB_DST_DSCR_FETCH_FROM_MEM 	(0x0 << 20) /* Destination address is updated when the descriptor is fetched from the memory. */
#define   DMAC_CTRLB_DST_DSCR_FETCH_DISABLE 	(0x1 << 20) /* Buffer Descriptor Fetch operation is disabled for the destination. */

#define   DMAC_CTRLB_FC_MEM2MEM_DMA_FC 		(0x0 << 21) /* Memory-to-Memory Transfer DMAC is flow controller */
#define   DMAC_CTRLB_FC_MEM2PER_DMA_FC 		(0x1 << 21) /* Memory-to-Peripheral Transfer DMAC is flow controller */
#define   DMAC_CTRLB_FC_PER2MEM_DMA_FC 		(0x2 << 21) /* Peripheral-to-Memory Transfer DMAC is flow controller */
#define   DMAC_CTRLB_FC_PER2PER_DMA_FC 		(0x3 << 21) /* Peripheral-to-Peripheral Transfer DMAC is flow controller */

#define   DMAC_CTRLB_SRC_INCR_INCREMENTING 	(0x0 << 24) /* The source address is incremented */
#define   DMAC_CTRLB_SRC_INCR_DECREMENTING 	(0x1 << 24) /* The source address is decremented */
#define   DMAC_CTRLB_SRC_INCR_FIXED 		(0x2 << 24) /* The source address remains unchanged */

#define   DMAC_CTRLB_DST_INCR_INCREMENTING 	(0x0 << 28) /* The destination address is incremented */
#define   DMAC_CTRLB_DST_INCR_DECREMENTING 	(0x1 << 28) /* The destination address is decremented */
#define   DMAC_CTRLB_DST_INCR_FIXED 		(0x2 << 28) /* The destination address remains unchanged */

#define DMAC_CTRLB_IEN 		(0x1 << 30)

#define   DMAC_CTRLB_AUTO_DISABLE 	(0x0 << 31) /* Automatic multiple buffer transfer is disabled. */
#define   DMAC_CTRLB_AUTO_ENABLE 	(0x1 << 31) /* Automatic multiple buffer transfer is enabled. This bit enables replay mode or contiguous mode when several buffers are transferred. */



/* -------- DMAC_CFG : DMAC Channel Configuration Register -------- */
#define DMAC_CFG_AHB_PROT		24

#define   DMAC_CFG_SRC_REP_CONTIGUOUS_ADDR 	(0x0 << 8) /* When automatic mode is activated, source address is contiguous between two buffers. */
#define   DMAC_CFG_SRC_REP_RELOAD_ADDR 		(0x1 << 8) /* When automatic mode is activated, the source address and the control register are reloaded from previous transfer. */

#define   DMAC_CFG_SRC_H2SEL_SW 			(0x0 << 9) /* Software handshaking triggers transfer request. */
#define   DMAC_CFG_SRC_H2SEL_HW 			(0x1 << 9) /* Hardware handshaking interface triggers transfer request. */

#define   DMAC_CFG_DST_REP_CONTIGUOUS_ADDR 	(0x0 << 12) /* When automatic mode is activated, destination address is contiguous between two buffers. */
#define   DMAC_CFG_DST_REP_RELOAD_ADDR 		(0x1 << 12) /* When automatic mode is activated, the destination and the control register are reloaded from the previous transfer. */

#define   DMAC_CFG_DST_H2SEL_SW 			(0x0 << 13) /* Software handshaking triggers transfer request. */
#define   DMAC_CFG_DST_H2SEL_HW 			(0x1 << 13) /* Hardware handshaking interface triggers a transfer request. */

#define   DMAC_CFG_SOD_DISABLE 				(0x0 << 16) /* STOP ON DONE disabled, the descriptor fetch operation ignores DONE Field of CTRLA register. */
#define   DMAC_CFG_SOD_ENABLE 				(0x1 << 16) /* STOP ON DONE activated, the DMAC module is automatically disabled if DONE FIELD is set to 1. */

#define   DMAC_CFG_LOCK_IF_DISABLE 			(0x0 << 20) /* Interface Lock capability is disabled */
#define   DMAC_CFG_LOCK_IF_ENABLE 			(0x1 << 20) /* Interface Lock capability is enabled */

#define   DMAC_CFG_LOCK_B_DISABLE 			(0x0 << 21) /* AHB Bus Locking capability is disabled. */
#define   DMAC_CFG_LOCK_B_ENABLE 			(0x1 << 21) /* AHB Bus Locking capability is enabled. */

#define   DMAC_CFG_LOCK_IF_L_CHUNK 			(0x0 << 22) /* The Master Interface Arbiter is locked by the channel x for a chunk transfer. */
#define   DMAC_CFG_LOCK_IF_L_BUFFER 		(0x1 << 22) /* The Master Interface Arbiter is locked by the channel x for a buffer transfer. */

#define   DMAC_CFG_FIFOCFG_ALAP_CFG 		(0x0 << 28) /* The largest defined length AHB burst is performed on the destination AHB interface. */
#define   DMAC_CFG_FIFOCFG_HALF_CFG 		(0x1 << 28) /* When half FIFO size is available/filled, a source/destination request is serviced. */
#define   DMAC_CFG_FIFOCFG_ASAP_CFG 		(0x2 << 28) /* When there is enough space/data available to perform a single AHB access, then the request is serviced. */

#define DMAC_CFG_DST_PER_MSB	14
#define DMAC_CFG_SRC_PER_MSB	10
#define DMAC_CFG_DST_PER		4
#define DMAC_CFG_SRC_PER		0

// Flag to mark as a DMA identifier
#define DMA_ID_MASK			(0x3f)
#define DMA_ID				(1 << 16)
#define DMAC_0_ID			(0 << 8)
#define DMAC_1_ID			(1 << 8)


// Memory to memory transfers can occur on any
// DMA controller with a free channel.
#define DMA_ID_MEM			(DMA_ID | DMA_ID_MASK)

/* DMAC_0
 * --------------------------- */
#define DMA_ID_HSMCIO		(DMA_ID | DMAC_0_ID | 0)
#define DMA_ID_SPI0_TX		(DMA_ID | DMAC_0_ID | 1)
#define DMA_ID_SPI0_RX		(DMA_ID | DMAC_0_ID | 2)
#define DMA_ID_USART0_TX	(DMA_ID | DMAC_0_ID | 3)
#define DMA_ID_USART0_RX	(DMA_ID | DMAC_0_ID | 4)
#define DMA_ID_USART1_TX	(DMA_ID | DMAC_0_ID | 5)
#define DMA_ID_USART1_RX	(DMA_ID | DMAC_0_ID | 6)
#define DMA_ID_TWI0_TX		(DMA_ID | DMAC_0_ID | 7)
#define DMA_ID_TWI0_RX		(DMA_ID | DMAC_0_ID | 8)
#define DMA_ID_TWI1_TX		(DMA_ID | DMAC_0_ID | 9)
#define DMA_ID_TWI1_RX		(DMA_ID | DMAC_0_ID | 10)
#define DMA_ID_UART0_TX		(DMA_ID | DMAC_0_ID | 11)
#define DMA_ID_UART0_RX		(DMA_ID | DMAC_0_ID | 12)
#define DMA_ID_SSC0_TX		(DMA_ID | DMAC_0_ID | 13)
#define DMA_ID_SSC0_RX		(DMA_ID | DMAC_0_ID | 14)
#define DMA_ID_SMD_TX		(DMA_ID | DMAC_0_ID | 15)
#define DMA_ID_SMD_RX		(DMA_ID | DMAC_0_ID | 16)
#define DMAC_0_LAST_CHANNEL		16

/* DMAC_1
 * --------------------------- */
#define DMA_ID_HSMCI1		(DMA_ID | DMAC_1_ID | 0)
#define DMA_ID_HSMCI2		(DMA_ID | DMAC_1_ID | 1)
#define DMA_ID_ADC_RX		(DMA_ID | DMAC_1_ID | 2)
#define DMA_ID_SSC1_TX		(DMA_ID | DMAC_1_ID | 3)
#define DMA_ID_SSC1_RX		(DMA_ID | DMAC_1_ID | 4)
#define DMA_ID_UART1_TX		(DMA_ID | DMAC_1_ID | 5)
#define DMA_ID_UART1_RX		(DMA_ID | DMAC_1_ID | 6)
#define DMA_ID_USART2_TX	(DMA_ID | DMAC_1_ID | 7)
#define DMA_ID_USART2_RX	(DMA_ID | DMAC_1_ID | 8)
#define DMA_ID_USART3_TX	(DMA_ID | DMAC_1_ID | 9)
#define DMA_ID_USART3_RX	(DMA_ID | DMAC_1_ID | 10)
#define DMA_ID_TWI2_TX		(DMA_ID | DMAC_1_ID | 11)
#define DMA_ID_TWI2_RX		(DMA_ID | DMAC_1_ID | 12)
#define DMA_ID_DBGU_TX		(DMA_ID | DMAC_1_ID | 13)
#define DMA_ID_DBGU_RX		(DMA_ID | DMAC_1_ID | 14)
#define DMA_ID_SPI1_TX		(DMA_ID | DMAC_1_ID | 15)
#define DMA_ID_SPI1_RX		(DMA_ID | DMAC_1_ID | 16)
#define DMA_ID_SHA_TX		(DMA_ID | DMAC_1_ID | 17)
#define DMA_ID_AES_TX		(DMA_ID | DMAC_1_ID | 18)
#define DMA_ID_AES_RX		(DMA_ID | DMAC_1_ID | 19)
#define DMA_ID_TDES_TX		(DMA_ID | DMAC_1_ID | 20)
#define DMA_ID_TDES_RX		(DMA_ID | DMAC_1_ID | 21)
#define DMAC_1_LAST_CHANNEL		21


/* IRQ status flags returned by the callback */
#define DMA_IRQ_DICERR	(1 << 3)	// Descriptor Integrity Check Error
#define DMA_IRQ_ERR		(1 << 2)	// AHB Read or Write Error Access.
#define DMA_IRQ_CBTC	(1 << 1)	// Chained buffer has terminated. LLI Fetch operation is disabled
#define DMA_IRQ_BTC		(1 << 0)	// Buffer transfer has terminated

#endif
