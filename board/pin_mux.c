/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v6.0
processor: LPC55S69
package_id: LPC55S69JBD100
mcu_data: ksdk2_0
processor_version: 6.0.1
pin_labels:
- {pin_num: '10', pin_signal: PIO1_9/FC1_SCK/CT_INP4/SCT0_OUT2/FC4_CTS_SDA_SSEL0/ADC0_12, label: IQR}
- {pin_num: '14', pin_signal: PIO0_16/FC4_TXD_SCL_MISO_WS/CLKOUT/CT_INP4/SECURE_GPIO0_16/ADC0_8, label: KFETON, identifier: KFETON_ID}
- {pin_num: '31', pin_signal: PIO1_5/FC0_RXD_SDA_MOSI_DATA/SD0_D2/CTIMER2_MAT0/SCT_GPI0, label: PWRON, identifier: PWRON_ID}
- {pin_num: '64', pin_signal: PIO1_18/SD1_POW_EN/SCT0_OUT5/PLU_OUT0, label: IRQ, identifier: IRQ_ID}
- {pin_num: '27', pin_signal: PIO0_27/FC2_TXD_SCL_MISO_WS/CTIMER3_MAT2/SCT0_OUT6/FC7_RXD_SDA_MOSI_DATA/PLU_OUT0/SECURE_GPIO0_27, label: USART2 TX}
- {pin_num: '3', pin_signal: PIO1_24/FC2_RXD_SDA_MOSI_DATA/SCT0_OUT1/SD1_D1/FC3_SSEL3/PLU_OUT6, label: USART2 RX}
- {pin_num: '68', pin_signal: PIO1_26/FC2_CTS_SDA_SSEL0/SCT0_OUT3/CT_INP3/UTICK_CAP1/HS_SPI_SSEL3/PLU_IN5, label: USART2 CTS}
- {pin_num: '85', pin_signal: PIO1_27/FC2_RTS_SCL_SSEL1/SD0_D4/CTIMER0_MAT3/CLKOUT/PLU_IN4, label: USART2 RTS}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_iocon.h"
#include "pin_mux.h"

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 *
 * END ****************************************************************************************************************/
void BOARD_InitBootPins(void)
{
    BOARD_InitPins();
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', coreID: cm33_core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '92', peripheral: FLEXCOMM0, signal: RXD_SDA_MOSI_DATA, pin_signal: PIO0_29/FC0_RXD_SDA_MOSI_DATA/SD1_D2/CTIMER2_MAT3/SCT0_OUT8/CMP0_OUT/PLU_OUT2/SECURE_GPIO0_29,
    mode: inactive, slew_rate: standard, invert: disabled, open_drain: disabled}
  - {pin_num: '94', peripheral: FLEXCOMM0, signal: TXD_SCL_MISO_WS, pin_signal: PIO0_30/FC0_TXD_SCL_MISO_WS/SD1_D3/CTIMER0_MAT0/SCT0_OUT9/SECURE_GPIO0_30, mode: inactive,
    slew_rate: standard, invert: disabled, open_drain: disabled}
  - {pin_num: '4', peripheral: FLEXCOMM4, signal: TXD_SCL_MISO_WS, pin_signal: PIO1_20/FC7_RTS_SCL_SSEL1/CT_INP14/FC4_TXD_SCL_MISO_WS/PLU_OUT2, mode: inactive, slew_rate: standard,
    invert: disabled, open_drain: disabled}
  - {pin_num: '30', peripheral: FLEXCOMM4, signal: RXD_SDA_MOSI_DATA, pin_signal: PIO1_21/FC7_CTS_SDA_SSEL0/CTIMER3_MAT2/FC4_RXD_SDA_MOSI_DATA/PLU_OUT3, mode: inactive,
    slew_rate: standard, invert: disabled, open_drain: disabled}
  - {pin_num: '5', peripheral: GPIO, signal: 'PIO1, 6', pin_signal: PIO1_6/FC0_TXD_SCL_MISO_WS/SD0_D3/CTIMER2_MAT1/SCT_GPI3, mode: inactive, slew_rate: standard,
    invert: disabled, open_drain: disabled}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
