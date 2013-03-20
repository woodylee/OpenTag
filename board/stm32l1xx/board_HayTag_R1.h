/* Copyright 2013 JP Norair
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
  * @file       /board/stm32l1xx/board_HayTag_R1.h
  * @author     JP Norair
  * @version    R100
  * @date       13 Mar 2013
  * @brief      Board Configuration for HayTag R1
  * @ingroup    Platform
  *
  * Do not include this file, include OT_platform.h
  *
  * SPECIAL NOTE: search for "note BOARD Macro" lines in this file.  These
  * must be ported to other STM32L boards, because the OpenTag STM32L platform 
  * implementation depends on them.
  *
  ******************************************************************************
  */
  

#ifndef __board_HayTag_R1_H
#define __board_HayTag_R1_H

#include "build_config.h"
#include "platform_STM32L1xx.h"

#ifdef __NULL_RADIO__
#   include "radio_null.h"
#else
#   include "radio_SPIRIT1.h"
#endif



/// Macro settings: ENABLED, DISABLED, NOT_AVAILABLE
#ifdef ENABLED
#   undef ENABLED
#endif
#define ENABLED  1

#ifdef DISABLED
#   undef DISABLED
#endif
#define DISABLED  0

#ifdef NOT_AVAILABLE
#   undef NOT_AVAILABLE
#endif
#define NOT_AVAILABLE   DISABLED




/** Additional RF Front End Parameters and Settings <BR>
  * ========================================================================<BR>
  * HayTag R1 assumes 6dB attenuation on antenna (25% efficiency) and 0.5dB
  * insertion loss for TX.
  */
#define RF_PARAM_BAND   433
#define RF_HDB_ATTEN    13      //Half dB attenuation (units = 0.5dB), used to scale TX power
#define RF_RSSI_OFFSET  6       //Offset applied to RSSI calculation



/** MCU Feature settings      <BR>
  * ========================================================================<BR>
  * Implemented capabilities of the STM32L variants
  */
//From platform_STM32L1xx.h
//#define MCU_FEATURE(VAL)              MCU_FEATURE_##VAL   // FEATURE 
#define MCU_FEATURE_MULTISPEED          DISABLED         // Allows usage of MF-HF clock boosting
#define MCU_FEATURE_MAPEEPROM           DISABLED
#define MCU_FEATURE_MPIPECDC            DISABLED        // USB-CDC MPipe implementation
#define MCU_FEATURE_MPIPEUART           ENABLED         // UART MPipe Implementation
#define MCU_FEATURE_MPIPEI2C            DISABLED        // I2C MPipe Implementation
#define MCU_FEATURE_MEMCPYDMA           ENABLED         // MEMCPY DMA should be lower priority than MPIPE DMA

#define MCU_PARAM(VAL)                  MCU_PARAM_##VAL
#define MCU_PARAM_PORTS                 3               // This STM32L has ports A, B, C
#define MCU_PARAM_VOLTLEVEL             2               // 3=1.2, 2=1.5V, 1=1.8V
#define MCU_PARAM_POINTERSIZE           4               // Pointers are 4 bytes




/** Platform Memory Configuration <BR>
  * ========================================================================<BR>
  * OpenTag needs to know where it can put Nonvolatile memory (file system) and
  * how much space it can allocate for filesystem.  For this configuration, 
  * Veelite is put into FLASH.
  *
  * The STM32L uses 256 byte Flash pages and 4KB Flash sectors.  Therefore, it
  * is best to allocate the FS in Flash on 4KB boundaries because this is the 
  * resolution that can be write protected (or, as with FS, *NOT* write 
  * protected).  Best practice with STM32 chips is to put the FS at the back of
  * of the Flash space because this seems to work best with the debugger HW.
  */

#define SRAM_SIZE               (10*1024)
#define EEPROM_SIZE             (4*1024)
#define FLASH_SIZE              (64*1024)

// Using EEPROM: Pages figure is irrelevant
#define FLASH_NUM_PAGES         (FLASH_SIZE/FLASH_PAGE_SIZE)
#define FLASH_FS_ADDR           (EEPROM_START_ADDR)
#define FLASH_FS_PAGES          0
#define FLASH_FS_FALLOWS        0 
#define FLASH_FS_ALLOC          (EEPROM_SIZE) 








/** Board-based Feature Settings <BR>
  * ========================================================================<BR>
  * 1. Two buttons are supported, but only if MPIPE is not enabled.
  *    <LI> SW1: multiplexed with TX/SCL </LI>
  *    <LI> SW2: multiplexed with RX/SDA </LI>
  * 
  * 2. Two LEDs can be supported.
  *    <LI> TRIG1 (typically Orange LED) is multiplexed on TEST pin.  TEST pin
  *         is a feature of the HayTag, and it has no special purpose in HW. </LI>
  *    <LI> TRIG2 (typically Green LED) is multiplexed on the TRACE-SWO pin.
  *         If you are using an SWO Tracer, you would need to disable TRIG2.
  *         (editorial note: I have never used or even seen an SWO Tracer) </LI>
  *
  * 3. MPIPE can support UART TX/RX (no flow control) or I2C.  In either case,
  *    a break character can be transmitted by the client ahead of the packet 
  *    in order to wakeup the system.  Assume that 20us is needed between the
  *    client-TX falling edge and start-bit rising edge.
  *
  * 4. The Debug and Upload interface is SWD.  There is no serial bootloader
  *    capability.  (Editorial Note: A ST-Link-V2 has SWD and a low price of
  *    US$25.  It can work with OpenOCD, so the toolchain is still very cheap.)
  * 
  * 5. The SPIRIT1 SPI benefits from the highest speed clock up to 20 MHz.
  */
#define BOARD_FEATURE(VAL)              BOARD_FEATURE_##VAL
#define BOARD_FEATURE_USBCONVERTER      ENABLED                 // Is UART connected via USB converter?
#define BOARD_FEATURE_MPIPE_DIRECT      ENABLED
#define BOARD_FEATURE_MPIPE_BREAK       DISABLED                // Send/receive leading break for wakeup
#define BOARD_FEATURE_MPIPE_CS          DISABLED                // Chip-Select / DTR wakeup control
#define BOARD_FEATURE_MPIPE_FLOWCTL     DISABLED                // RTS/CTS style flow control

#define BOARD_FEATURE_MPIPE_QMGMT       ENABLED                 // (possibly defunct)

#define BOARD_FEATURE_LFXTAL            ENABLED                 // LF XTAL used as Clock source
#define BOARD_FEATURE_HFXTAL            DISABLED                // HF XTAL used as Clock source
#define BOARD_FEATURE_RFXTAL            ENABLED                 // XTAL for RF chipset
#define BOARD_FEATURE_INVERT_TRIG1      DISABLED
#define BOARD_FEATURE_INVERT_TRIG2      DISABLED

