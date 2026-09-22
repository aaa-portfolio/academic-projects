//#############################################################################
//
// FILE: epwm_ex2_updown_aq.c
//
// TITLE: ePWM Action Qualifier Module - Using up/down count.
//
//! \addtogroup driver_example_list
//! <h1> ePWM Up Down Count Action Qualifier</h1>
//!
//! This example configures ePWM1, ePWM2, ePWM3 to produce a waveform with//! independent modulation on ePWMxA and ePWMxB.
//!
//! The compare values CMPA and CMPB are modified within the ePWM's ISR.
//!
//! The TB counter is in up/down count mode for this example.
//!
//! ePWM4A and ePWM4B is used
//
//
// Included Files
//
#include "driverlib.h"
#include "device.h"
//
// Defines
//
#define myADC0_BASE ADCA_BASE
#define GPIO_PIN_EPWM4_A 6
#define myEPWM4_EPWMA_GPIO 6
#define myEPWM4_EPWMA_PIN_CONFIG GPIO_6_EPWM4_A
#define GPIO_PIN_EPWM4_B 7
#define myEPWM4_EPWMB_GPIO 7
#define myEPWM4_EPWMB_PIN_CONFIG GPIO_7_EPWM4_B
#define myEPWM4_BASE EPWM4_BASE
//
// Globals
//
uint16_t cmpVal;
__interrupt void epwm4ISR(void);
//
// Main
//
void main(void)
{
 //
 // Initialize device clock and peripherals
 //
 Device_init();
 //
 // Disable pin locks and enable internal pull ups.
 //
 Device_initGPIO();
 //
 // Initialize PIE and clear PIE registers. Disables CPU interrupts.
 //
 Interrupt_initModule();
 //
 // Initialize the PIE vector table with pointers to the shell Interrupt
 // Service Routines (ISR).
 //
 Interrupt_initVectorTable();
 //
 // Assign the interrupt service routines to ePWM interrupts
 //
 Interrupt_register(INT_ADCA1, &epwm4ISR);
 //
 // Disable sync(Freeze clock to PWM as well)
 //
 SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
 EALLOW;
 // EPWM3A Initialisation
 //GPIO_setPinConfig(myEPWM3_EPWMA_PIN_CONFIG);
 //GPIO_setPadConfig(myEPWM3_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
 //GPIO_setQualificationMode(myEPWM3_EPWMA_GPIO, GPIO_QUAL_SYNC);
 // EPWM4A Initialization
 GPIO_setPinConfig(myEPWM4_EPWMA_PIN_CONFIG);
GPIO_setPadConfig(myEPWM4_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
GPIO_setQualificationMode(myEPWM4_EPWMA_GPIO, GPIO_QUAL_SYNC);
 // EPWM4B Initialization
 GPIO_setPinConfig(myEPWM4_EPWMB_PIN_CONFIG);
GPIO_setPadConfig(myEPWM4_EPWMB_GPIO, GPIO_PIN_TYPE_STD);
GPIO_setQualificationMode(myEPWM4_EPWMB_GPIO, GPIO_QUAL_SYNC);
 EPWM_setClockPrescaler(myEPWM4_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
 EPWM_setTimeBasePeriod(myEPWM4_BASE, 1250); // SET TBPRD HERE EPWM_setTimeBaseCounter(myEPWM4_BASE, 0);
 EPWM_setTimeBaseCounterMode(myEPWM4_BASE,
EPWM_COUNTER_MODE_UP_DOWN);
 EPWM_disablePhaseShiftLoad(myEPWM4_BASE);
 EPWM_setPhaseShift(myEPWM4_BASE, 0);
 EPWM_setCounterCompareValue(myEPWM4_BASE,
EPWM_COUNTER_COMPARE_A, 625); // SET INITIAL CMPA VALUE HERE EPWM_setCounterCompareShadowLoadMode(myEPWM4_BASE,
EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);
 //EPWM_setCounterCompareValue(myEPWM3_BASE,
EPWM_COUNTER_COMPARE_B, 50);
 //EPWM_setCounterCompareShadowLoadMode(myEPWM3_BASE,
EPWM_COUNTER_COMPARE_B, EPWM_COMP_LOAD_ON_CNTR_ZERO);
 EPWM_disableActionQualifierShadowLoadMode(myEPWM4_BASE,
EPWM_ACTION_QUALIFIER_A);
 EPWM_setActionQualifierShadowLoadMode(myEPWM4_BASE,
EPWM_ACTION_QUALIFIER_A, EPWM_AQ_LOAD_ON_CNTR_ZERO);
 EPWM_setActionQualifierAction(myEPWM4_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
 EPWM_setActionQualifierAction(myEPWM4_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
 EPWM_setActionQualifierAction(myEPWM4_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
 EPWM_setActionQualifierAction(myEPWM4_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
 //EPWM_setActionQualifierAction(myEPWM3_BASE, EPWM_AQ_OUTPUT_A,
EPWM_AQ_OUTPUT_NO_CHANGE,
EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
 //EPWM_setActionQualifierAction(myEPWM3_BASE, EPWM_AQ_OUTPUT_A,
EPWM_AQ_OUTPUT_NO_CHANGE,
EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);
 EPWM_setDeadBandDelayPolarity(myEPWM4_BASE, EPWM_DB_RED, EPWM_DB_POLARITY_ACTIVE_HIGH);
 EPWM_setDeadBandDelayPolarity(myEPWM4_BASE, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);
 EPWM_setDeadBandDelayMode(myEPWM4_BASE, EPWM_DB_RED, true);
 //EPWM_setRisingEdgeDelayCountShadowLoadMode(myEPWM4_BASE,
EPWM_RED_LOAD_ON_CNTR_ZERO);
 EPWM_disableRisingEdgeDelayCountShadowLoadMode(myEPWM4_BASE);
 EPWM_setRisingEdgeDelayCount(myEPWM4_BASE, 50); // SET DEADBAND
HERE
 EPWM_setDeadBandDelayMode(myEPWM4_BASE, EPWM_DB_FED, true);
 //EPWM_setFallingEdgeDelayCountShadowLoadMode(myEPWM4_BASE,
EPWM_FED_LOAD_ON_CNTR_ZERO);
 EPWM_disableFallingEdgeDelayCountShadowLoadMode(myEPWM4_BASE);
 EPWM_setFallingEdgeDelayCount(myEPWM4_BASE, 50); // SET DEADBAND
HERE
 EPWM_enableADCTrigger(myEPWM4_BASE, EPWM_SOC_A);
 EPWM_setADCTriggerSource(myEPWM4_BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_ZERO);
 EPWM_setADCTriggerEventPrescale(myEPWM4_BASE, EPWM_SOC_A, 1);
 //EPWM_enableInterrupt(myEPWM4_BASE);
 //EPWM_setInterruptSource(myEPWM4_BASE, EPWM_INT_TBCTR_ZERO);
 //EPWM_setInterruptEventCount(myEPWM4_BASE, 3);
 // ADC Initialisation
 ADC_setVREF(myADC0_BASE, ADC_REFERENCE_INTERNAL, ADC_REFERENCE_3_3V);
 //
 // Configures the analog-to-digital converter module prescaler.
 //
 ADC_setPrescaler(myADC0_BASE, ADC_CLK_DIV_2_0);
 //
 // Sets the timing of the end-of-conversion pulse
 //
 ADC_setInterruptPulseMode(myADC0_BASE, ADC_PULSE_END_OF_CONV);
 //
 // Powers up the analog-to-digital converter core.
 //
 ADC_enableConverter(myADC0_BASE);
 //
 // Delay for 1ms to allow ADC time to power up
 //
 DEVICE_DELAY_US(5000);
 //
 // SOC Configuration: Setup ADC EPWM channel and trigger settings
 //
 // Disables SOC burst mode.
 //
 ADC_disableBurstMode(myADC0_BASE);
 //
 // Sets the priority mode of the SOCs.
 //
 ADC_setSOCPriority(myADC0_BASE, ADC_PRI_ALL_ROUND_ROBIN);
 //
 // Start of Conversion 1 Configuration
 //
 //
 //ADC_setupSOC(myADC0_BASE, ADC_SOC_NUMBER1,
ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN3, 15U);
 ADC_setupSOC(myADC0_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM4_SOCA, ADC_CH_ADCIN0, 8U); // INPUT VOLTAGE ADC_setupSOC(myADC0_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_EPWM4_SOCA, ADC_CH_ADCIN4, 8U); // INDUCTOR CURRENT ADC_setupSOC(myADC0_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_EPWM4_SOCA, ADC_CH_ADCIN6, 8U); // OUTPUT VOLTAGE ADC_setInterruptSOCTrigger(myADC0_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_NONE);
 ADC_setInterruptSOCTrigger(myADC0_BASE, ADC_SOC_NUMBER1, ADC_INT_SOC_TRIGGER_NONE);
 ADC_setInterruptSOCTrigger(myADC0_BASE, ADC_SOC_NUMBER2, ADC_INT_SOC_TRIGGER_NONE);
 //
 // ADC Interrupt 1 Configuration
 // Source : ADC_SOC_NUMBER1
 // Interrupt Source: enabled
 // Continuous Mode : disabled
 //
 //
 ADC_setInterruptSource(myADC0_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER2);
 ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);
