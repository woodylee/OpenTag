/* Copyright 2009-2011 JP Norair
  *
  * Licensed under the OpenTag License, Version 1.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  * http://www.indigresso.com/wiki/doku.php?id=opentag:license_1_0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  */
/**
  * @file       /otradio/spirit1/SPIRIT1_STM32L1xx.h
  * @author     JP Norair
  * @version    R100
  * @date       4 Jan 2013
  * @brief      SPIRIT1 transceiver interface implementation for STM32L1xx
  * @ingroup    SPIRIT1
  *
  ******************************************************************************
  */

#include "OT_platform.h"
#if 1 //defined(PLATFORM_STM32L1xx)

#include "OT_utils.h"
#include "OT_types.h"
#include "OT_config.h"


#include "SPIRIT1_interface.h"

//For test
#include "OTAPI.h"


#ifdef RADIO_DB_ATTENUATION
#   define _ATTEN_DB    RADIO_DB_ATTENUATION
#else
#   define _ATTEN_DB    0
#endif




/// RF IRQ GPIO Macros:
#if (RADIO_IRQ0_SRCLINE < 5)
#   define _RFIRQ0  (EXTI0_IRQn + RADIO_IRQ0_SRCLINE)
#elif (RADIO_IRQ0_SRCLINE < 10)
#   define _EXTI9_5_USED
#   define _RFIRQ0  (EXTI9_5_IRQn)
#else
#   define _EXTI15_10_USED
#   define _RFIRQ0  (EXTI15_10_IRQn)
#endif

#if (RADIO_IRQ1_SRCLINE < 5)
#   define _RFIRQ1  (EXTI0_IRQn + RADIO_IRQ1_SRCLINE)
#elif ((RADIO_IRQ0_SRCLINE < 10) && !defined(_EXTI9_5_USED))
#   define _EXTI9_5_USED
#   define _RFIRQ1  (EXTI9_5_IRQn)
#elif !defined(_EXTI15_10_USED)
#   define _EXTI15_10_USED
#   define _RFIRQ1  (EXTI15_10_IRQn)
#endif

#if (RADIO_IRQ2_SRCLINE < 5)
#   define _RFIRQ2  (EXTI0_IRQn + RADIO_IRQ2_SRCLINE)
#elif ((RADIO_IRQ2_SRCLINE < 10) && !defined(_EXTI9_5_USED))
#   define _EXTI9_5_USED
#   define _RFIRQ2  (EXTI9_5_IRQn)
#elif !defined(_EXTI15_10_USED)
#   define _EXTI15_10_USED
#   define _RFIRQ2  (EXTI15_10_IRQn)
#endif

#if (RADIO_IRQ3_SRCLINE < 5)
#   define _RFIRQ3  (EXTI0_IRQn + RADIO_IRQ3_SRCLINE)
#elif ((RADIO_IRQ3_SRCLINE < 10) && !defined(_EXTI9_5_USED))
#   define _EXTI9_5_USED
#   define _RFIRQ3  (EXTI9_5_IRQn)
#elif !defined(_EXTI15_10_USED)
#   define _EXTI15_10_USED
#   define _RFIRQ3  (EXTI15_10_IRQn)
#endif



/// SPI Bus Macros: 
/// Most are straightforward, but take special note of the clocking macros.
/// On STM32L, the peripheral clock must be enabled for the peripheral to work.
/// There are clock-enable bits for active and low-power mode.  Both should be
/// enabled before SPI usage, and both disabled afterward.
#if (RADIO_SPI_ID == 1)
#   define _SPICLK          (PLATFORM_HSCLOCK_HZ/BOARD_PARAM_APB2CLKDIV)
#   define _UART_IRQ        SPI1_IRQn
#   define _DMA_ISR         platform_dma1ch2_isr
#   define _DMARX           DMA1_Channel2
#   define _DMATX           DMA1_Channel3
#   define _DMARX_IRQ       DMA1_Channel2_IRQn
#   define _DMATX_IRQ       DMA1_Channel3_IRQn
#   define _DMARX_IFG       (0x2 << (4*(2-1)))
#   define _DMATX_IFG       (0x2 << (4*(3-1)))
#   define __SPI_CLKON()    (RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#   define __SPI_CLKOFF()   (RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN)