#define BOARD_PARAM(VAL)                BOARD_PARAM_##VAL
#define BOARD_PARAM_LFHz                32768
#define BOARD_PARAM_LFtol               0.00002
#define BOARD_PARAM_MFHz                4200000
#define BOARD_PARAM_MFmult              1                       // Main CLK = HFHz * (MFmult/MFdiv)
#define BOARD_PARAM_MFdiv               1
#define BOARD_PARAM_MFtol               0.02
#define BOARD_PARAM_HFHz                16000000
#define BOARD_PARAM_HFmult              1                       // Turbo CLK = HFHz * (HFmult/HFdiv)
#define BOARD_PARAM_HFdiv               1
#define BOARD_PARAM_HFtol               0.02

// 24 MHz RF XTAL
#define BOARD_PARAM_RFHz                24000000                
#define BOARD_PARAM_RFtol               0.00003

#define BOARD_PARAM_AHBCLKDIV           1                       // AHB Clk = Main CLK / AHBCLKDIV
#define BOARD_PARAM_APB2CLKDIV          1                       // APB2 Clk = Main CLK / AHBCLKDIV
#define BOARD_PARAM_APB1CLKDIV          1                       // APB1 Clk = Main CLK / AHBCLKDIV

// SWD Interface (HayTag has SWD but no JTAG)
// The SWO (Trace) pin is available, but not often used
#define BOARD_SWDIO_PORT                GPIOA
#define BOARD_SWDIO_PINNUM              13
#define BOARD_SWDIO_PIN                 (1<<13)
#define BOARD_SWDCLK_PORT               GPIOA
#define BOARD_SWDCLK_PINNUM             14
#define BOARD_SWDCLK_PIN                (1<<14)
#define BOARD_TRACESWO_PORT             GPIOB
#define BOARD_TRACESWO_PINNUM           (1<<3)
#define BOARD_TRACESWO_PIN              3

// SW1/SW2 are implemented on the MPIPE TX/RX pins, only if MPIPE is disabled
#define BOARD_SW1_PORTNUM               1                   // Port B
#define BOARD_SW1_PORT                  GPIOB
#define BOARD_SW1_PINNUM                6
#define BOARD_SW1_PIN                   (1<<6)
#define BOARD_SW1_POLARITY              0
#define BOARD_SW1_PULLING               1

#define BOARD_SW2_PORTNUM               1
#define BOARD_SW2_PORT                  GPIOB
#define BOARD_SW2_PINNUM                7
#define BOARD_SW2_PIN                   (1<<7)
#define BOARD_SW2_POLARITY              0
#define BOARD_SW2_PULLING               1

// LED interface is implemented for test setups, on TEST pin and TRACE-SWO
// Obviously, if TRACESWO is enabled, only one LED can be used.
#define BOARD_LEDG_PORTNUM              1                   // Port B
#define BOARD_LEDG_PORT                 GPIOB
#define BOARD_LEDG_PINNUM               3
#define BOARD_LEDG_PIN                  (1<<3)
#define BOARD_LEDG_POLARITY             1

#define BOARD_LEDO_PORTNUM              1                   // Port B
#define BOARD_LEDO_PORT                 GPIOB
#define BOARD_LEDO_PINNUM               4
#define BOARD_LEDO_PIN                  (1<<4)
#define BOARD_LEDO_POLARITY             1

// MPIPE-UART interface: RX/TX only with no flow control
#define BOARD_UART_ID                   1                   // USART1
#define BOARD_UART_PORTNUM              1                   // Port B
#define BOARD_UART_PORT                 GPIOB
#define BOARD_UART_TXPINNUM             6
#define BOARD_UART_RXPINNUM             7
#define BOARD_UART_TXPIN                (1<<6)
#define BOARD_UART_RXPIN                (1<<7)

// MPIPE-I2C interface: multiplexed on UART, so not available at the same time
#define BOARD_I2C_ID                    1                   // I2C1
#define BOARD_I2C_PORTNUM               1                   // Port B
#define BOARD_I2C_PORT                  GPIOB
#define BOARD_I2C_SCLPINNUM             6
#define BOARD_I2C_SDAPINNUM             7
#define BOARD_I2C_SCLPIN                (1<<6)
#define BOARD_I2C_SDAPIN                (1<<7)

// SPIRIT1 RF interface
#define BOARD_RFGPIO_PORTNUM            1
#define BOARD_RFGPIO_PORT               GPIOB
#define BOARD_RFGPIO_0PINNUM            0
#define BOARD_RFGPIO_1PINNUM            1
#define BOARD_RFGPIO_2PINNUM            2
#define BOARD_RFGPIO_3PINNUM            10
#define BOARD_RFGPIO_SDNPINNUM          11
#define BOARD_RFGPIO_0PIN               (1<<BOARD_RFGPIO_0PINNUM)
#define BOARD_RFGPIO_1PIN               (1<<BOARD_RFGPIO_1PINNUM)
#define BOARD_RFGPIO_2PIN               (1<<BOARD_RFGPIO_2PINNUM)
#define BOARD_RFGPIO_3PIN               (1<<BOARD_RFGPIO_3PINNUM)
#define BOARD_RFGPIO_SDNPIN             (1<<BOARD_RFGPIO_SDNPINNUM)
#define BOARD_RFSPI_ID                  1       //SPI1
#define BOARD_RFSPI_PORTNUM             0       //Port A
#define BOARD_RFSPI_PORT                GPIOA
#define BOARD_RFSPI_MOSIPINNUM          7
#define BOARD_RFSPI_MISOPINNUM          6
#define BOARD_RFSPI_SCLKPINNUM          5
#define BOARD_RFSPI_CSNPINNUM           4
#define BOARD_RFSPI_MOSIPIN             (1<<BOARD_RFSPI_MOSIPINNUM)
#define BOARD_RFSPI_MISOPIN             (1<<BOARD_RFSPI_MISOPINNUM)
#define BOARD_RFSPI_SCLKPIN             (1<<BOARD_RFSPI_SCLKPINNUM)
#define BOARD_RFSPI_CSNPIN              (1<<BOARD_RFSPI_CSNPINNUM)






/** BOARD Macros (most of them, anyway) <BR>
  * ========================================================================<BR>
  * BOARD Macros are required by the OpenTag STM32L platform implementation.
  * We can assume certain things at the platform level, but not everything,
  * which is why BOARD Macros are required.
  *
  * If your board is implementing a radio (extremely likely), there are likely
  * some additional Board macros down near the radio configuration section.
  */
    // DMA is used in MEMCPY and thus it could be needed at any time during
    // active runtime.  So, the active-clock must be enabled permanently.
#   if (MCU_FEATURE_MEMCPYDMA)
#       define _DMACLK_N    RCC_AHBENR_DMA1EN
#   else
#       define _DMACLK_N    0
#       define _DMACLK_DYNAMIC
#   endif

    //Flash should be enabled unless you seriously modify OpenTag.
#   define _FLITFCLK_N      RCC_AHBENR_FLITFEN

    //The built-in CRC engine is rarely used, start disabled
#   define _CRCCLK_N        0   //RCC_AHBENR_CRCEN

    // This board uses GPIO A and B, but not C.  C is used for the 32 kHz OSC.