/* Function assigned for the Cortex-M33 (Core #0) */
void BOARD_InitPins(void)
{
    /* Enables the clock for the I/O controller.: Enable Clock. */
    CLOCK_EnableClock(kCLOCK_Iocon);

    const uint32_t port0_pin29_config = (/* Pin is configured as FC0_RXD_SDA_MOSI_DATA */
                                         IOCON_PIO_FUNC1 |
                                         /* No addition pin function */
                                         IOCON_PIO_MODE_INACT |
                                         /* Standard mode, output slew rate control is enabled */
                                         IOCON_PIO_SLEW_STANDARD |
                                         /* Input function is not inverted */
                                         IOCON_PIO_INV_DI |
                                         /* Enables digital function */
                                         IOCON_PIO_DIGITAL_EN |
                                         /* Open drain is disabled */
                                         IOCON_PIO_OPENDRAIN_DI);
    /* PORT0 PIN29 (coords: 92) is configured as FC0_RXD_SDA_MOSI_DATA */
    IOCON_PinMuxSet(IOCON, 0U, 29U, port0_pin29_config);

    const uint32_t port0_pin30_config = (/* Pin is configured as FC0_TXD_SCL_MISO_WS */
                                         IOCON_PIO_FUNC1 |
                                         /* No addition pin function */
                                         IOCON_PIO_MODE_INACT |
                                         /* Standard mode, output slew rate control is enabled */
                                         IOCON_PIO_SLEW_STANDARD |
                                         /* Input function is not inverted */
                                         IOCON_PIO_INV_DI |
                                         /* Enables digital function */
                                         IOCON_PIO_DIGITAL_EN |
                                         /* Open drain is disabled */
                                         IOCON_PIO_OPENDRAIN_DI);
    /* PORT0 PIN30 (coords: 94) is configured as FC0_TXD_SCL_MISO_WS */
    IOCON_PinMuxSet(IOCON, 0U, 30U, port0_pin30_config);

    const uint32_t port1_pin20_config = (/* Pin is configured as FC4_TXD_SCL_MISO_WS */
                                         IOCON_PIO_FUNC5 |
                                         /* No addition pin function */
                                         IOCON_PIO_MODE_INACT |
                                         /* Standard mode, output slew rate control is enabled */
                                         IOCON_PIO_SLEW_STANDARD |
                                         /* Input function is not inverted */
                                         IOCON_PIO_INV_DI |
                                         /* Enables digital function */
                                         IOCON_PIO_DIGITAL_EN |
                                         /* Open drain is disabled */
                                         IOCON_PIO_OPENDRAIN_DI);
    /* PORT1 PIN20 (coords: 4) is configured as FC4_TXD_SCL_MISO_WS */
    IOCON_PinMuxSet(IOCON, 1U, 20U, port1_pin20_config);

    const uint32_t port1_pin21_config = (/* Pin is configured as FC4_RXD_SDA_MOSI_DATA */
                                         IOCON_PIO_FUNC5 |
                                         /* No addition pin function */
                                         IOCON_PIO_MODE_INACT |
                                         /* Standard mode, output slew rate control is enabled */
                                         IOCON_PIO_SLEW_STANDARD |
                                         /* Input function is not inverted */
                                         IOCON_PIO_INV_DI |
                                         /* Enables digital function */
                                         IOCON_PIO_DIGITAL_EN |
                                         /* Open drain is disabled */
                                         IOCON_PIO_OPENDRAIN_DI);
    /* PORT1 PIN21 (coords: 30) is configured as FC4_RXD_SDA_MOSI_DATA */
    IOCON_PinMuxSet(IOCON, 1U, 21U, port1_pin21_config);

    const uint32_t port1_pin6_config = (/* Pin is configured as PIO1_6 */
                                        IOCON_PIO_FUNC0 |
                                        /* No addition pin function */
                                        IOCON_PIO_MODE_INACT |
                                        /* Standard mode, output slew rate control is enabled */
                                        IOCON_PIO_SLEW_STANDARD |
                                        /* Input function is not inverted */
                                        IOCON_PIO_INV_DI |
                                        /* Enables digital function */
                                        IOCON_PIO_DIGITAL_EN |
                                        /* Open drain is disabled */
                                        IOCON_PIO_OPENDRAIN_DI);
    /* PORT1 PIN6 (coords: 5) is configured as PIO1_6 */
    IOCON_PinMuxSet(IOCON, 1U, 6U, port1_pin6_config);
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitGT202Shield:
- options: {callFromInitBoot: 'false', coreID: cm33_core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '62', peripheral: FLEXCOMM8, signal: HS_SPI_MISO, pin_signal: PIO1_3/SCT0_OUT4/HS_SPI_MISO/USB0_PORTPWRN/PLU_OUT6, mode: inactive, slew_rate: fast,
    invert: disabled, open_drain: disabled}
  - {pin_num: '60', peripheral: FLEXCOMM8, signal: HS_SPI_MOSI, pin_signal: PIO0_26/FC2_RXD_SDA_MOSI_DATA/CLKOUT/CT_INP14/SCT0_OUT5/USB0_IDVALUE/FC0_SCK/HS_SPI_MOSI/SECURE_GPIO0_26,
    mode: inactive, slew_rate: fast, invert: disabled, open_drain: disabled}
  - {pin_num: '61', peripheral: FLEXCOMM8, signal: HS_SPI_SCK, pin_signal: PIO1_2/CTIMER0_MAT3/SCT_GPI6/HS_SPI_SCK/USB1_PORTPWRN/PLU_OUT5, mode: inactive, slew_rate: fast,
    invert: disabled, open_drain: disabled}
  - {pin_num: '59', peripheral: FLEXCOMM8, signal: HS_SPI_SSEL1, pin_signal: PIO1_1/FC3_RXD_SDA_MOSI_DATA/CT_INP3/SCT_GPI5/HS_SPI_SSEL1/USB1_OVERCURRENTN/PLU_OUT4,
    mode: inactive, slew_rate: fast, invert: disabled, open_drain: disabled}
  - {pin_num: '24', peripheral: GPIO, signal: 'PIO1, 8', pin_signal: PIO1_8/FC0_CTS_SDA_SSEL0/SD0_CLK/SCT0_OUT1/FC4_SSEL2/ADC0_4, direction: OUTPUT, gpio_init_state: 'true',
    mode: pullDown, slew_rate: standard, invert: disabled, open_drain: disabled, asw: disabled}
  - {pin_num: '10', peripheral: GPIO, signal: 'PIO1, 9', pin_signal: PIO1_9/FC1_SCK/CT_INP4/SCT0_OUT2/FC4_CTS_SDA_SSEL0/ADC0_12, direction: INPUT, mode: pullUp, slew_rate: standard,
    invert: disabled, open_drain: disabled, asw: disabled}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitGT202Shield
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
/* Function assigned for the Cortex-M33 (Core #0) */
void BOARD_InitGT202Shield(void)
{
    /* Enables the clock for the I/O controller.: Enable Clock. */
    CLOCK_EnableClock(kCLOCK_Iocon);

    /* Enables the clock for the GPIO1 module */
    CLOCK_EnableClock(kCLOCK_Gpio1);

    gpio_pin_config_t gpio1_pin24_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 1U
    };
    /* Initialize GPIO functionality on pin PIO1_8 (pin 24)  */
    GPIO_PinInit(GPIO, 1U, 8U, &gpio1_pin24_config);

    gpio_pin_config_t gpio1_pin10_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_9 (pin 10)  */
    GPIO_PinInit(GPIO, 1U, 9U, &gpio1_pin10_config);

    const uint32_t port0_pin26_config = (/* Pin is configured as HS_SPI_MOSI */
                                         IOCON_PIO_FUNC9 |
                                         /* No addition pin function */
                                         IOCON_PIO_MODE_INACT |
                                         /* Fast mode, slew rate control is disabled */
                                         IOCON_PIO_SLEW_FAST |
                                         /* Input function is not inverted */
                                         IOCON_PIO_INV_DI |
                                         /* Enables digital function */
                                         IOCON_PIO_DIGITAL_EN |
                                         /* Open drain is disabled */
                                         IOCON_PIO_OPENDRAIN_DI);
    /* PORT0 PIN26 (coords: 60) is configured as HS_SPI_MOSI */
    IOCON_PinMuxSet(IOCON, 0U, 26U, port0_pin26_config);

    const uint32_t port1_pin1_config = (/* Pin is configured as HS_SPI_SSEL1 */
                                        IOCON_PIO_FUNC5 |
                                        /* No addition pin function */
                                        IOCON_PIO_MODE_INACT |
                                        /* Fast mode, slew rate control is disabled */
                                        IOCON_PIO_SLEW_FAST |
                                        /* Input function is not inverted */
                                        IOCON_PIO_INV_DI |
                                        /* Enables digital function */
                                        IOCON_PIO_DIGITAL_EN |
                                        /* Open drain is disabled */
                                        IOCON_PIO_OPENDRAIN_DI);
    /* PORT1 PIN1 (coords: 59) is configured as HS_SPI_SSEL1 */
    IOCON_PinMuxSet(IOCON, 1U, 1U, port1_pin1_config);

    const uint32_t port1_pin2_config = (/* Pin is configured as HS_SPI_SCK */
                                        IOCON_PIO_FUNC6 |
                                        /* No addition pin function */
                                        IOCON_PIO_MODE_INACT |
                                        /* Fast mode, slew rate control is disabled */
                                        IOCON_PIO_SLEW_FAST |
                                        /* Input function is not inverted */
                                        IOCON_PIO_INV_DI |
                                        /* Enables digital function */
                                        IOCON_PIO_DIGITAL_EN |
                                        /* Open drain is disabled */
                                        IOCON_PIO_OPENDRAIN_DI);
    /* PORT1 PIN2 (coords: 61) is configured as HS_SPI_SCK */
    IOCON_PinMuxSet(IOCON, 1U, 2U, port1_pin2_config);

    const uint32_t port1_pin3_config = (/* Pin is configured as HS_SPI_MISO */
                                        IOCON_PIO_FUNC6 |
                                        /* No addition pin function */
                                        IOCON_PIO_MODE_INACT |
                                        /* Fast mode, slew rate control is disabled */
                                        IOCON_PIO_SLEW_FAST |
                                        /* Input function is not inverted */
                                        IOCON_PIO_INV_DI |
                                        /* Enables digital function */
                                        IOCON_PIO_DIGITAL_EN |
                                        /* Open drain is disabled */
                                        IOCON_PIO_OPENDRAIN_DI);
    /* PORT1 PIN3 (coords: 62) is configured as HS_SPI_MISO */
    IOCON_PinMuxSet(IOCON, 1U, 3U, port1_pin3_config);

    const uint32_t port1_pin8_config = (/* Pin is configured as PIO1_8 */
                                        IOCON_PIO_FUNC0 |
                                        /* Selects pull-down function */
                                        IOCON_PIO_MODE_PULLDOWN |
                                        /* Standard mode, output slew rate control is enabled */
                                        IOCON_PIO_SLEW_STANDARD |
                                        /* Input function is not inverted */
                                        IOCON_PIO_INV_DI |
                                        /* Enables digital function */
                                        IOCON_PIO_DIGITAL_EN |
                                        /* Open drain is disabled */
                                        IOCON_PIO_OPENDRAIN_DI |
                                        /* Analog switch is disabled */
                                        IOCON_PIO_ASW_DIS_DI);
    /* PORT1 PIN8 (coords: 24) is configured as PIO1_8 */
    IOCON_PinMuxSet(IOCON, 1U, 8U, port1_pin8_config);

    const uint32_t port1_pin9_config = (/* Pin is configured as PIO1_9 */
                                        IOCON_PIO_FUNC0 |
                                        /* Selects pull-up function */
                                        IOCON_PIO_MODE_PULLUP |
                                        /* Standard mode, output slew rate control is enabled */
                                        IOCON_PIO_SLEW_STANDARD |
                                        /* Input function is not inverted */
                                        IOCON_PIO_INV_DI |
                                        /* Enables digital function */
                                        IOCON_PIO_DIGITAL_EN |
                                        /* Open drain is disabled */
                                        IOCON_PIO_OPENDRAIN_DI |
                                        /* Analog switch is disabled */
                                        IOCON_PIO_ASW_DI);
    /* PORT1 PIN9 (coords: 10) is configured as PIO1_9 */
    IOCON_PinMuxSet(IOCON, 1U, 9U, port1_pin9_config);
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitSilex2401Shield:
- options: {callFromInitBoot: 'false', coreID: cm33_core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '62', peripheral: FLEXCOMM8, signal: HS_SPI_MISO, pin_signal: PIO1_3/SCT0_OUT4/HS_SPI_MISO/USB0_PORTPWRN/PLU_OUT6, mode: inactive, slew_rate: fast,
    invert: disabled, open_drain: disabled}
  - {pin_num: '61', peripheral: FLEXCOMM8, signal: HS_SPI_SCK, pin_signal: PIO1_2/CTIMER0_MAT3/SCT_GPI6/HS_SPI_SCK/USB1_PORTPWRN/PLU_OUT5, mode: inactive, slew_rate: fast,
    invert: disabled, open_drain: disabled}
  - {pin_num: '60', peripheral: FLEXCOMM8, signal: HS_SPI_MOSI, pin_signal: PIO0_26/FC2_RXD_SDA_MOSI_DATA/CLKOUT/CT_INP14/SCT0_OUT5/USB0_IDVALUE/FC0_SCK/HS_SPI_MOSI/SECURE_GPIO0_26,
    mode: inactive, slew_rate: fast, invert: disabled, open_drain: disabled}
  - {pin_num: '59', peripheral: FLEXCOMM8, signal: HS_SPI_SSEL1, pin_signal: PIO1_1/FC3_RXD_SDA_MOSI_DATA/CT_INP3/SCT_GPI5/HS_SPI_SSEL1/USB1_OVERCURRENTN/PLU_OUT4,
    mode: inactive, slew_rate: fast, invert: disabled, open_drain: disabled}
  - {pin_num: '14', peripheral: GPIO, signal: 'PIO0, 16', pin_signal: PIO0_16/FC4_TXD_SCL_MISO_WS/CLKOUT/CT_INP4/SECURE_GPIO0_16/ADC0_8, direction: OUTPUT, mode: pullDown,
    slew_rate: standard, invert: disabled, open_drain: disabled}
  - {pin_num: '31', peripheral: GPIO, signal: 'PIO1, 5', pin_signal: PIO1_5/FC0_RXD_SDA_MOSI_DATA/SD0_D2/CTIMER2_MAT0/SCT_GPI0, direction: OUTPUT, mode: pullDown,
    slew_rate: standard, invert: disabled, open_drain: disabled}
  - {pin_num: '64', peripheral: GPIO, signal: 'PIO1, 18', pin_signal: PIO1_18/SD1_POW_EN/SCT0_OUT5/PLU_OUT0, direction: INPUT, mode: pullUp, slew_rate: standard,
    invert: disabled, open_drain: disabled}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitSilex2401Shield
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
/* Function assigned for the Cortex-M33 (Core #0) */
void BOARD_InitSilex2401Shield(void)
{
    /* Enables the clock for the I/O controller.: Enable Clock. */
    CLOCK_EnableClock(kCLOCK_Iocon);

    /* Enables the clock for the GPIO0 module */
    CLOCK_EnableClock(kCLOCK_Gpio0);

    /* Enables the clock for the GPIO1 module */
    CLOCK_EnableClock(kCLOCK_Gpio1);

    gpio_pin_config_t KFETON_ID_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO0_16 (pin 14)  */
    GPIO_PinInit(BOARD_INITSILEX2401SHIELD_KFETON_ID_GPIO, BOARD_INITSILEX2401SHIELD_KFETON_ID_PORT, BOARD_INITSILEX2401SHIELD_KFETON_ID_PIN, &KFETON_ID_config);

    gpio_pin_config_t PWRON_ID_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_5 (pin 31)  */
    GPIO_PinInit(BOARD_INITSILEX2401SHIELD_PWRON_ID_GPIO, BOARD_INITSILEX2401SHIELD_PWRON_ID_PORT, BOARD_INITSILEX2401SHIELD_PWRON_ID_PIN, &PWRON_ID_config);

    gpio_pin_config_t IRQ_ID_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PIO1_18 (pin 64)  */
    GPIO_PinInit(BOARD_INITSILEX2401SHIELD_IRQ_ID_GPIO, BOARD_INITSILEX2401SHIELD_IRQ_ID_PORT, BOARD_INITSILEX2401SHIELD_IRQ_ID_PIN, &IRQ_ID_config);

    IOCON->PIO[0][16] =
        ((IOCON->PIO[0][16] &
          /* Mask bits to zero which are setting */
          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_MODE_MASK | IOCON_PIO_SLEW_MASK | IOCON_PIO_INVERT_MASK | IOCON_PIO_DIGIMODE_MASK | IOCON_PIO_OD_MASK)))

         /* Selects pin function.
          * : PORT016 (pin 14) is configured as PIO0_16. */
         | IOCON_PIO_FUNC(PIO0_16_FUNC_ALT0)

         /* Selects function mode (on-chip pull-up/pull-down resistor control).
          * : Pull-down.
          * Pull-down resistor enabled. */
         | IOCON_PIO_MODE(PIO0_16_MODE_PULL_DOWN)

         /* Driver slew rate.
          * : Standard mode, output slew rate control is enabled.
          * More outputs can be switched simultaneously. */
         | IOCON_PIO_SLEW(PIO0_16_SLEW_STANDARD)

         /* Input polarity.
          * : Disabled.
          * Input function is not inverted. */
         | IOCON_PIO_INVERT(PIO0_16_INVERT_DISABLED)

         /* Select Digital mode.
          * : Digital mode, digital input is enabled. */
         | IOCON_PIO_DIGIMODE(PIO0_16_DIGIMODE_DIGITAL)

         /* Controls open-drain mode.
          * : Normal.
          * Normal push-pull output. */
         | IOCON_PIO_OD(PIO0_16_OD_NORMAL));

    const uint32_t port0_pin26_config = (/* Pin is configured as HS_SPI_MOSI */
                                         IOCON_PIO_FUNC9 |
                                         /* No addition pin function */
                                         IOCON_PIO_MODE_INACT |
                                         /* Fast mode, slew rate control is disabled */
                                         IOCON_PIO_SLEW_FAST |
                                         /* Input function is not inverted */
                                         IOCON_PIO_INV_DI |
                                         /* Enables digital function */
                                         IOCON_PIO_DIGITAL_EN |
                                         /* Open drain is disabled */
                                         IOCON_PIO_OPENDRAIN_DI);
    /* PORT0 PIN26 (coords: 60) is configured as HS_SPI_MOSI */
    IOCON_PinMuxSet(IOCON, 0U, 26U, port0_pin26_config);

    const uint32_t port1_pin1_config = (/* Pin is configured as HS_SPI_SSEL1 */
                                        IOCON_PIO_FUNC5 |
                                        /* No addition pin function */
                                        IOCON_PIO_MODE_INACT |
                                        /* Fast mode, slew rate control is disabled */
                                        IOCON_PIO_SLEW_FAST |
                                        /* Input function is not inverted */
                                        IOCON_PIO_INV_DI |
                                        /* Enables digital function */
                                        IOCON_PIO_DIGITAL_EN |
                                        /* Open drain is disabled */
                                        IOCON_PIO_OPENDRAIN_DI);
    /* PORT1 PIN1 (coords: 59) is configured as HS_SPI_SSEL1 */
    IOCON_PinMuxSet(IOCON, 1U, 1U, port1_pin1_config);

    const uint32_t IRQ_ID = (/* Pin is configured as PIO1_18 */
                             IOCON_PIO_FUNC0 |
                             /* Selects pull-up function */
                             IOCON_PIO_MODE_PULLUP |
                             /* Standard mode, output slew rate control is enabled */
                             IOCON_PIO_SLEW_STANDARD |
                             /* Input function is not inverted */
                             IOCON_PIO_INV_DI |
                             /* Enables digital function */
                             IOCON_PIO_DIGITAL_EN |
                             /* Open drain is disabled */
                             IOCON_PIO_OPENDRAIN_DI);
    /* PORT1 PIN18 (coords: 64) is configured as PIO1_18 */
    IOCON_PinMuxSet(IOCON, BOARD_INITSILEX2401SHIELD_IRQ_ID_PORT, BOARD_INITSILEX2401SHIELD_IRQ_ID_PIN, IRQ_ID);

    const uint32_t port1_pin2_config = (/* Pin is configured as HS_SPI_SCK */
                                        IOCON_PIO_FUNC6 |
                                        /* No addition pin function */
                                        IOCON_PIO_MODE_INACT |
                                        /* Fast mode, slew rate control is disabled */
                                        IOCON_PIO_SLEW_FAST |
                                        /* Input function is not inverted */
                                        IOCON_PIO_INV_DI |
                                        /* Enables digital function */
                                        IOCON_PIO_DIGITAL_EN |
                                        /* Open drain is disabled */
                                        IOCON_PIO_OPENDRAIN_DI);
    /* PORT1 PIN2 (coords: 61) is configured as HS_SPI_SCK */
    IOCON_PinMuxSet(IOCON, 1U, 2U, port1_pin2_config);

    const uint32_t port1_pin3_config = (/* Pin is configured as HS_SPI_MISO */
                                        IOCON_PIO_FUNC6 |
                                        /* No addition pin function */
                                        IOCON_PIO_MODE_INACT |
                                        /* Fast mode, slew rate control is disabled */
                                        IOCON_PIO_SLEW_FAST |
                                        /* Input function is not inverted */
                                        IOCON_PIO_INV_DI |
                                        /* Enables digital function */
                                        IOCON_PIO_DIGITAL_EN |
                                        /* Open drain is disabled */
                                        IOCON_PIO_OPENDRAIN_DI);
    /* PORT1 PIN3 (coords: 62) is configured as HS_SPI_MISO */
    IOCON_PinMuxSet(IOCON, 1U, 3U, port1_pin3_config);

    const uint32_t PWRON_ID = (/* Pin is configured as PIO1_5 */
                               IOCON_PIO_FUNC0 |
                               /* Selects pull-down function */
                               IOCON_PIO_MODE_PULLDOWN |
                               /* Standard mode, output slew rate control is enabled */
                               IOCON_PIO_SLEW_STANDARD |
                               /* Input function is not inverted */
                               IOCON_PIO_INV_DI |
                               /* Enables digital function */
                               IOCON_PIO_DIGITAL_EN |
                               /* Open drain is disabled */
                               IOCON_PIO_OPENDRAIN_DI);
    /* PORT1 PIN5 (coords: 31) is configured as PIO1_5 */
    IOCON_PinMuxSet(IOCON, BOARD_INITSILEX2401SHIELD_PWRON_ID_PORT, BOARD_INITSILEX2401SHIELD_PWRON_ID_PIN, PWRON_ID);
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitSequansSkywireShield:
- options: {callFromInitBoot: 'false', coreID: cm33_core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '27', peripheral: FLEXCOMM2, signal: TXD_SCL_MISO_WS, pin_signal: PIO0_27/FC2_TXD_SCL_MISO_WS/CTIMER3_MAT2/SCT0_OUT6/FC7_RXD_SDA_MOSI_DATA/PLU_OUT0/SECURE_GPIO0_27}
  - {pin_num: '3', peripheral: FLEXCOMM2, signal: RXD_SDA_MOSI_DATA, pin_signal: PIO1_24/FC2_RXD_SDA_MOSI_DATA/SCT0_OUT1/SD1_D1/FC3_SSEL3/PLU_OUT6}
  - {pin_num: '68', peripheral: FLEXCOMM2, signal: CTS_SDA_SSEL0, pin_signal: PIO1_26/FC2_CTS_SDA_SSEL0/SCT0_OUT3/CT_INP3/UTICK_CAP1/HS_SPI_SSEL3/PLU_IN5}
  - {pin_num: '85', peripheral: FLEXCOMM2, signal: RTS_SCL_SSEL1, pin_signal: PIO1_27/FC2_RTS_SCL_SSEL1/SD0_D4/CTIMER0_MAT3/CLKOUT/PLU_IN4}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitSequansSkywireShield
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
/* Function assigned for the Cortex-M33 (Core #0) */
void BOARD_InitSequansSkywireShield(void)
{
    /* Enables the clock for the I/O controller.: Enable Clock. */
    CLOCK_EnableClock(kCLOCK_Iocon);

    IOCON->PIO[0][27] = ((IOCON->PIO[0][27] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT027 (pin 27) is configured as FC2_TXD_SCL_MISO_WS. */
                         | IOCON_PIO_FUNC(PIO0_27_FUNC_ALT1)

                         /* Select Digital mode.
                          * : Digital mode, digital input is enabled. */
                         | IOCON_PIO_DIGIMODE(PIO0_27_DIGIMODE_DIGITAL));

    IOCON->PIO[1][24] = ((IOCON->PIO[1][24] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT124 (pin 3) is configured as FC2_RXD_SDA_MOSI_DATA. */
                         | IOCON_PIO_FUNC(PIO1_24_FUNC_ALT1)

                         /* Select Digital mode.
                          * : Digital mode, digital input is enabled. */
                         | IOCON_PIO_DIGIMODE(PIO1_24_DIGIMODE_DIGITAL));

    IOCON->PIO[1][26] = ((IOCON->PIO[1][26] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK | IOCON_PIO_FUNC(1))))

                         /* Selects pin function.
                          * : PORT126 (pin 68) is configured as FC2_CTS_SDA_SSEL0. */
                         | IOCON_PIO_FUNC(PIO1_26_FUNC_ALT1)

                         /* Select Digital mode.
                          * : Digital mode, digital input is enabled. */
                         | IOCON_PIO_DIGIMODE(PIO1_26_DIGIMODE_DIGITAL));

    IOCON->PIO[1][27] = ((IOCON->PIO[1][27] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))

                         /* Selects pin function.
                          * : PORT127 (pin 85) is configured as FC2_RTS_SCL_SSEL1. */
                         | IOCON_PIO_FUNC(PIO1_27_FUNC_ALT1)

                         /* Select Digital mode.
                          * : Digital mode, digital input is enabled. */
                         | IOCON_PIO_DIGIMODE(PIO1_27_DIGIMODE_DIGITAL));

}
/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
