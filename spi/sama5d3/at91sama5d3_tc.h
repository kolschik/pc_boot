#ifndef AT91SAMA5D3_TC_H_
#define AT91SAMA5D3_TC_H_

/*
 * TC base
 */
#define AT91SAMA5D3_TC_BASE0         0xF0010000UL
#define AT91SAMA5D3_TC_BASE1         0xF8014000UL
#define AT91SAMA5D3_TC_SIZE          0x100
#define AT91SAMA5D3_ID_TC0           26
#define AT91SAMA5D3_ID_TC1           27

/* Timer Counter registers offsets */
#define AT91SAMA5D3_TC_CH(ch)       (ch * 0x40)
#define AT91SAMA5D3_TC_CH_CCR(ch)   (ch * 0x40 + 0x00)
#define AT91SAMA5D3_TC_CH_CMR(ch)   (ch * 0x40 + 0x04)
#define AT91SAMA5D3_TC_CH_CV(ch)    (ch * 0x40 + 0x10)
#define AT91SAMA5D3_TC_CH_RA(ch)    (ch * 0x40 + 0x14)
#define AT91SAMA5D3_TC_CH_RB(ch)    (ch * 0x40 + 0x18)
#define AT91SAMA5D3_TC_CH_RC(ch)    (ch * 0x40 + 0x1C)
#define AT91SAMA5D3_TC_CH_SR(ch)    (ch * 0x40 + 0x20)
#define AT91SAMA5D3_TC_CH_IER(ch)   (ch * 0x40 + 0x24)
#define AT91SAMA5D3_TC_CH_IDR(ch)   (ch * 0x40 + 0x28)
#define AT91SAMA5D3_TC_CH_IMR(ch)   (ch * 0x40 + 0x2C)
#define AT91SAMA5D3_TC_BCR          0xC0
#define AT91SAMA5D3_TC_BMR          0xC4

#define AT91SAMA5D3_TC_TC0XC0S_NONE ( 1 << 0 )
#define AT91SAMA5D3_TC_TC0XC1S_NONE ( 1 << 2 )
#define AT91SAMA5D3_TC_TC0XC2S_NONE ( 1 << 4 )

#define AT91SAMA5D3_TC_ACPC_TOGGLE 		    ( 3 << 18 )
#define AT91SAMA5D3_TC_WAVE 				( 1 << 15 )
#define AT91SAMA5D3_TC_CPCTRG			 	( 1 << 14 )
#define AT91SAMA5D3_TC_WAVE_SEL_UP_AUTO	    ( 3 << 13 )
#define AT91SAMA5D3_TC_CLKS_TIMER_DIV8 	    ( 1 << 0 )
#define AT91SAMA5D3_TC_CLKS_TIMER_DIV32 	( 2 << 0 )
#define AT91SAMA5D3_TC_CLKS_TIMER_DIV128 	( 3 << 0 )

#define AT91SAMA5D3_TC_CPCS     ( 1 << 4 )
#define AT91SAMA5D3_TC_SWTRG    ( 1 << 2 )
#define AT91SAMA5D3_TC_CLKEN    ( 1 << 0 )


#endif
