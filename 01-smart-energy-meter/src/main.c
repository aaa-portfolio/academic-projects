#include "driverlib.h"
#include "device.h"
#include "thd_config.h"
#include <stdio.h>

/* -------- Raw ADC buffers (filled by ISR) -------- */
uint16_t voltage_raw_buffer[TOTAL_SAMPLES];
uint16_t current_raw_buffer[TOTAL_SAMPLES];

/* -------- Shared flags -------- */
volatile uint16_t sample_count      = 0;
volatile uint8_t  sampling_complete = 0;
volatile uint32_t send_count = 0;




/* -------- Result struct (global, pre-initialized) -------- */
THD_Result_t thd_result = {0};

/* -------- Prototypes -------- */
__interrupt void adcA1ISR(void);
void SCI_SendHarmonics(void);
static void send_float(float val);
void SCI_SendBinary(void);

/* ========================================================================= */
void main(void)
{
    Device_init();
    Device_initGPIO();
    Interrupt_initModule();
    Interrupt_initVectorTable();

    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_ADCA);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_ADCB);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM4);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_SCIA);

    Interrupt_register(INT_ADCA1, &adcA1ISR);

    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    EALLOW;

    /* ---- EPWM4: 10 kHz SOC trigger ---- */
    EPWM_setClockPrescaler(EPWM4_BASE, EPWM_CLOCK_DIVIDER_1,
                                        EPWM_HSCLOCK_DIVIDER_1);
    EPWM_setTimeBasePeriod(EPWM4_BASE, 7813);
    EPWM_setTimeBaseCounter(EPWM4_BASE, 0);
    EPWM_setTimeBaseCounterMode(EPWM4_BASE, EPWM_COUNTER_MODE_UP_DOWN);
    EPWM_disablePhaseShiftLoad(EPWM4_BASE);
    EPWM_setPhaseShift(EPWM4_BASE, 0);
    EPWM_enableADCTrigger(EPWM4_BASE, EPWM_SOC_A);
    EPWM_setADCTriggerSource(EPWM4_BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_ZERO);
    EPWM_setADCTriggerEventPrescale(EPWM4_BASE, EPWM_SOC_A, 1);

    /* ---- ADCA: voltage on ADCIN0 ---- */
    ADC_setVREF(ADCA_BASE, ADC_REFERENCE_INTERNAL, ADC_REFERENCE_3_3V);
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_2_0);
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCA_BASE);
    DEVICE_DELAY_US(5000);
    ADC_disableBurstMode(ADCA_BASE);
    ADC_setSOCPriority(ADCA_BASE, ADC_PRI_ALL_ROUND_ROBIN);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM4_SOCA, ADC_CH_ADCIN0, 8U);
    ADC_setInterruptSOCTrigger(ADCA_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_NONE);
    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_disableContinuousMode(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);

    /* ---- ADCB: current on ADCIN4 ---- */
    ADC_setVREF(ADCB_BASE, ADC_REFERENCE_INTERNAL, ADC_REFERENCE_3_3V);
    ADC_setPrescaler(ADCB_BASE, ADC_CLK_DIV_2_0);
    ADC_setInterruptPulseMode(ADCB_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCB_BASE);
    DEVICE_DELAY_US(5000);
    ADC_disableBurstMode(ADCB_BASE);
    ADC_setSOCPriority(ADCB_BASE, ADC_PRI_ALL_ROUND_ROBIN);
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER0,
                 ADC_TRIGGER_EPWM4_SOCA, ADC_CH_ADCIN4, 8U);
    ADC_setInterruptSOCTrigger(ADCB_BASE, ADC_SOC_NUMBER0,
                               ADC_INT_SOC_TRIGGER_NONE);

    /* ---- SCIA: TX on GPIO29 ---- */
    GPIO_setPinConfig(GPIO_28_SCIA_RX);
    GPIO_setPinConfig(GPIO_29_SCIA_TX);
    GPIO_setPadConfig(28, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(29, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(28, GPIO_QUAL_ASYNC);
    GPIO_setQualificationMode(29, GPIO_QUAL_ASYNC);
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF | SCI_INT_TXFF |
                                 SCI_INT_FE | SCI_INT_OE | SCI_INT_PE |
                                 SCI_INT_RXERR | SCI_INT_RXRDY_BRKDT | SCI_INT_TXRDY);
    SCI_clearOverflowStatus(SCIA_BASE);
    SCI_resetTxFIFO(SCIA_BASE);
    SCI_resetRxFIFO(SCIA_BASE);
    SCI_resetChannels(SCIA_BASE);
    SCI_setConfig(SCIA_BASE, DEVICE_LSPCLK_FREQ, 9600,
                      (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));
    SCI_performSoftwareReset(SCIA_BASE);
    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX0, SCI_FIFO_RX0);
    SCI_enableFIFO(SCIA_BASE);
    SCI_enableModule(SCIA_BASE);

    EDIS;

    DFT_Init_Tables();

    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    /* Clear any stale ADC interrupt flags left over from previous programming */
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER1);

    Interrupt_enable(INT_ADCA1);
    EINT; ERTM;

    DEVICE_DELAY_US(1000);


    for (;;)
    {
        if (sampling_complete)
        {
            //SCI_writeCharBlockingFIFO(SCIA_BASE, 0x55);
            Process_Samples();
            //SCI_writeCharBlockingFIFO(SCIA_BASE, 0x66);
            send_count++;
            SCI_SendBinary();
            sample_count = 0;
            sampling_complete = 0;
        }
    }
}

/* ------------------------------------------------------------------------- */

__interrupt void adcA1ISR(void)
{
    if (ADC_getInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER1))
            {
                ADC_clearInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER1);
                ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
            }

    if (!sampling_complete && sample_count < TOTAL_SAMPLES)
    {
        voltage_raw_buffer[sample_count] =
            ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
        current_raw_buffer[sample_count] =
            ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER0);
        sample_count++;

        if (sample_count >= TOTAL_SAMPLES) sampling_complete = 1;
    }

    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

/* ------------------------------------------------------------------------- */

static inline void send_float(float val)
{
    union { float f; uint8_t b[4]; } u;
    u.f = val;
    SCI_writeCharBlockingFIFO(SCIA_BASE, u.b[3]);
    SCI_writeCharBlockingFIFO(SCIA_BASE, u.b[2]);
    SCI_writeCharBlockingFIFO(SCIA_BASE, u.b[1]);
    SCI_writeCharBlockingFIFO(SCIA_BASE, u.b[0]);
}

void SCI_SendBinary(void)
{
    SCI_writeCharBlockingFIFO(SCIA_BASE, 0xAA);  /* sync byte */

    send_float(thd_result.voltage_rms);
    send_float(thd_result.current_rms);
    send_float(thd_result.voltage_thd);
    send_float(thd_result.current_thd);
    send_float(thd_result.power_factor);
    send_float(thd_result.active_power);
    send_float(thd_result.reactive_power);

    SCI_writeCharBlockingFIFO(SCIA_BASE, 0xBB);  /* end marker */
}

