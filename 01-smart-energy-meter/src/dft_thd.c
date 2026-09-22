
#include "thd_config.h"
#include <math.h>
#include <string.h>
#include <stdint.h>

float voltage_samples[TOTAL_SAMPLES];
float current_samples[TOTAL_SAMPLES];

static Complex_t voltage_spectrum[NUM_HARMONICS + 1];
static Complex_t current_spectrum[NUM_HARMONICS + 1];

static float   cos_table[NUM_HARMONICS + 1];
static float   sin_table[NUM_HARMONICS + 1];
static uint16_t tables_initialized = 0;

static const float INV_N_SAMPLES = 1.0f / (float)SAMPLES_PER_CYCLE;
static const float INV_N_TOTAL   = 1.0f / (float)TOTAL_SAMPLES;

static float Complex_Magnitude(Complex_t c)
{
    return sqrtf(c.real * c.real + c.imag * c.imag);
}
static float Complex_Phase_Degrees(Complex_t c)
{
    return atan2f(c.imag, c.real) * 180.0f / PI;
}
static float Complex_MagnitudeSq(Complex_t c)
{
    return (c.real * c.real + c.imag * c.imag);
}

void DFT_Init_Tables(void)
{
    uint32_t k;
    float    omega;
    if (tables_initialized) return;
    for (k = 0; k < NUM_HARMONICS + 1; k++)
    {
        omega        = 2.0f * PI * (float)k * INV_N_SAMPLES;
        cos_table[k] = cosf(omega);
        sin_table[k] = sinf(omega);
    }
    tables_initialized = 1;
}

void Convert_Raw_To_Physical(void)
{
    uint32_t i;
    float    v_raw, i_raw;
    for (i = 0; i < TOTAL_SAMPLES; i++)
    {
        v_raw = (float)voltage_raw_buffer[i] - VOLTAGE_ZERO_ADC;
        voltage_samples[i] = v_raw * VOLTAGE_SCALE_FACTOR;
        i_raw = (float)current_raw_buffer[i] - ACS712_ZERO_ADC;
        current_samples[i] = i_raw * CURRENT_SCALE_FACTOR;
    }
}