#elif (RADIO_SPI_ID == 2)
#   define _SPICLK          (PLATFORM_HSCLOCK_HZ/BOARD_PARAM_APB1CLKDIV)
#   define _UART_IRQ        SPI2_IRQn
#   define _DMA_ISR         platform_dma1ch4_isr
#   define _DMARX           DMA1_Channel4
#   define _DMATX           DMA1_Channel5
#   define _DMARX_IRQ       DMA1_Channel4_IRQn
#   define _DMATX_IRQ       DMA1_Channel5_IRQn
#   define _DMARX_IFG       (0x2 << (4*(4-1)))
#   define _DMATX_IFG       (0x2 << (4*(5-1)))
#   define __SPI_CLKON()    (RCC->APB1ENR |= RCC_APB1ENR_SPI2EN)
#   define __SPI_CLKOFF()   (RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN)

#else
#   error "RADIO_SPI_ID is misdefined, must be 1 or 2."

#endif

#define __DMA_CLEAR_IFG()   (DMA1->IFCR = _DMARX_IFG)
#define __DMA_CLEAR_IRQ()   (NVIC->ICPR[(ot_u32)(_DMARX_IRQ>>5)] = (1 << ((ot_u32)_DMARX_IRQ & 0x1F)))
#define __DMA_TRIGGER()     do { _DMARX->CCR |= DMA_CCR1_EN; _DMATX->CCR |= DMA_CCR1_EN; } while(0)

#define __SPI_CS_HIGH()     RADIO_SPICS_PORT->BSRRL = (ot_u32)RADIO_SPICS_PIN
#define __SPI_CS_LOW()      RADIO_SPICS_PORT->BRR   = (ot_u32)RADIO_SPICS_PIN
#define __SPI_CS_ON()       __SPI_CS_LOW()
#define __SPI_CS_OFF()      __SPI_CS_HIGH()
#define __SPI_ENABLE()      RADIO_SPI->CR1 |= SPI_CR1_SPE
#define __SPI_DISABLE()     RADIO_SPI->CR1 &= ~SPI_CR1_SPE
#define __SPI_GET(VAL)      VAL = RADIO_SPI->DR
#define __SPI_PUT(VAL)      RADIO_SPI->DR = VAL




/** Module Data for radio driver interface <BR>
  * ========================================================================
  */
spirit1_struct spirit1;




/** Embedded Interrupts <BR>
  * ========================================================================<BR>
  * None: The Radio core only uses the GPIO interrupts, which must be handled
  * universally in platform_isr_STM32L1xx.c due to the multiplexed nature of 
  * the EXTI system.  However, the DMA RX complete EVENT is used by the SPI 
  * engine.  EVENTS are basically a way to sleep where you would otherwise
  * need to use busywait loops.  ARM Cortex-M takes all 3 points.
  */




/** Basic Control Functions <BR>
  * ========================================================================
  */
void spirit1_coredump() {
///debugging function to dump-out register values of RF core (not all are used)
    ot_u8 i;

    for (i=0; i<0xFF; i++) {
        ot_u8 label[]   = { 'R', 'E', 'G', '_', 0, 0 };
        ot_u8 regval    = spirit1_read(i);

        otutils_bin2hex(&i, &label[4], 1);
        ///@todo wait for mpipe here (?)
        otapi_log_msg(MSG_raw, 6, 1, label, &regval);
    }
}