#   define _GPIOCLK_N       (   RCC_AHBENR_GPIOAEN  \
                            |   RCC_AHBENR_GPIOBEN  \
                            |   RCC_AHBENR_GPIOCEN  )

    // DMA will be enabled in Sleep on demand by the device driver.  By default
    // it is disabled in sleep
#   define _DMACLK_LP   0
#   define _DMACLK_DYNAMIC_LP

    // SRAM should always be clocked during SLEEP
#   define _SRAMCLK_LP      RCC_AHBLPENR_SRAMLPEN

    // FLASH needs to be clocked during LP modes, such as if you are sending
    // constant data from Flash across a DMA+Peripheral.  We assume this is the
    // case.  In any case, the supply current hit is negligible.
#   define _FLITFCLK_LP     RCC_AHBLPENR_FLITFLPEN

    // HW CRC32 engine is never used in sleep, or much at all, so fuck it
#   define _CRCCLK_LP       0       //RCC_AHBLPENR_CRCLPEN

    // GPIOA and B are also enabled in Low-Power Modes
#   define _GPIOCLK_LP      (   RCC_AHBLPENR_GPIOALPEN  \
                            |   RCC_AHBLPENR_GPIOBLPEN  \
                            |   RCC_AHBLPENR_GPIOCLPEN  )

//@note BOARD Macro for Peripheral Clock initialization at startup
static inline void BOARD_PERIPH_INIT(void) {
    //1. AHB Clock Setup for Active Mode
    RCC->AHBENR    = (_DMACLK_N | _FLITFCLK_N | _CRCCLK_N | _GPIOCLK_N);

    // 1b. AHB Clock Setup for Sleep Mode
    RCC->AHBLPENR  = (_DMACLK_LP | _SRAMCLK_LP | _FLITFCLK_LP | _CRCCLK_LP | _GPIOCLK_LP);

    // 2. APB2 Clocks in Active Mode.  APB2 is the high-speed peripheral bus.  
    // The default is all-off, and it is the job of the peripheral drivers to 
    // enable/disable their clocks as needed.  SYSCFG is the exception.
    // USART1, SPI1, ADC1, TIM11, TIM10, TIM9, SYSCFG.
    RCC->APB2ENR   = (RCC_APB2ENR_TIM9EN) | RCC_APB2ENR_SYSCFGEN;

    // 3. APB1 Clocks in Active Mode.  APB1 is the low-speed peripheral bus.
    // The default is all-off, and it is the job of the peripheral drivers to 
    // enable/disable their clocks as needed.  PWR is the exception.
    // COMP, DAC, PWR, USB, I2C2, I2C1, USART3, USART2, SPI2, WWDG, LCD, TIM7,
    // TIM6, TIM4, TIM3, TIM2
    RCC->APB1ENR   = (RCC_APB1ENR_PWREN); 
}


//@note BOARD Macro for DMA peripheral enabling
static inline void BOARD_DMA_CLKON(void) {
#ifdef _DMACLK_DYNAMIC
    // DMA is not enabled for normal mode by default, so enable it
    RCC->AHBENR    |= RCC_AHBENR_DMA1EN;
#endif
#ifdef _DMACLK_DYNAMIC_LP
    // DMA is not enabled for low-power mode by default, so enable it
    RCC->AHBLPENR  |= RCC_AHBLPENR_DMA1LPEN;
#endif
}

static inline void BOARD_DMA_CLKOFF(void) {
#ifdef _DMACLK_DYNAMIC
    // DMA is not enabled for normal mode by default, so disable it
    RCC->AHBENR    &= ~RCC_AHBENR_DMA1EN;
#endif
#ifdef _DMACLK_DYNAMIC_LP
    // DMA is not enabled for low-power mode by default, so disable it
    RCC->AHBLPENR  &= ~RCC_AHBLPENR_DMA1LPEN;
#endif
}

//Wipe-out some temporary constants which have generic names
#undef _DMACLK_N
#undef _FLITFCLK_N
#undef _CRCCLK_N
#undef _GPIOCLK_N
#undef _DMACLK_LP
#undef _SRAMCLK_LP
#undef _FLITFCLK_LP
#undef _CRCCLK_LP
#undef _GPIOCLK_LP




///@note BOARD Macro for EXTI initialization.  See also EXTI macros at the
///      bottom of the page.
static inline void BOARD_EXTI_STARTUP(void) {
    // EXTI0-3: RFGPIO0, RFGPIO1, RFGPIO2, SW2 (TRACESWO)
    SYSCFG->EXTICR[0]  |= (BOARD_RFGPIO_PORTNUM << 0) \
                        | (BOARD_RFGPIO_PORTNUM << 4) \
                        | (BOARD_RFGPIO_PORTNUM << 8) \
                        | (BOARD_SW2_PORTNUM << 12);
    
    // EXTI4-7: SW1, unused, SCL (if MPIPE_I2C), RX (if MPIPE_UART)
    SYSCFG->EXTICR[1]  |= (BOARD_SW1_PORTNUM << 0) \
                        | (BOARD_I2C_PORTNUM << 8) \
                        | (BOARD_UART_PORTNUM << 12);
    
    // EXTI8-11: unused, unused, RFGPIO3 (unused), unused
    //SYSCFG->EXTICR[2]  |= (BOARD_RFGPIO_PORTNUM << 8);
    
    // EXTI12-15: unused, unused, unused, unused
    //SYSCFG->EXTICR[3]  |= 
}