void Calculate_GDFT(float *V, float *I, Complex_t *V_DFT, Complex_t *I_DFT)
{
    int   h, s;
    float sq_V_sum = 0.0f, sq_I_sum = 0.0f;
    float v_fund = 0.0f, i_fund = 0.0f;
    float v_fund_inv = 0.0f, i_fund_inv = 0.0f;
    float cCF;
    float sV0, sV1, sV2, sI0, sI1, sI2;
    float v_mag, i_mag;
    float abs_sq_V1, abs_sq_I1, sq_V_sum_H2, sq_I_sum_H2;
    float v_dc_mag, i_dc_mag;

    for (h = 0; h < (NUM_HARMONICS + 1); h++)
    {
        cCF = 2.0f * cos_table[h];
        sV0 = 0.0f; sV1 = 0.0f; sV2 = 0.0f;
        sI0 = 0.0f; sI1 = 0.0f; sI2 = 0.0f;

        for (s = 0; s < TOTAL_SAMPLES; s++)
        {
            if (h == 0) {
                sV0 += V[s];
                sI0 += I[s];
            } else {
                sV0 = V[s] + cCF * sV1 - sV2;
                sV2 = sV1; sV1 = sV0;
                sI0 = I[s] + cCF * sI1 - sI2;
                sI2 = sI1; sI1 = sI0;
            }
        }

        if (h == 0) {
            V_DFT[h].real = sV0 * INV_N_TOTAL; V_DFT[h].imag = 0.0f;
            I_DFT[h].real = sI0 * INV_N_TOTAL; I_DFT[h].imag = 0.0f;
        } else {
            V_DFT[h].real = (sV1 - sV2 * cos_table[h]) * SQRT2 * INV_N_TOTAL;
            V_DFT[h].imag = (sV2 * sin_table[h])       * SQRT2 * INV_N_TOTAL;
            I_DFT[h].real = (sI1 - sI2 * cos_table[h]) * SQRT2 * INV_N_TOTAL;
            I_DFT[h].imag = (sI2 * sin_table[h])       * SQRT2 * INV_N_TOTAL;
        }

        sq_V_sum += Complex_MagnitudeSq(V_DFT[h]);
        sq_I_sum += Complex_MagnitudeSq(I_DFT[h]);

        if (h == 0) continue;

        if (h == 1) {
            v_fund     = Complex_Magnitude(V_DFT[1]);
            i_fund     = Complex_Magnitude(I_DFT[1]);
            v_fund_inv = (v_fund > 0.001f) ? 1.0f / v_fund : 0.0f;
            i_fund_inv = (i_fund > 0.001f) ? 1.0f / i_fund : 0.0f;
        }

        v_mag = Complex_Magnitude(V_DFT[h]);
        thd_result.voltage_harmonics[h].magnitude  = v_mag;
        thd_result.voltage_harmonics[h].phase      = Complex_Phase_Degrees(V_DFT[h]);
        thd_result.voltage_harmonics[h].percentage = (v_fund > 0.001f) ? (v_mag * v_fund_inv * 100.0f) : 0.0f;

        i_mag = Complex_Magnitude(I_DFT[h]);
        thd_result.current_harmonics[h].magnitude  = i_mag;
        thd_result.current_harmonics[h].phase      = Complex_Phase_Degrees(I_DFT[h]);
        thd_result.current_harmonics[h].percentage = (i_fund > 0.001f) ? (i_mag * i_fund_inv * 100.0f) : 0.0f;
    }

    thd_result.voltage_rms = sqrtf(sq_V_sum);
    thd_result.current_rms = sqrtf(sq_I_sum);
    thd_result.voltage_fundamental = Complex_Magnitude(V_DFT[1]);
    thd_result.current_fundamental = Complex_Magnitude(I_DFT[1]);

    abs_sq_V1   = Complex_MagnitudeSq(V_DFT[1]);
    abs_sq_I1   = Complex_MagnitudeSq(I_DFT[1]);
    sq_V_sum_H2 = sq_V_sum - Complex_MagnitudeSq(V_DFT[0]) - abs_sq_V1;
    sq_I_sum_H2 = sq_I_sum - Complex_MagnitudeSq(I_DFT[0]) - abs_sq_I1;

    thd_result.voltage_thd = sqrtf(sq_V_sum_H2 > 0.0f ? sq_V_sum_H2 / abs_sq_V1 : 0.0f) * 100.0f;
    thd_result.current_thd = sqrtf(sq_I_sum_H2 > 0.0f ? sq_I_sum_H2 / abs_sq_I1 : 0.0f) * 100.0f;

    v_dc_mag = Complex_Magnitude(V_DFT[0]);
    thd_result.voltage_harmonics[0].magnitude  = v_dc_mag;
    thd_result.voltage_harmonics[0].phase      = Complex_Phase_Degrees(V_DFT[0]);
    thd_result.voltage_harmonics[0].percentage = (v_fund > 0.001f) ? (v_dc_mag * v_fund_inv * 100.0f) : 0.0f;

    i_dc_mag = Complex_Magnitude(I_DFT[0]);
    thd_result.current_harmonics[0].magnitude  = i_dc_mag;
    thd_result.current_harmonics[0].phase      = Complex_Phase_Degrees(I_DFT[0]);
    thd_result.current_harmonics[0].percentage = (i_fund > 0.001f) ? (i_dc_mag * i_fund_inv * 100.0f) : 0.0f;
}

float Calculate_Power_Factor(Complex_t v_fundamental, Complex_t i_fundamental)
{
    float v_phase    = Complex_Phase_Degrees(v_fundamental);
    float i_phase    = Complex_Phase_Degrees(i_fundamental);
    float phase_diff = (v_phase - i_phase) * PI / 180.0f;
    return fabsf(cosf(phase_diff));
}

void Process_Samples(void)
{
    float v_fund_mag, i_fund_mag;
    float dpf, pf_sq;

    Convert_Raw_To_Physical();
    Calculate_GDFT(voltage_samples, current_samples, voltage_spectrum, current_spectrum);

    v_fund_mag = Complex_Magnitude(voltage_spectrum[1]);
    i_fund_mag = Complex_Magnitude(current_spectrum[1]);

    if (v_fund_mag > 0.001f && i_fund_mag > 0.001f) {
        dpf = Calculate_dPower_Factor(voltage_spectrum[1], current_spectrum[1]);
        float i1_over_irms = i_fund_mag / thd_result.current_rms;
        thd_result.power_factor = dpf * i1_over_irms;

        thd_result.active_power = thd_result.voltage_rms * thd_result.current_rms * thd_result.power_factor;
        pf_sq = thd_result.power_factor * thd_result.power_factor;
        thd_result.reactive_power = thd_result.voltage_rms * thd_result.current_rms * sqrtf(pf_sq < 1.0f ? 1.0f - pf_sq : 0.0f);
    }
    else {
        thd_result.power_factor   = 0.0f;
        thd_result.active_power   = 0.0f;
        thd_result.reactive_power = 0.0f;
    }
    thd_result.valid     = 1;
}