void spirit1_load_defaults() {
/// The data ordering is: WRITE LENGTH, WRITE HEADER (0), START ADDR, VALUES
/// Ignore registers that are set later, are unused, or use the hardware default values.
    static const ot_u8 spirit1_defaults[] = {
        3,  0,  0x01,   DRF_ANA_FUNC_CONF0, 
       10,  0,  0x07,   DRF_IF_OFFSET_ANA, DRF_SYNT3, DRF_SYNT2, DRF_SYNT1, 
                        DRF_SYNT0, DRF_CHSPACE, DRF_IF_OFFSET_DIG,
        3,  0,  0xB4,   DRF_XO_RCO_TEST,
        4,  0,  0x9E,   DRF_SYNTH_CONFIG1, DRF_SYNTH_CONFIG0,
        3,  0,  0x18,   DRF_PAPOWER0,
        3,  0,  0x1C,   DRF_FDEV0, 
        3,  0,  0x27,   DRF_ANT_SELECT_CONF, 
        3,  0,  0x3A,   DRF_QI,
        3,  0,  0x4F,   DRF_PCKT_FLT_OPTIONS,
        0   //Terminating 0
    };
    
    ot_u8* cursor;
    cursor = (ot_u8*)spirit1_defaults;

    while (*cursor != 0) {
        ot_u8 cmd_len   = *cursor++;
        ot_u8* cmd      = cursor;
        cursor         += cmd_len;
        spirit1_spibus_io(cmd_len, 0, cmd);
    }

    // Debugging Test to make sure data was written (look at first write block)
//    {
//        volatile ot_u8 test;
//        ot_u8 i;
//        for (i=0x01; i<=0x0D; ++i) {
//            test = spirit1_read(i);
//        }
//    }
}



void spirit1_shutdown() {
/// Raise the Shutdown Line
    RADIO_SDN_PORT->BSRRL = RADIO_SDN_PIN;
}


void spirit1_reset() {
/// Turn-off interrupts, send Reset strobe, and wait for reset to finish.
    spirit1_int_turnoff(RFI_ALL);
    spirit1_strobe(STROBE(SRES));
    spirit1_waitforreset();
}


void spirit1_waitforreset() {
/// Wait for nPOR signal to go low.  
/// Save non-blocking implementation for a rainy day.
    while (RADIO_IRQ0_PORT->IDR & RADIO_IRQ0_PIN);
}


ot_u16 spirit1_getstatus() {
/// Status is sent during every SPI access, so refresh it manually by doing a
/// dummy read, and then returning the global status data that is obtained 
/// during the read process.
    spirit1_read(0);
    return spirit1.status;
}



ot_u16 spirit1_mcstate() { 
    static const ot_u8 cmd[2] = { 1, RFREG(MC_STATE1) };
    spirit1_spibus_io(2, 2, (ot_u8*)cmd);
    return (ot_u16)*((ot_u16*)spirit1.busrx);
}


ot_u8   spirit1_ldctime()       { return spirit1_read( RFREG(TIMERS2) ); }
ot_u8   spirit1_ldcrtime()      { return spirit1_read( RFREG(TIMERS0) ); }
ot_u8   spirit1_rxtime()        { return spirit1_read( RFREG(TIMERS4) ); }
ot_u8   spirit1_rxbytes()       { return spirit1_read( RFREG(LINEAR_FIFO_STATUS0) ); }
ot_u8   spirit1_txbytes()       { return spirit1_read( RFREG(LINEAR_FIFO_STATUS1) ); }
ot_u8   spirit1_rssi()          { return spirit1_read( RFREG(RSSI_LEVEL) ); }



void spirit1_linkinfo(spirit1_link* link) {
    static const ot_u8 cmd[2] = { 0x01, RFREG(LINK_QUALIF2) };
    
    // Do Read command on LINK_QUALIF[2:0] and store results in link structure
    spirit1_spibus_io(2, 3, (ot_u8*)cmd);
    
    // Convert 3 byte SPIRIT1 output into 4 byte data structure
    link->pqi   = spirit1.busrx[0];
    link->sqi   = spirit1.busrx[1] & ~0x80;
    link->lqi   = spirit1.busrx[2] >> 4;
    link->agc   = spirit1.busrx[2] & 0x0f;
}