///@note BOARD Macro for initializing GPIO ports at startup, pursuant to the
///      connections in the schematic of this board.  This funciotn
static inline void BOARD_PORT_STARTUP(void) {
    
    // JTAG/SWD Interface: Set to Output-GND unless in DEBUG mode
#   if !defined(__DEBUG__)
    BOARD_SWDCLK_PORT->MODER |= (GPIO_MODER_OUT << (BOARD_SWDCLK_PINNUM*2)) \
                              | (GPIO_MODER_OUT << (BOARD_SWDIO_PINNUM*2));
#   endif 
    
    // The test interface is set to ALT by default, on PB3 & 4, as these are
    // JTAG ports that are not in use.  Clear that shit.
    GPIOB->MODER   &= ~(3 << (3*2)) & ~(3 << (4*2));
    
    
    // External UART on USART1 (Port B).  RTS/CTS flow control always disabled.
    // RX pin and CTS use input pull-down, TX pin uses push-pull output
    // CTS and RTS are conducted in SW, if enabled.
#   if (MCU_FEATURE(MPIPEUART) == ENABLED)
        BOARD_UART_PORT->PUPDR     |= (1 << (BOARD_UART_RXPINNUM*2)) \
                                    | (1 << (BOARD_UART_TXPINNUM*2));
        BOARD_UART_PORT->OSPEEDR   |= (GPIO_OSPEEDR_2MHz << (BOARD_UART_RXPINNUM*2)) \
                                    | (GPIO_OSPEEDR_2MHz << (BOARD_UART_TXPINNUM*2));
        BOARD_UART_PORT->AFR[0]    |= (7 << (BOARD_UART_RXPINNUM*4)) \
                                    | (7 << (BOARD_UART_TXPINNUM*4));                               
        BOARD_UART_PORT->MODER     |= (GPIO_MODER_ALT << (BOARD_UART_RXPINNUM*2))  \
                                    | (GPIO_MODER_ALT << (BOARD_UART_TXPINNUM*2));

    // External I2C on I2C1.
#   elif (MCU_FEATURE(MPIPEI2C) == ENABLED)
        BOARD_I2C_PORT->OSPEEDR    |= (GPIO_OSPEEDR_10MHz << (BOARD_I2C_SDAPINNUM*2))  \
                                    | (GPIO_OSPEEDR_10MHz << (BOARD_I2C_SCLPINNUM*2));
        BOARD_I2C_PORT->OTYPER     |= (1 << (BOARD_I2C_SDAPINNUM))  \
                                    | (1 << (BOARD_I2C_SCLPINNUM));
        BOARD_I2C_PORT->AFR[0]     |= (4 << (BOARD_I2C_SDAPINNUM*4))  \
                                    | (4 << (BOARD_I2C_SCLPINNUM*4));
        BOARD_I2C_PORT->MODER      |= (GPIO_MODER_ALT << (BOARD_I2C_SDAPINNUM*2))  \
                                    | (GPIO_MODER_ALT << (BOARD_I2C_SCLPINNUM*2));

    // No MPIPE, so set Pushbutton interface
    // Set to active-low pullups: Input @ 400kHz is startup-default
#   else
        //BOARD_SW1_PORT->MODER  &= ~(3 << (BOARD_SW1_PINNUM*2));
        BOARD_SW1_PORT->PUPDR      |= (1 << (BOARD_SW1_PINNUM*2)) \
                                    | (1 << (BOARD_SW2_PINNUM*2));
#   endif
    
    
    //SPIRIT1 RF Interface, using SPI1 and some GPIOs
    //GPIO0-3 are pull-down inputs, SDN is 2MHz push-pull output
    //SPI MISO is pull-down, rest of SPI is non-pulled
    BOARD_RFGPIO_PORT->MODER   |= (GPIO_MODER_OUT << (BOARD_RFGPIO_SDNPINNUM*2));
    BOARD_RFGPIO_PORT->OSPEEDR |= (GPIO_OSPEEDR_2MHz << (BOARD_RFGPIO_SDNPINNUM*2));
    BOARD_RFGPIO_PORT->PUPDR   |= (2 << (BOARD_RFGPIO_0PINNUM*2)) \
                                | (2 << (BOARD_RFGPIO_1PINNUM*2)) \
                                | (2 << (BOARD_RFGPIO_2PINNUM*2)) \
                                | (2 << (BOARD_RFGPIO_3PINNUM*2));
    BOARD_RFGPIO_PORT->BSRRL    = BOARD_RFGPIO_SDNPIN;
    BOARD_RFSPI_PORT->PUPDR    |= (0 << (BOARD_RFSPI_MOSIPINNUM*2)) \
                                | (2 << (BOARD_RFSPI_MISOPINNUM*2)) \
                                | (0 << (BOARD_RFSPI_SCLKPINNUM*2)) \
                                | (0 << (BOARD_RFSPI_CSNPINNUM*2));
    BOARD_RFSPI_PORT->MODER    |= (GPIO_MODER_ALT << (BOARD_RFSPI_MOSIPINNUM*2)) \
                                | (GPIO_MODER_ALT << (BOARD_RFSPI_MISOPINNUM*2)) \
                                | (GPIO_MODER_ALT << (BOARD_RFSPI_SCLKPINNUM*2)) \
                                | (GPIO_MODER_OUT << (BOARD_RFSPI_CSNPINNUM*2));
    BOARD_RFSPI_PORT->OSPEEDR  |= (GPIO_OSPEEDR_10MHz << (BOARD_RFSPI_MOSIPINNUM*2)) \
                                | (GPIO_OSPEEDR_10MHz << (BOARD_RFSPI_MISOPINNUM*2)) \
                                | (GPIO_OSPEEDR_10MHz << (BOARD_RFSPI_SCLKPINNUM*2)) \
                                | (GPIO_OSPEEDR_10MHz << (BOARD_RFSPI_CSNPINNUM*2));
    BOARD_RFSPI_PORT->AFR[0]   |= (5 << ((BOARD_RFSPI_MOSIPINNUM)*4)) \
                                | (5 << ((BOARD_RFSPI_MISOPINNUM)*4)) \
                                | (5 << ((BOARD_RFSPI_SCLKPINNUM)*4));
    BOARD_RFSPI_PORT->BSRRL     = BOARD_RFSPI_CSNPIN;
    
    
    // Set up all not-connected pins as output ground push-pull
    // PA0-3, PA8-12
    GPIOA->MODER   |= (GPIO_MODER_OUT << (0*2)) | (GPIO_MODER_OUT << (1*2)) \
                    | (GPIO_MODER_OUT << (2*2)) | (GPIO_MODER_OUT << (3*2)) \
                    | (GPIO_MODER_OUT << (8*2)) | (GPIO_MODER_OUT << (9*2)) \
                    | (GPIO_MODER_OUT << (10*2))| (GPIO_MODER_OUT << (11*2)) \
                    | (GPIO_MODER_OUT << (12*2));
    
    // PB3-5, PB8-9
    GPIOB->MODER   |= (GPIO_MODER_OUT << (3*2)) | (GPIO_MODER_OUT << (4*2)) \
                    | (GPIO_MODER_OUT << (5*2)) | (GPIO_MODER_OUT << (8*2)) \
                    | (GPIO_MODER_OUT << (9*2));
    
    // PC13
    GPIOC->MODER   |= (GPIO_MODER_OUT << (13*2));
    
    //The END
}



static inline void BOARD_PORT_STANDBY() {

    // JTAG/SWD Interface: Keep Alive
    
    // Pushbutton Interface: Keep Alive
    
    // LED interface: Keep Alive
    // LED usage will impact energy usage tremendously, so don't use LEDs in 
    // low-power apps. 
    
    // MPIPE interface on PB6-7, set to output ground.
#   if ((MCU_FEATURE(MPIPEUART) != ENABLED) && (MCU_FEATURE(MPIPEI2C) != ENABLED))
        GPIOB->PUPDR   &= ~(3 << (6*2)) & ~(7 << (6*2));
        GPIOB->MODER   &= ~(3 << (6*2)) & ~(7 << (6*2));
        GPIOB->ODR     &= ~(1 << 6) & ~(1 << 7);
#   endif
    
    
    //SPIRIT1 RF Interface, using SPI1 and some GPIOs
    //GPIO0-3 are floating inputs (default), SDN is 2MHz push-pull output
    
    // ground spi pins?
}




///@note BOARD Macro for initializing the STM32 PVD Module (voltage monitor) at
///      startup.  Only relevant for battery-powered boards.
static inline void BOARD_POWER_STARTUP(void) {
#   if (MCU_FEATURE_SVMONITOR == ENABLED)
    // Level0 is 1.9V, each level is +0.2V.  Check your battery spec and change
    // the _LEVx component to match it.  Typical Li-Ion undervoltage is 2.7V.
    PWR->CR    |= PWR_CR_PLS_LEV0 | PWR_CR_PVDE;  
#   endif
}




