#ifndef SCALE_FILTER_H
#define SCALE_FILTER_H

#include <stdint.h>

#ifndef SCALE_FILTER_WIN_SIZE
// por padr�o usa 64 amostras; voc� pode #define SCALE_FILTER_WIN_SIZE antes do include
#define SCALE_FILTER_WIN_SIZE 64
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float avg_counts;
    float avg_grams;
    float sigma_counts;
    float sigma_grams;
    float slope_counts;
    float slope_grams;
    uint8_t is_stable;      // 1 se sigma/slope em gramas estiverem abaixo dos limiares
    uint8_t step_detected;  // 1 se houve degrau > limiar (em g) entre m�dias consecutivas
} ScaleFilterOut;

typedef struct {
    // buffer e �ndices
    int32_t buffer[SCALE_FILTER_WIN_SIZE];
    int     idx;
    uint8_t filled;

    // limiares
    float stability_sigma_g;
    float stability_slope_g;
    float step_threshold_g;

    // mem�ria para detec��o de degrau
    float  prev_avg_g;
    uint8_t first_avg;
} ScaleFilter;

// inicializa preenchendo o buffer todo com "initial_counts" (tipicamente o offset/tara)
void ScaleFilter_Init(ScaleFilter* sf, int32_t initial_counts);

// recome�a o filtro ap�s uma tara/auto-zero (refaz o buffer com o novo offset)
void ScaleFilter_ResetWithOffset(ScaleFilter* sf, int32_t new_offset_counts);

// ajusta os limiares (em gramas)
void ScaleFilter_SetThresholds(ScaleFilter* sf,
                               float sigma_g_thr,
                               float slope_g_thr,
                               float step_thr_g);

// insere uma nova amostra (em counts) e retorna m�tricas em "out"
void ScaleFilter_Push(ScaleFilter* sf, int32_t new_counts, ScaleFilterOut* out);

#ifdef __cplusplus
}
#endif

#endif // SCALE_FILTER_H