/** Bus interface (SPI + 2x GPIO) <BR>
  * ========================================================================
  */

void spirit1_init_bus() {
/// @note platform_init_periphclk() should have alread enabled RADIO_SPI clock
/// and GPIO clocks
    
    ///0. Assure that Shutdown Line is Low
    RADIO_SDN_PORT->BSRRH = RADIO_SDN_PIN;
    
    ///1. Set-up DMA to work with SPI.  The DMA is bound to the SPI and it is
    ///   used for Duplex TX+RX.  The DMA RX Channel is used as an EVENT.  The
    ///   STM32L can do in-context naps using EVENTS.  To enable the EVENT, we
    ///   enable the DMA RX interrupt bit, but not the NVIC.
    _DMARX->CCR     = DMA_CCR1_MINC | DMA_CCR1_PL_VHI | DMA_CCR1_TCIE;
    _DMARX->CMAR    = (ot_u32)&spirit1.status;
    _DMARX->CPAR    = (ot_u32)&RADIO_SPI->DR;
    _DMATX->CCR     = DMA_CCR1_DIR | DMA_CCR1_MINC | DMA_CCR1_PL_VHI;
    _DMATX->CPAR    = (ot_u32)&RADIO_SPI->DR;
    
    // Don't enable NVIC, because we want an EVENT, not an interrupt.
    //NVIC->IP[(ot_u32)_DMARX_IRQ]        = PLATFORM_NVIC_RF_GROUP;
    //NVIC->ISER[(ot_u32)(_DMARX_IRQ>>5)] = (1 << ((ot_u32)_DMARX_IRQ & 0x1F));


    ///2. Set-up SPI peripheral for Master Mode, Full Duplex, DMAs used for TRX.
    ///   Configure SPI clock divider to make sure that clock rate is not above
    ///   10MHz.  SPI clock = bus clock/2, so only systems that have HSCLOCK
    ///   above 20MHz need to divide.
#   if (_SPICLK > 20000000)
#       define _SPI_DIV (1<<3)
#   else
#       define _SPI_DIV (0<<3)
#   endif
    RADIO_SPI->CR1  = SPI_CR1_MSTR | _SPI_DIV;
    RADIO_SPI->CR2  = SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN;
    
    
    ///3. Wait for rest of bus to stabilize on power-up, then put it to sleep.
    ///   SPIRIT1 can do SPI while in sleep mode.  "Sleep" mode is either 
    ///   implemented as STANDBY or SLEEP, per SPIRIT1 specs, depending on the
    ///   driver.  Usually, it is STANDBY.  Both can take SPI I/O.
    spirit1_waitforreset();
    radio_sleep();
    
    
    ///4. Managing SPIRIT1 GPIOs as sources of External Interrupt:
    ///   The GPIOs should be bound to EXTI lines in BOARD_EXTI_STARTUP(), 
    ///   defined in the board support header and run in platform_init().
    ///   For the current impl, all the GPIOs are rising-edge activators
    EXTI->PR    =  RFI_ALL;         //clear flag bits
    EXTI->IMR  &= ~RFI_ALL;         //clear interrupt enablers
    EXTI->EMR  &= ~RFI_ALL;         //clear event enablers
    EXTI->FTSR &= ~RFI_ALL;         // For this version, all GPIOs are rising edge
    EXTI->RTSR |= RFI_ALL;

    NVIC->IP[(uint32_t)_RFIRQ0]         = PLATFORM_NVIC_RF_GROUP;
    NVIC->ISER[((uint32_t)_RFIRQ0>>5)]  = (1 << ((uint32_t)_RFIRQ0 & 0x1F));
#   ifdef _RFIRQ1
    NVIC->IP[(uint32_t)_RFIRQ1]         = PLATFORM_NVIC_RF_GROUP;
    NVIC->ISER[((uint32_t)_RFIRQ1>>5)]  = (1 << ((uint32_t)_RFIRQ1 & 0x1F));
#   endif
#   ifdef _RFIRQ2
    NVIC->IP[(uint32_t)_RFIRQ2]         = PLATFORM_NVIC_RF_GROUP;
    NVIC->ISER[((uint32_t)_RFIRQ2>>5)]  = (1 << ((uint32_t)_RFIRQ2 & 0x1F));
#   endif
#   ifdef _RFIRQ3
    NVIC->IP[(uint32_t)_RFIRQ3]         = PLATFORM_NVIC_RF_GROUP;
    NVIC->ISER[((uint32_t)_RFIRQ3>>5)]  = (1 << ((uint32_t)_RFIRQ3 & 0x1F));
#   endif
}



