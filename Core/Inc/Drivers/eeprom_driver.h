/*******************************************************************************
 * @file        eeprom_driver.h
 * @brief       API do Driver N�O-BLOQUEANTE para EEPROM I2C (V8.2)
 ******************************************************************************/

#ifndef EEPROM_DRIVER_H
#define EEPROM_DRIVER_H

#include "stm32c0xx_hal.h" // Importa os types do HAL (I2C_HandleTypeDef)
#include <stdbool.h>
#include <stdint.h>

// --- DEFINES DAS CONSTANTES FALTANTES ---
// (Estas constantes s�o baseadas nas suas respostas)
#define EEPROM_PAGE_SIZE        32     // 32 Bytes para AT24C64 (serve para AT24C512 tamb�m)
#define EEPROM_WRITE_TIME_MS    5      // 5ms de tempo de escrita de p�gina
#define EEPROM_I2C_TIMEOUT      100    // Timeout de boot (para fun��es bloqueantes)

//==============================================================================
// API P�blica (V8.2 - Ass�ncrona)
//==============================================================================

/**
 * @brief Inicializa o driver (guarda o handle I2C).
 */
void EEPROM_Driver_Init(I2C_HandleTypeDef *hi2c);

/**
 * @brief Verifica (bloqueante) se o dispositivo responde no barramento I2C.
 * Seguro para usar no BOOT.
 */
bool EEPROM_Driver_IsReady(void);

// --- API de Leitura Bloqueante (Apenas para Boot) ---
/**
 * @brief L� dados da EEPROM (Modo BLOQUEANTE).
 * ATEN��O: Use APENAS durante a inicializa��o (App_Manager_Init).
 */
bool EEPROM_Driver_Read_Blocking(uint16_t addr, uint8_t *data, uint16_t size);

bool EEPROM_Driver_Write_Blocking(uint16_t addr, const uint8_t *data, uint16_t size);

// --- API de Escrita Ass�ncrona (FSM) ---

/**
 * @brief Verifica se a FSM de escrita ass�ncrona est� ocupada.
 * @return true se uma escrita DMA ou um delay de p�gina de 5ms estiver ativo.
 */
bool EEPROM_Driver_IsBusy(void);

/**
 * @brief Inicia uma sequ�ncia de escrita ass�ncrona (DMA + FSM).
 * Esta fun��o retorna imediatamente. A escrita acontece em segundo plano.
 * Chame EEPROM_Driver_Write_Async_Poll() no superloop para process�-la.
 */
bool EEPROM_Driver_Write_Async_Start(uint16_t addr, const uint8_t *data, uint16_t size);

/**
 * @brief (FSM Poll) Processa a fila de escrita ass�ncrona.
 * Gerencia os callbacks de DMA e os delays de 5ms da p�gina (n�o-bloqueante).
 * @return true se a sequ�ncia completa de escrita terminou, false se ainda est� ocupada.
 */
bool EEPROM_Driver_Write_Async_Poll(void);


// --- Callbacks de ISR (Chamados por stm32c0xx_it.c) ---
// (O driver I2C do C071 usa DMA, ent�o precisamos dos callbacks de DMA)
void EEPROM_Driver_HandleTxCplt(I2C_HandleTypeDef *hi2c);
void EEPROM_Driver_HandleError(I2C_HandleTypeDef *hi2c);

/**
 * @brief Verifica se ocorreu um erro de I2C/DMA na �ltima opera��o ass�ncrona.
 * Limpa o flag de erro se ele for lido.
 * @return true se um erro ocorreu, false caso contr�rio.
 */
bool EEPROM_Driver_GetAndClearErrorFlag(void);


#endif // EEPROM_DRIVER_H