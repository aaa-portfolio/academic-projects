#ifndef THD_CONFIG_H
#define THD_CONFIG_H

#include <stdint.h>

/* -------- Sampling configuration -------- */
#define SAMPLE_FREQUENCY    6400
#define GRID_FREQUENCY      48
#define SAMPLES_PER_CYCLE   128
#define TOTAL_SAMPLES       256
#define NUM_HARMONICS       15

/* -------- Math constants -------- */
#define PI                  3.14159265f
#define SQRT2               1.41421356f

/* -------- Sensor calibration -------- */
#define VOLTAGE_ZERO_ADC    0.0f
#define ACS712_ZERO_ADC     350.0f
#define ADC_MAX             4095.0f
#define ADC_VREF            3.3f
#define VOLTAGE_SENS        1.0f      /* ZMPT101B measured gain */
#define CURRENT_SENS        0.24f         /* ACS712 measured V/A    */

/* Pre-combined scale factors (raw ADC counts -> volts / amps) */
#define VOLTAGE_SCALE_FACTOR  ((ADC_VREF / ADC_MAX) / VOLTAGE_SENS)
#define CURRENT_SCALE_FACTOR  ((ADC_VREF / ADC_MAX) / CURRENT_SENS)

/* -------- Complex type -------- */
typedef struct {
    float real;
    float imag;
} Complex_t;

/* -------- Per-harmonic info -------- */
typedef struct {
    float magnitude;    /* RMS value              */
    float phase;        /* degrees                */
    float percentage;   /* percent of fundamental */
} Harmonic_t;

/* -------- Aggregate result -------- */
typedef struct {
    float       voltage_rms;
    float       current_rms;
    float       voltage_fundamental;
    float       current_fundamental;
    float       voltage_thd;        /* %   */
    float       current_thd;        /* %   */
    float       power_factor;       /* true PF */
    float       active_power;       /* W   */
    float       reactive_power;     /* VAR */
    uint32_t    timestamp;
    uint16_t     valid;
    Harmonic_t  voltage_harmonics[NUM_HARMONICS + 1];
    Harmonic_t  current_harmonics[NUM_HARMONICS + 1];
} THD_Result_t;

/* -------- Globals (defined in main.c / dft_thd.c) -------- */
extern uint16_t      voltage_raw_buffer[TOTAL_SAMPLES];
extern uint16_t      current_raw_buffer[TOTAL_SAMPLES];
extern float         voltage_samples[TOTAL_SAMPLES];
extern float         current_samples[TOTAL_SAMPLES];
extern THD_Result_t  thd_result;

void  DFT_Init_Tables(void);
void  Convert_Raw_To_Physical(void);
void  Calculate_GDFT(float *V, float *I,
                     Complex_t *V_DFT, Complex_t *I_DFT);
float Calculate_Power_Factor(Complex_t v_fundamental,
                             Complex_t i_fundamental);
void  Process_Samples(void);


#endif /* THD_CONFIG_H */