///@note BOARD Macro for initializing the STM32 Crystal startup routine, done
///      at startup.  For any build using CMSIS libraries (standard), this will
///      be blank.  In those cases, crystal init is done in the CMSIS system
///      startup function.
static inline void BOARD_XTAL_STARTUP(void) {
// Currently this is handled in the system startup function.  OpenTag requires
// a 32768Hz clock
}


   

///@note BOARD Macros for Radio module interrupt vectoring.  Connect these to
///      the radio interface driver you are using.  Check the schematic of your
///      board to see where the Radio IRQ lines are routed.  Also check the 
///      radio interface header documentation (it's really quite simple). 
///      These Macros will get called in the universal EXTI interrupt handler,
///      typically implemented in platform_isr_STM32L.c
#define BOARD_RADIO_EXTI0_ISR()     spirit1_irq0_isr()
#define BOARD_RADIO_EXTI1_ISR()     spirit1_irq1_isr()
#define BOARD_RADIO_EXTI2_ISR()     spirit1_irq2_isr()
#define BOARD_RADIO_EXTI3_ISR();
#define BOARD_RADIO_EXTI4_ISR();
#define BOARD_RADIO_EXTI5_ISR();
#define BOARD_RADIO_EXTI6_ISR();     
#define BOARD_RADIO_EXTI7_ISR();     
#define BOARD_RADIO_EXTI8_ISR();     
#define BOARD_RADIO_EXTI9_ISR();     
#define BOARD_RADIO_EXTI10_ISR();
#define BOARD_RADIO_EXTI11_ISR();
#define BOARD_RADIO_EXTI12_ISR();
#define BOARD_RADIO_EXTI13_ISR();
#define BOARD_RADIO_EXTI14_ISR();
#define BOARD_RADIO_EXTI15_ISR();




/** Note: Clocking for the Board's MCU      <BR>
  * ========================================================================<BR>
  * The STM32L can be clocked up to 32MHz, but generally we use it at 16 MHz.
  * At the 1.5V core setting, 16MHz can be used with 1 flash waitstate, but 
  * OpenTag code tends to abhor branching, so flash wait states have less 
  * affect on OT efficiency than they have on most other codebases.
  */
#define MCU_PARAM_LFXTALHz          BOARD_PARAM_LFHz
#define MCU_PARAM_LFXTALtol         BOARD_PARAM_LFtol
//#define MCU_PARAM_LFOSCHz           BOARD_PARAM_LFHz
//#define MCU_PARAM_LFOSCtol          BOARD_PARAM_LFtol
//#define MCU_PARAM_XTALHz            BOARD_PARAM_HFHz
//#define MCU_PARAM_XTALmult          BOARD_PARAM_HFmult
//#define MCU_PARAM_XTALtol           BOARD_PARAM_HFtol
#define MCU_PARAM_OSCHz             ((BOARD_PARAM_HFHz*BOARD_PARAM_HFmult)/BOARD_PARAM_HFdiv)
#define MCU_PARAM_OSCmult           BOARD_PARAM_HFmult
#define MCU_PARAM_OSCtol            BOARD_PARAM_HFtol

#define PLATFORM_MCLK_DIV           BOARD_PARAM_MCLKDIV
#define PLATFORM_SMCLK_DIV          BOARD_PARAM_SMCLKDIV

#define PLATFORM_LSCLOCK_PINS       BOARD_LFXT_PINS
#define PLATFORM_LSCLOCK_CONF       BOARD_LFXT_CONF
#define PLATFORM_HSCLOCK_PINS       BOARD_HFXT_PINS
#define PLATFORM_HSCLOCK_CONF       BOARD_HFXT_CONF
#define PLATFORM_LSCLOCK_HZ         BOARD_PARAM_LFHz
#define PLATFORM_LSCLOCK_ERROR      BOARD_PARAM_LFtol
#define PLATFORM_MSCLOCK_HZ         (BOARD_PARAM_MFHz)
#define PLATFORM_MSCLOCK_ERROR      BOARD_PARAM_MFtol
#define PLATFORM_HSCLOCK_HZ         ((BOARD_PARAM_HFHz*BOARD_PARAM_HFmult)/BOARD_PARAM_HFdiv)
#define PLATFORM_HSCLOCK_ERROR      BOARD_PARAM_HFtol
#define PLATFORM_HSCLOCK_MULT       BOARD_PARAM_HFmult








/** Peripheral definitions for this platform <BR>
  * ========================================================================<BR>
  * <LI> OT_GPTIM:  "General Purpose Timer" used by OpenTag kernel and other
  *                   internal stuff (sometimes Radio MAC timer). </LI>
  * <LI> OT_TRIG:   Optional test trigger(s) usable in OpenTag apps.  Often the 
  *                   triggers are implemented on LED pins.  Trig1 is usually
  *                   green (typ 570nm) and Trig2 orange (typ 605nm). </LI>
  * <LI> OT_SWITCH: Optional input switches/sources.  Usually there is at least
  *                   one.  Many apps (especially demos) require a button. </LI>
  * <LI> OT_GWNADC: Optional floating pin or other type of noise source that 
  *                   can be used with ADC-based true-random-number generator </LI>
  * <LI> OT_GWNDRV: Optional driver pin to power some sort of noise source </LI>
  * <LI> MPIPE:     The wireline interface, usually either UART or USB-CDC, but 
  *                   possible to use with a multi-master I2C or SPI as well </LI>
  */
#define OT_GPTIM_ID         'R'
#define OT_GPTIM            RTC
#define OT_GPTIM_CLOCK      32768
#define OT_GPTIM_RES        1024
#define OT_GPTIM_SHIFT      0
#define TI_TO_CLK(VAL)      ((OT_GPTIM_RES/1024)*VAL)
#define CLK_TO_TI(VAL)      (VAL/(OT_GPTIM_RES/1024))


#if (OT_GPTIM_CLOCK == BOARD_PARAM_LFHz)
#   define OT_GPTIM_ERROR   BOARD_PARAM_LFtol
#else
#   define OT_GPTIM_ERROR   BOARD_PARAM_HFtol
#endif

#define OT_GPTIM_ERRDIV     32768 //this needs to be hard-coded

#define OT_KTIM_IRQ_SRCLINE     BOARD_GPTIM1_PINNUM
#define OT_MACTIM_IRQ_SRCLINE   BOARD_GPTIM2_PINNUM



#define OT_TRIG1_PORT       BOARD_LEDG_PORT
#define OT_TRIG1_PINNUM     BOARD_LEDG_PINNUM
#define OT_TRIG1_PIN        BOARD_LEDG_PIN
#define OT_TRIG1_POLARITY   BOARD_LEDG_POLARITY
#define OT_TRIG2_PORT       BOARD_LEDO_PORT
#define OT_TRIG2_PINNUM     BOARD_LEDO_PINNUM
#define OT_TRIG2_PIN        BOARD_LEDO_PIN
#define OT_TRIG2_POLARITY   BOARD_LEDO_POLARITY