void spirit1_spibus_wait() {
/// Blocking wait for SPI bus to be over
    while (RADIO_SPI->SR & SPI_SR_BSY);
}




void spirit1_spibus_io(ot_u8 cmd_len, ot_u8 resp_len, ot_u8* cmd) {
///@note BOARD_DMA_CLKON() must be defined in the board support header as a 
/// macro or inline function.  As the board may be using DMA for numerous
/// peripherals, we cannot assume in this module if it is appropriate to turn-
/// off the DMA for all other modules.

    BOARD_DMA_CLKON();
    __SPI_CLKON();
    __SPI_ENABLE();
    
    ///Flip CS
    __SPI_CS_ON();
    
    /// Set-up DMA, and trigger it.  TX goes out from parameter.  RX goes into
    /// module buffer.  If doing a read, the garbage data getting duplexed onto
    /// TX doesn't affect the SPIRIT1.  If doing a write, simply disregard the 
    /// RX duplexed data.
    _DMARX->CNDTR = (cmd_len+resp_len);
    _DMATX->CMAR  = (ot_u32)cmd;
    __DMA_CLEAR_IFG();
    __DMA_TRIGGER();
    
    /// Use Cortex-M WFE (Wait For Event) to hold until DMA is complete.  This
    /// is the CM way of doing a busywait loop, but turning off the CPU core.
    /// The while loop is for safety purposes, in case another event comes.
    do {
        __WFE();
    } while((DMA1->ISR & _DMARX_IFG) == 0);
    __DMA_CLEAR_IRQ();
    __DMA_CLEAR_IFG();

    /// Turn-off and disable SPI to save energy
    __SPI_CS_OFF();
    __SPI_DISABLE();
    __SPI_CLKOFF();
    BOARD_DMA_CLKOFF();
}


void spirit1_strobe(ot_u8 strobe) {
    ot_u8 cmd[2];
    cmd[0]  = 0x80;
    cmd[1]  = strobe;
    spirit1_spibus_io(2, 0, &strobe);
}

ot_u8 spirit1_read(ot_u8 addr) {
    ot_u8 cmd[2];
    cmd[0]  = 1;
    cmd[1]  = addr;
    spirit1_spibus_io(2, 1, cmd);
    return spirit1.busrx[0];
}

void spirit1_burstread(ot_u8 start_addr, ot_u8 length, ot_u8* data) {
    ot_u8 cmd[2];
    cmd[0]  = 1;
    cmd[1]  = start_addr;
    spirit1_spibus_io(2, length, (ot_u8*)cmd);
    platform_memcpy(data, spirit1.busrx, length);
}

void spirit1_write(ot_u8 addr, ot_u8 data) {
    ot_u8 cmd[3];
    cmd[0]  = 0;
    cmd[1]  = addr;
    cmd[2]  = data;
    spirit1_spibus_io(3, 0, cmd);
}

void spirit1_burstwrite(ot_u8 start_addr, ot_u8 length, ot_u8* cmd_data) {
    cmd_data[0] = 0;
    cmd_data[1] = start_addr;
    spirit1_spibus_io((2+length), 0, cmd_data);
}












/** Advanced Configuration <BR>
  * ========================================================================<BR>
  */

