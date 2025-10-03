/*******************************************************************************
 * @file        retarget.c
 * @brief       Redirecionamento (Retarget) da fun��o printf para UART (V8.1 - DMA).
 * @version     2.1 (Refatorado por Dev STM)
 * @details     Este m�dulo redireciona o fputc (usado pelo printf) para o
 * driver CLI (CLI_Printf_Transmit), que gerencia um FIFO de TX ass�ncrono
 * que alimenta a Bomba (Pump) de DMA.
 ******************************************************************************/

#include "retarget.h"
#include <stdio.h>
#include "cli_driver.h" // Depend�ncia principal para TX ass�ncrono

//==============================================================================
// Vari�veis Est�ticas e Globais
//==============================================================================

static UART_HandleTypeDef* s_debug_huart = NULL;
static UART_HandleTypeDef* s_dwin_huart = NULL;

RetargetDestination_t g_retarget_dest = TARGET_DEBUG;

//==============================================================================
// Fun��es P�blicas
//==============================================================================

void Retarget_Init(UART_HandleTypeDef* debug_huart, UART_HandleTypeDef* dwin_huart)
{
    s_debug_huart = debug_huart;
    s_dwin_huart = dwin_huart;

    // Garante que a biblioteca C chame fputc para CADA caractere, imediatamente.
    setvbuf(stdout, NULL, _IONBF, 0);
}


//==============================================================================
// Reimplementa��o de Fun��es da Biblioteca C Padr�o (REFATORADO V8.1)
//==============================================================================

/**
 * @brief Reimplementa��o N�O-BLOQUEANTE do fputc.
 */
int fputc(int ch, FILE *f)
{
    uint8_t c = (uint8_t)ch;

    // printf() S� deve ir para o console de DEBUG (UART1).
    if (g_retarget_dest == TARGET_DEBUG)
    {
        if (s_debug_huart != NULL)
        {
            // Substitui o HAL_UART_Transmit(..., HAL_MAX_DELAY) por:
            // Chama a fun��o at�mica do driver CLI que enfileira o byte no FIFO
            // e retorna imediatamente.
            CLI_Printf_Transmit(c);
        }
    }
    // O caso TARGET_DWIN � removido (n�o � seguro enviar printf() cru para o DWIN).
    
    return ch;
}

int ferror(FILE *f)
{
    return 0;
}