#if (OT_TRIG1_POLARITY != 0)
#   define OT_TRIG1_ON()    OT_TRIG1_PORT->BSRRL = OT_TRIG1_PIN;
#   define OT_TRIG1_OFF()   OT_TRIG1_PORT->BSRRH = OT_TRIG1_PIN;
#   define OT_TRIG1_TOG()   OT_TRIG1_PORT->ODR  ^= OT_TRIG1_PIN;
#else 
#   define OT_TRIG1_ON()    OT_TRIG1_PORT->MODER |= (OT_TRIG1_PINNUM*2);
#   define OT_TRIG1_OFF()   OT_TRIG1_PORT->MODER &= ~(OT_TRIG1_PINNUM*2);
#   define OT_TRIG1_OFF()   OT_TRIG1_PORT->MODER &= ~(OT_TRIG1_PINNUM*2);
#endif

#if (OT_TRIG2_POLARITY != 0)
#   define OT_TRIG2_ON()    OT_TRIG2_PORT->BSRRL = OT_TRIG2_PIN;
#   define OT_TRIG2_OFF()   OT_TRIG2_PORT->BSRRH = OT_TRIG2_PIN;
#   define OT_TRIG2_TOG()   OT_TRIG2_PORT->ODR  ^= OT_TRIG2_PIN;
#else 
#   define OT_TRIG2_ON()    OT_TRIG2_PORT->MODER |= (OT_TRIG2_PINNUM*2);
#   define OT_TRIG2_OFF()   OT_TRIG2_PORT->MODER &= ~(OT_TRIG2_PINNUM*2);
#   define OT_TRIG2_TOG()   OT_TRIG2_PORT->MODER ^= (OT_TRIG2_PINNUM*2);
#endif



#define OT_SWITCH1_PORTNUM  BOARD_SW1_PORTNUM
#define OT_SWITCH1_PORT     BOARD_SW1_PORT
#define OT_SWITCH1_PINNUM   BOARD_SW1_PINNUM
#define OT_SWITCH1_PIN      BOARD_SW1_PIN
#define OT_SWITCH1_POLARITY BOARD_SW1_POLARITY

#define OT_SWITCH2_PORTNUM  BOARD_SW2_PORTNUM
#define OT_SWITCH2_PORT     BOARD_SW2_PORT
#define OT_SWITCH2_PINNUM   BOARD_SW2_PINNUM
#define OT_SWITCH2_PIN      BOARD_SW2_PIN
#define OT_SWITCH2_POLARITY BOARD_SW2_POLARITY




// Use PA6 as floating pin for ADC-based random number.  You could also put on 
// a low voltage, reverse-biased zener on the board to produce a pile of noise 
// if you are getting low-entropy on your pin (2.1V zener usually is nice).
// Set OT_GWNDRV to enable such a zener setup.
#define OT_GWNADC_PORTNUM   0
#define OT_GWNADC_PORT      GPIOA
#define OT_GWNADC_PINNUM    6
#define OT_GWNADC_PIN       (1<<6)
#define OT_GWNADC_BITS      8

//#define OT_GWNDRV_PORTNUM   0
//#define OT_GWNDRV_PORT      GPIOA
//#define OT_GWNDRV_PINNUM    7
//#define OT_GWNDRV_PIN       (1<<7)







/** Boilerplate STM32L MPipe Setup <BR>
  * ========================================================================<BR>
  * USB MPipe requires a CDC firmware library subsystem, which typically uses
  * memcpy to move data across HW buffers and such.  UART MPipe *REQUIRES* a,
  * DMA however.  You could implement a driver without a DMA, but DMA makes it 
  * so much cleaner and better.
  */
#if (MCU_FEATURE_MPIPECDC == ENABLED)
// USB is mostly independent from OT, but the startup code does need to know 
// how to boost the crystal
#   if (BOARD_PARAM_HFHz != 2000000) && (BOARD_PARAM_HFHz != 3000000) \
      && (BOARD_PARAM_HFHz != 4000000) && (BOARD_PARAM_HFHz != 6000000) \
      && (BOARD_PARAM_HFHz != 8000000) && (BOARD_PARAM_HFHz != 12000000) \
      && (BOARD_PARAM_HFHz != 16000000) && (BOARD_PARAM_HFHz != 24000000)
#       error "USB requires 2, 3, 4, 6, 8, 12, 16, or 24 MHz HSE XTAL."
#   elif (BOARD_PARAM_HFtol > 0.000050)
#       error "USB requires that the tolerance of the HSE is < +/- 50ppm"
#   endif
#   define MPIPE_USB_ID         0
#   define MPIPE_USB            USB0
#   define MPIPE_USBDP_PORT     BOARD_USB_PORT
#   define MPIPE_USBDM_PORT     BOARD_USB_PORT
#   define MPIPE_USBDP_PIN      BOARD_USB_DPPIN
#   define MPIPE_USBDM_PIN      BOARD_USB_DMPIN

#else
#   define MPIPE_DMANUM         1
#   define MPIPE_DMA            DMA1
#   define MPIPE_UART_ID        BOARD_UART_ID
#   define MPIPE_UART_PORTNUM   BOARD_UART_PORTNUM
#   define MPIPE_UART_PORT      BOARD_UART_PORT
#   define MPIPE_UART_RXPIN     BOARD_UART_RXPIN
#   define MPIPE_UART_TXPIN     BOARD_UART_TXPIN
#   define MPIPE_RTS_PORT       BOARD_UART_PORT
#   define MPIPE_CTS_PORT       BOARD_UART_PORT
#   define MPIPE_RTS_PIN        BOARD_UART_RTSPIN
#   define MPIPE_CTS_PIN        BOARD_UART_CTSPIN
#   define MPIPE_UART_PINS      (MPIPE_UART_RXPIN | MPIPE_UART_TXPIN)

#   if (MPIPE_UART_ID == 1)
#       define MPIPE_UART       USART1
#       define MPIPE_DMA_RXCHAN_ID  5
#       define MPIPE_DMA_TXCHAN_ID  4
#       define __USE_DMA1_CHAN5
#       define __USE_DMA1_CHAN4

#   elif (MPIPE_UART_ID == 2)
#       define MPIPE_UART       USART2
#       define MPIPE_DMA_RXCHAN_ID  6
#       define MPIPE_DMA_TXCHAN_ID  7
#       define __USE_DMA1_CHAN7
#       define __USE_DMA1_CHAN6

#   elif (MPIPE_UART_ID == 3)
#       define MPIPE_UART       USART3
#       define MPIPE_DMA_RXCHAN_ID  3
#       define MPIPE_DMA_TXCHAN_ID  2
#       define __USE_DMA1_CHAN3
#       define __USE_DMA1_CHAN2

#   elif (MPIPE_UART_ID == 4)
#       define MPIPE_UART       UART4