ot_int spirit1_calc_rssi(ot_u8 encoded_value) {
/// From SPIRIT1 datasheet: "The measured power is reported in steps of half-dB
/// from 0 to 255 and is offset in such a way that -120 dBm corresponds to 
/// about 20."  In other words, it is linear: { 0 = -130dBm, 255 = -2.5dBm }.
/// This function turns the coded value into a normal, signed int.

/// @note In future implementations there may be a selection to allow OpenTag
///       to use half-dBm precision in all areas of the system, but for now it
///       uses whole-dBm (1dBm) precision.  Most antennas have >1dBm variance,
///       (often >2dBm) so unless you have an advanced subsampling algorithm 
///       for range-finding -- probably requiring a kalman filter in the
///       location engine -- using half-dBm will not yield an improvement.

    ot_int rssi_val;
    rssi_val    = (ot_int)encoded_value;    // Convert to signed int
    rssi_val  >>= 1;                        // Make whole-dBm (divide by 2)
    rssi_val   -= 130;                      // Apply 130 dBm offset
    return rssi_val;
}


ot_u8 spirit1_calc_rssithr(ot_u8 input) {
/// SPIRIT1 treats RSSI thresholding through the normal RSSI engine.  The specs
/// are the same as those used in spirit1_calc_rssi() above, but the process is
/// using different input and output.
/// 
/// Input is a whole-dBm value encoded linearly as: {0=-140dBm, 127=-13dBm}.
/// Output is the value that should go into SPIRIT1 RSSI_TH field.

    ot_int rssi_thr;
    rssi_thr    = input;
    rssi_thr   -= (_ATTEN_DB + 10);                 // SPIRIT1 uses -130 as baseline, hence +10
    rssi_thr    = (rssi_thr < 0) ? 0 : rssi_thr;    // Clip baseline at 0
    rssi_thr  <<= 1;                                // Multiply by 2 to yield half-dBm.
    
    return rssi_thr;
}


void spirit1_set_txpwr(ot_u8 pwr_code) {
/// Sets the tx output power.
/// "pwr_code" is a value, 0-127, that is: eirp_code/2 - 40 = TX dBm
/// i.e. eirp_code=0 => -40 dBm, eirp_code=80 => 0 dBm, etc

///@todo Table is not calibrated, it is a guess based on some known points.
///      A half-dB LUT from a hermite-cubic interpolated model should be built
///      and used until formal characterization is possible.
    static const ot_u8 pa_lut[85] = {
         0, 120, 113, 107, 102,  98,  94,  91,  88,  85,  82,  80,      //-30 to -24.5
        78,  76,  74,  73,  72,  71,  70,  69,  68,  67,  66,  65,      //-24 to -18.5
        64,  63,  62,  61,  60,  59,  59,  58,  57,  56,  55,  54,      //-18 to -12.5
        53,  52,  51,  50,  49,  48,  47,  46,  45,  43,  41,  39,      //-12 to  -6.5
        37,  34,  30,  26,  23,  21,  19,  18,  17,  16,  15,  14,      // -6 to  -0.5
        13,  13,  12,  12,  11,  11,  10,  10,   9,   9,   8,   8,      //  0 to   5.5
         7,   7,   7,   6,   6,   6,   5,   5,   5,   4,   4,   4,      //  6 to   11.5
         3                                                              // 12 
    };
    ot_u8   pa_table[10];
    ot_u8*  cursor;
    ot_int  step;
    ot_int  eirp_val;

    // "-20" corresponds to 20 half-dB steps.  
    // SPIRIT1 PA starts at -30, pwr_code at -40.
    eirp_val    = pwr_code;
    eirp_val   += (_ATTEN_DB*2) - 20;  

    // Adjust base power code in case it is out-of-range
    if (eirp_val < 0)       eirp_val = 0;
    else if (eirp_val > 84) eirp_val = 84;

    // Build PA RAMP using 8 steps of variable size.
    pa_table[0] = 0;
    pa_table[1] = RFREG(PAPOWER8);
    cursor      = &pa_table[2];
    step        = eirp_val >> 3;
    do {
        *cursor++   = eirp_val;
        eirp_val   -= step;
    } while (cursor != &pa_table[9]);
    
    // Write new PA Table to device
    spirit1_spibus_io(10, 0, pa_table);
}