ADC_disableContinuousMode(myADC0_BASE, ADC_INT_NUMBER1);
ADC_enableInterrupt(myADC0_BASE, ADC_INT_NUMBER1);
ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);
 EDIS;
 //
 // Enable sync and clock to PWM
 //
 SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
 //
 // Enable ePWM interrupts
 //
 Interrupt_enable(INT_ADCA1);
 //
 // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
 //
 EINT;
 ERTM;
 //
 // IDLE loop. Just sit and loop forever (optional):
 //
 for(;;)
 {
 NOP;
 }
}
//
// epwm4ISR - ePWM 4 ISR
//
float dnew, enew;
float dold = 0, eold = 0;
float dmax = 0.9f, dmin = 0.05f;
float V0ref = 50.0f, ILref = 0; // SET REFERENCE VALUES HERE
float V0meas, ILmeas, VINmeas;
float V0ana, ILana, VINana;
float T = 0.000025f;
float KP = 0.017573f;//0.05f; // SET KP VALUE HERE
float KI = 13.1982f; //5.0f; // SET KI VALUE HERE
__interrupt void epwm4ISR(void)
{
 V0meas = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER2); //A6 ILmeas = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1); //A4 VINmeas = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0); //A0 VINana = (float)((VINmeas - 430.0f)/74.909f);
 ILana = (float)((ILmeas - 2015.0f)/(-380.01f));
 V0ana = (float)((V0meas - 432.0f)/35.36f);
 enew = V0ref - V0ana;
 dnew = dold + KP*(enew - eold) + (KI*T)*(enew); // Backward
 if(dnew>dmax)
 dnew = dmax;
 if(dnew<dmin)
 dnew = dmin;
 dold = dnew;
 eold = enew;
 cmpVal = (uint16_t)(dnew * 1250); // SET TBPRD HERE
 EPWM_setCounterCompareValue(myEPWM4_BASE,EPWM_COUNTER_COMPARE_A,cmpVal);
 ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);
 //
 // Clear INT flag for this timer
 //
 //EPWM_clearEventTriggerInterruptFlag(myEPWM4_BASE);
 //
 // Acknowledge interrupt group
 //
 Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