#   elif (MPIPE_UART_ID == 5)
#       define MPIPE_UART       UART5

#   else
#       error "MPIPE_UART_ID is defined out of range"
#   endif

#endif







/** Boilerplate STM32L SPIRIT1-RF Setup <BR>
  * ========================================================================<BR>
  * The SPIRIT1 interface uses an SPI (ideally SPI1 because it is attached to
  * the high-speed APB2 bus).  It also typically uses a DMA for bulk transfers,
  * during which the core can be shut-off for reducing power.
  */
 
#define RADIO_SPI_ID    BOARD_RFSPI_ID
#if (BOARD_RFSPI_ID == 1)
#   define RADIO_SPI    SPI1
#   if (defined(__USE_DMA1_CHAN2) || defined(__USE_DMA1_CHAN3))
#       error "RADIO SPI is SPI1, which needs DMA Ch2 & Ch3, but they are used."
#   else
#       define __USE_DMA1_CHAN2
#       define __USE_DMA1_CHAN3
#       define __N_ISR_DMA1_Channel2
#       define __N_ISR_DMA1_Channel3
#   endif
#elif (BOARD_RFSPI_ID == 2)
#   define RADIO_SPI    SPI2
#   if (defined(__USE_DMA1_CHAN4) || defined(__USE_DMA1_CHAN5))
#       error "RADIO SPI is SPI2, which needs DMA Ch4 & Ch5, but they are used."
#   else
#       define __USE_DMA1_CHAN4
#       define __USE_DMA1_CHAN5
#       define __N_ISR_DMA1_Channel4
#       define __N_ISR_DMA1_Channel5
#   endif
#else
#   error "RADIO_SPI must be 1 or 2"
#endif

#define RADIO_SPI_PORT              BOARD_RFSPI_PORT
#define RADIO_SPIMOSI_PORT          BOARD_RFSPI_PORT
#define RADIO_SPIMISO_PORT          BOARD_RFSPI_PORT
#define RADIO_SPICLK_PORT           BOARD_RFSPI_PORT
#define RADIO_SPICS_PORT            BOARD_RFSPI_PORT
#define RADIO_SPIMOSI_PIN           BOARD_RFSPI_MOSIPIN
#define RADIO_SPIMISO_PIN           BOARD_RFSPI_MISOPIN
#define RADIO_SPICLK_PIN            BOARD_RFSPI_SCLKPIN
#define RADIO_SPICS_PIN             BOARD_RFSPI_CSNPIN

#define RADIO_IRQ0_SRCPORT          BOARD_RFGPIO_PORTNUM
#define RADIO_IRQ1_SRCPORT          BOARD_RFGPIO_PORTNUM
#define RADIO_IRQ2_SRCPORT          BOARD_RFGPIO_PORTNUM
#define RADIO_IRQ3_SRCPORT          BOARD_RFGPIO_PORTNUM
#define RADIO_IRQ0_SRCLINE          BOARD_RFGPIO_0PINNUM
#define RADIO_IRQ1_SRCLINE          BOARD_RFGPIO_1PINNUM
#define RADIO_IRQ2_SRCLINE          BOARD_RFGPIO_2PINNUM
#define RADIO_IRQ3_SRCLINE          -1

#define RADIO_SDN_PORT              BOARD_RFGPIO_PORT
#define RADIO_IRQ0_PORT             BOARD_RFGPIO_PORT
#define RADIO_IRQ1_PORT             BOARD_RFGPIO_PORT
#define RADIO_IRQ2_PORT             BOARD_RFGPIO_PORT
#define RADIO_IRQ3_PORT             BOARD_RFGPIO_PORT
#define RADIO_SDN_PIN               BOARD_RFGPIO_SDNPIN
#define RADIO_IRQ0_PIN              BOARD_RFGPIO_0PIN
#define RADIO_IRQ1_PIN              BOARD_RFGPIO_1PIN
#define RADIO_IRQ2_PIN              BOARD_RFGPIO_2PIN
#define RADIO_IRQ3_PIN              BOARD_RFGPIO_3PIN





/** Boilerplate STM32L DMA Memcpy Setup <BR>
  * ========================================================================<BR>
  * Memcpy should have a low DMA priority, such as Channel7.  It is a MEM2MEM
  * function of the DMA, so it can take any channel.  If there is another user
  * of DMA Channel 7, then the potential throughput of that communication
  * interface is limited by the duty time of memcpy.
  */
#if (MCU_FEATURE_MEMCPYDMA != ENABLED)
#   error "Not using DMA for MEMCPY, a sub-optimal design choice."
#elif !defined(__USE_DMA1_CHAN7)
#   define MEMCPY_DMA_CHAN_ID 7
#   define MEMCPY_DMA       DMA1
#   define MEMCPY_DMACHAN   DMA1_Channel7
#   define __USE_DMA1_CHAN7
#elif !defined(__USE_DMA1_CHAN6)
#   define MEMCPY_DMA_CHAN_ID    6
#   define MEMCPY_DMA       DMA1
#   define MEMCPY_DMACHAN   DMA1_Channel6
#   define __USE_DMA1_CHAN6
#elif !defined(__USE_DMA1_CHAN5)
#   define MEMCPY_DMA_CHAN_ID    5
#   define MEMCPY_DMA       DMA1
#   define MEMCPY_DMACHAN   DMA1_Channel5
#   define __USE_DMA1_CHAN5
#elif !defined(__USE_DMA1_CHAN4)
#   define MEMCPY_DMA_CHAN_ID    4
#   define MEMCPY_DMA       DMA1
#   define MEMCPY_DMACHAN   DMA1_Channel4
#   define __USE_DMA1_CHAN4
#elif !defined(__USE_DMA1_CHAN3)
#   define MEMCPY_DMA_CHAN_ID    3
#   define MEMCPY_DMA       DMA1
#   define MEMCPY_DMACHAN   DMA1_Channel3
#   define __USE_DMA1_CHAN3
#elif !defined(__USE_DMA1_CHAN2)
#   define MEMCPY_DMA_CHAN_ID    2
#   define MEMCPY_DMA       DMA1
#   define MEMCPY_DMACHAN   DMA1_Channel2
#   define __USE_DMA1_CHAN2
#elif !defined(__USE_DMA1_CHAN1)
#   define MEMCPY_DMA_CHAN_ID    1
#   define MEMCPY_DMA       DMA1
#   define MEMCPY_DMACHAN   DMA1_Channel1
#   define __USE_DMA1_CHAN1
#else
#   error "There is no available DMA Channel to allocate to MEMCPY."
#endif






/** EXTI Line Utilization declaration <BR>
  * ========================================================================<BR>
  * EXTI Lines (external line interrupts coming from GPIO pins) are shared
  * by all GPIO ports, so setting them up correctly is a bit of a mess.  We
  * need to inspect all the external interrupt sources.
  */
#if (!defined(__USE_EXTI0) && ( \
        (OT_SWITCH1_PINNUM == 0) || (OT_SWITCH2_PINNUM == 0) || \
        (RADIO_IRQ0_SRCLINE == 0) || (RADIO_IRQ1_SRCLINE == 0) || (RADIO_IRQ2_SRCLINE == 0) || (RADIO_IRQ3_SRCLINE == 0) \
    ))