ot_bool spirit1_check_cspin(void) {
    return (ot_bool)(RADIO_IRQ2_PORT->IDR & RADIO_IRQ2_PIN);
}





/// Simple configuration method: 
/// This I/O configuration does not use many of the SPIRIT1 advanced features.
/// Those features will be experimented-with in the future.

static const ot_u8 gpio_rx[6] = { 
    0, RFREG(GPIO3_CONF),
    RFGPO_RX_FIFO_ALMOST_FULL,  //indicate buffer threshold condition (kept for RX)
    RFGPO_RSSI_ABOVE_THR,       //indicate if RSSI goes above/below CS threshold
    RFGPO_SYNC_WORD,            //indicate when sync word is qualified
    RFGPO_READY                 //indicate when listen/rx is exited 
};

static const ot_u8 gpio_tx[6] = { 
    0, RFREG(GPIO3_CONF),
    RFGPO_TX_FIFO_ALMOST_EMPTY, //indicate buffer threshold condition (kept for RX)
    RFGPO_RSSI_ABOVE_THR,       //indicate if RSSI goes above/below CCA threshold
    RFGPO_SLEEP_OR_STANDBY,     //indicate when csma is backing off 
    RFGPO_READY                 //indicate when tx is exited
};
    
void spirit1_iocfg_rx()  {
/// All EXTIs for RX and TX are rising-edge detect, so the edge-select bit is
/// set universally following chip startup.
    EXTI->PR        = RFI_ALL;                  //clear all pending bits
//    EXTI->RTSR     |= RFI_ALL;
    spirit1_spibus_io(6, 0, (ot_u8*)gpio_rx);
}

void spirit1_iocfg_tx()  {
/// All EXTIs for RX and TX are rising-edge detect, so the edge-select bit is
/// set universally following chip startup.
    EXTI->PR        = RFI_ALL;                  //clear all pending bits
//    EXTI->RTSR     |= RFI_ALL; 
    spirit1_spibus_io(6, 0, (ot_u8*)gpio_tx);
}


void sub_int_config(ot_u16 ie_sel) {
    EXTI->PR    = RFI_ALL;
    EXTI->IMR  &= ~(RFI_ALL);
    EXTI->IMR  |= ie_sel;
}

void spirit1_int_off()      {   sub_int_config(0);            }

void spirit1_int_listen()   {   spirit1.imode = MODE_Listen;    
                                sub_int_config(RFI_LISTEN);     }
                                
void spirit1_int_rxdata()   {   spirit1.imode = MODE_RXData;
                                sub_int_config(RFI_RXDATA);   }
                                
void spirit1_int_csma()     {   spirit1.imode = MODE_CSMA;
                                sub_int_config(RFI_CSMA);     }
                                
void spirit1_int_txdata()   {   spirit1.imode = MODE_TXData;
                                sub_int_config(RFI_TXDATA);   }

void spirit1_int_force(ot_u16 ifg_sel)   { EXTI->SWIER |= ifg_sel; }
void spirit1_int_turnon(ot_u16 ie_sel)   { EXTI->IMR   |= ie_sel;  }
void spirit1_int_turnoff(ot_u16 ie_sel)  { EXTI->IMR   &= ~ie_sel; }



void spirit1_irq0_isr() {
    spirit1_virtual_isr(spirit1.imode);
}

void spirit1_irq1_isr() {
    spirit1_virtual_isr(spirit1.imode + 1);
}

void spirit1_irq2_isr() {
    spirit1_virtual_isr(spirit1.imode + 2);
}

void spirit1_irq3_isr() {
    spirit1_virtual_isr(spirit1.imode + 3);
}




#endif //#if from top of file
