#ifndef RETARGET_H
#define RETARGET_H

#include "main.h"
#include <stdio.h>

// Enumera��o para os poss�veis destinos de sa�da do printf
typedef enum {
    TARGET_DEBUG, // UART2 para o PC/CLI (padr�o)
    TARGET_DWIN   // UART1 para o Display DWIN
} RetargetDestination_t;

// Vari�vel global que controla para onde o printf envia os dados.
extern RetargetDestination_t g_retarget_dest;

/**
 * @brief Inicializa o m�dulo de retarget com os handles das UARTs.
 * @param debug_huart Ponteiro para o handle da UART de debug (PC).
 * @param dwin_huart Ponteiro para o handle da UART do display DWIN.
 */
void Retarget_Init(UART_HandleTypeDef* debug_huart, UART_HandleTypeDef* dwin_huart);

#endif // RETARGET_H