#   define __USE_EXTI0
#endif
#if (!defined(__USE_EXTI1) && ( \
        (OT_SWITCH1_PINNUM == 1) || (OT_SWITCH2_PINNUM == 1) || \
        (RADIO_IRQ0_SRCLINE == 1) || (RADIO_IRQ1_SRCLINE == 1) || (RADIO_IRQ2_SRCLINE == 1) || (RADIO_IRQ3_SRCLINE == 1) \
    ))
#   define __USE_EXTI1
#endif
#if (!defined(__USE_EXTI2) && ( \
        (OT_SWITCH1_PINNUM == 2) || (OT_SWITCH2_PINNUM == 2) || \
        (RADIO_IRQ0_SRCLINE == 2) || (RADIO_IRQ1_SRCLINE == 2) || (RADIO_IRQ2_SRCLINE == 2) || (RADIO_IRQ3_SRCLINE == 2) \
    ))
#   define __USE_EXTI2
#endif
#if (!defined(__USE_EXTI3) && ( \
        (OT_SWITCH1_PINNUM == 3) || (OT_SWITCH2_PINNUM == 3) || \
        (RADIO_IRQ0_SRCLINE == 3) || (RADIO_IRQ1_SRCLINE == 3) || (RADIO_IRQ2_SRCLINE == 3) || (RADIO_IRQ3_SRCLINE == 3) \
    ))
#   define __USE_EXTI3
#endif
#if (!defined(__USE_EXTI4) && ( \
        (OT_SWITCH1_PINNUM == 4) || (OT_SWITCH2_PINNUM == 4) || \
        (RADIO_IRQ0_SRCLINE == 4) || (RADIO_IRQ1_SRCLINE == 4) || (RADIO_IRQ2_SRCLINE == 4) || (RADIO_IRQ3_SRCLINE == 4) \
    ))
#   define __USE_EXTI4
#endif
#if (!defined(__USE_EXTI5) && ( \
        (OT_SWITCH1_PINNUM == 5) || (OT_SWITCH2_PINNUM == 5) || \
        (RADIO_IRQ0_SRCLINE == 5) || (RADIO_IRQ1_SRCLINE == 5) || (RADIO_IRQ2_SRCLINE == 5) || (RADIO_IRQ3_SRCLINE == 5) \
    ))
#   define __USE_EXTI5
#endif
#if (!defined(__USE_EXTI6) && ( \
        (OT_SWITCH1_PINNUM == 6) || (OT_SWITCH2_PINNUM == 6) || \
        (RADIO_IRQ0_SRCLINE == 6) || (RADIO_IRQ1_SRCLINE == 6) || (RADIO_IRQ2_SRCLINE == 6) || (RADIO_IRQ3_SRCLINE == 6) \
    ))
#   define __USE_EXTI6
#endif
#if (!defined(__USE_EXTI7) && ( \
        (OT_SWITCH1_PINNUM == 7) || (OT_SWITCH2_PINNUM == 7) || \
        (RADIO_IRQ0_SRCLINE == 7) || (RADIO_IRQ1_SRCLINE == 7) || (RADIO_IRQ2_SRCLINE == 7) || (RADIO_IRQ3_SRCLINE == 7) \
    ))
#   define __USE_EXTI7
#endif
#if (!defined(__USE_EXTI8) && ( \
        (OT_SWITCH1_PINNUM == 8) || (OT_SWITCH2_PINNUM == 8) || \
        (RADIO_IRQ0_SRCLINE == 8) || (RADIO_IRQ1_SRCLINE == 8) || (RADIO_IRQ2_SRCLINE == 8) || (RADIO_IRQ3_SRCLINE == 8) \
    ))
#   define __USE_EXTI8
#endif
#if (!defined(__USE_EXTI9) && ( \
        (OT_SWITCH1_PINNUM == 9) || (OT_SWITCH2_PINNUM == 9) || \
        (RADIO_IRQ0_SRCLINE == 9) || (RADIO_IRQ1_SRCLINE == 9) || (RADIO_IRQ2_SRCLINE == 9) || (RADIO_IRQ3_SRCLINE == 9) \
    ))
#   define __USE_EXTI9
#endif
#if (!defined(__USE_EXTI10) && ( \
        (OT_SWITCH1_PINNUM == 10) || (OT_SWITCH2_PINNUM == 10) || \
        (RADIO_IRQ0_SRCLINE == 10) || (RADIO_IRQ1_SRCLINE == 10) || (RADIO_IRQ2_SRCLINE == 10) || (RADIO_IRQ3_SRCLINE == 10) \
    ))
#   define __USE_EXTI10
#endif
#if (!defined(__USE_EXTI11) && ( \
        (OT_SWITCH1_PINNUM == 11) || (OT_SWITCH2_PINNUM == 11) || \
        (RADIO_IRQ0_SRCLINE == 11) || (RADIO_IRQ1_SRCLINE == 11) || (RADIO_IRQ2_SRCLINE == 11) || (RADIO_IRQ3_SRCLINE == 11) \
    ))
#   define __USE_EXTI11
#endif
#if (!defined(__USE_EXTI12) && ( \
        (OT_SWITCH1_PINNUM == 12) || (OT_SWITCH2_PINNUM == 12) || \
        (RADIO_IRQ0_SRCLINE == 12) || (RADIO_IRQ1_SRCLINE == 12) || (RADIO_IRQ2_SRCLINE == 12) || (RADIO_IRQ3_SRCLINE == 12) \
    ))
#   define __USE_EXTI12
#endif
#if (!defined(__USE_EXTI13) && ( \
        (OT_SWITCH1_PINNUM == 13) || (OT_SWITCH2_PINNUM == 13) || \
        (RADIO_IRQ0_SRCLINE == 13) || (RADIO_IRQ1_SRCLINE == 13) || (RADIO_IRQ2_SRCLINE == 13) || (RADIO_IRQ3_SRCLINE == 13) \
    ))
#   define __USE_EXTI13
#endif
#if (!defined(__USE_EXTI14) && ( \
        (OT_SWITCH1_PINNUM == 14) || (OT_SWITCH2_PINNUM == 14) || \
        (RADIO_IRQ0_SRCLINE == 14) || (RADIO_IRQ1_SRCLINE == 14) || (RADIO_IRQ2_SRCLINE == 14) || (RADIO_IRQ3_SRCLINE == 14) \
    ))
#   define __USE_EXTI14
#endif
#if (!defined(__USE_EXTI15) && ( \
        (OT_SWITCH1_PINNUM == 15) || (OT_SWITCH2_PINNUM == 15) || \
        (RADIO_IRQ0_SRCLINE == 15) || (RADIO_IRQ1_SRCLINE == 15) || (RADIO_IRQ2_SRCLINE == 15) || (RADIO_IRQ3_SRCLINE == 15) \
    ))
#   define __USE_EXTI15
#endif





#endif
