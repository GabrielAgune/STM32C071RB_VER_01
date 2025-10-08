/*******************************************************************************
 * @file        eeprom_driver.c
 * @brief       Driver BLOQUEANTE robusto para EEPROM I2C (AT24C series).
 * @version     9.0 (Solu��o Definitiva)
 * @details     Implementa leitura e escrita em blocos (chunks) para garantir
 * a compatibilidade com grandes volumes de dados, evitando timeouts
 * do HAL e travamento do barramento I2C.
 ******************************************************************************/

#include "eeprom_driver.h"
#include <stdio.h>

//==============================================================================
// Defini��es
//==============================================================================
#define EEPROM_I2C_ADDR         (0x50 << 1)
#define EEPROM_I2C_TIMEOUT_MS   100

// Defina o tamanho m�ximo do bloco para uma �nica transa��o I2C. 128 � um valor seguro.
#define I2C_MAX_CHUNK_SIZE      128

static I2C_HandleTypeDef *s_i2c_handle = NULL;

//==============================================================================
// Fun��es P�blicas
//==============================================================================

void EEPROM_Driver_Init(I2C_HandleTypeDef *hi2c) {
    s_i2c_handle = hi2c;
}

bool EEPROM_Driver_IsReady(void) {
    if (s_i2c_handle == NULL) return false;
    return (HAL_I2C_IsDeviceReady(s_i2c_handle, EEPROM_I2C_ADDR, 2, EEPROM_I2C_TIMEOUT) == HAL_OK);
}

/**
 * @brief L� um grande volume de dados da EEPROM em blocos (chunks).
 */
bool EEPROM_Driver_Read_Blocking(uint16_t addr, uint8_t *data, uint16_t size) {
    if (s_i2c_handle == NULL || data == NULL || size == 0) return false;

    uint16_t bytes_remaining = size;
    uint16_t current_addr = addr;
    uint8_t* p_data = data;

    while (bytes_remaining > 0) {
        uint16_t chunk_size = (bytes_remaining > I2C_MAX_CHUNK_SIZE) ? I2C_MAX_CHUNK_SIZE : bytes_remaining;

        if (HAL_I2C_Mem_Read(s_i2c_handle, EEPROM_I2C_ADDR, current_addr, I2C_MEMADD_SIZE_16BIT, p_data, chunk_size, EEPROM_I2C_TIMEOUT_MS) != HAL_OK) {
            printf("EEPROM Read ERR: Falha em bloco no addr 0x%X\r\n", current_addr);
            return false;
        }

        current_addr += chunk_size;
        p_data += chunk_size;
        bytes_remaining -= chunk_size;
    }
    return true;
}

/**
 * @brief Escreve um grande volume de dados na EEPROM, respeitando a pagina��o.
 */
bool EEPROM_Driver_Write_Blocking(uint16_t addr, const uint8_t *data, uint16_t size) {
    if (s_i2c_handle == NULL || data == NULL || size == 0) return false;

    uint16_t bytes_remaining = size;
    uint16_t current_addr = addr;
    const uint8_t* p_data = data;

    while (bytes_remaining > 0) {
        // Calcula o quanto podemos escrever sem ultrapassar o limite da p�gina atual
        uint16_t chunk_size = EEPROM_PAGE_SIZE - (current_addr % EEPROM_PAGE_SIZE);
        if (chunk_size > bytes_remaining) {
            chunk_size = bytes_remaining;
        }

        if (HAL_I2C_Mem_Write(s_i2c_handle, EEPROM_I2C_ADDR, current_addr, I2C_MEMADD_SIZE_16BIT, (uint8_t*)p_data, chunk_size, EEPROM_I2C_TIMEOUT_MS) != HAL_OK) {
            printf("EEPROM Write ERR: Falha em bloco no addr 0x%X\r\n", current_addr);
            return false;
        }

        // Espera o tempo de escrita da p�gina (essencial!)
        HAL_Delay(EEPROM_WRITE_TIME_MS);

        current_addr += chunk_size;
        p_data += chunk_size;
        bytes_remaining -= chunk_size;
    }
    return true;
}

/******************************************************************************
 * O c�digo abaixo (FSM Ass�ncrona de Teste) n�o � mais necess�rio com
 * as fun��es bloqueantes robustas acima. Simplifique seu gerenciador para
 * chamar a fun��o de escrita bloqueante diretamente quando o flag 'dirty'
 * for detectado.
 ******************************************************************************/
bool EEPROM_Driver_IsBusy(void) { return false; } // N�o mais aplic�vel
bool EEPROM_Driver_Write_Async_Start(uint16_t addr, const uint8_t *data, uint16_t size) {
    // Apenas um wrapper para a nova fun��o bloqueante, para manter a FSM funcionando por enquanto.
    return EEPROM_Driver_Write_Blocking(addr, data, size);
}
bool EEPROM_Driver_Write_Async_Poll(void) { return true; } // Sempre retorna "terminado"
bool EEPROM_Driver_GetAndClearErrorFlag(void) { return false; } // N�o mais aplic�vel
void EEPROM_Driver_HandleTxCplt(I2C_HandleTypeDef *hi2c) {} // N�o mais aplic�vel
void EEPROM_Driver_HandleError(I2C_HandleTypeDef *hi2c) {} // N�o mais aplic�vel