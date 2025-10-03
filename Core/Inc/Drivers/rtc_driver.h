#ifndef RTC_DRIVER_H
#define RTC_DRIVER_H

#include "rtc.h" // Inclui o handle do HAL
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Inicializa o driver do RTC.
 */
void RTC_Driver_Init(RTC_HandleTypeDef* hrtc);

/**
 * @brief Tarefa de processo peri�dico do RTC (chamada no super-loop).
 */
void RTC_Driver_Process(void);

/**
 * @brief Define a data do RTC.
 * @return true em sucesso, false em falha.
 */
bool RTC_Driver_SetDate(uint8_t day, uint8_t month, uint8_t year); // Ano com 2 d�gitos

/**
 * @brief Define a hora do RTC.
 * @return true em sucesso, false em falha.
 */
bool RTC_Driver_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);

/**
 * @brief Obt�m a data atual do RTC.
 * @param day Ponteiro para armazenar o dia.
 * @param month Ponteiro para armazenar o m�s.
 * @param year Ponteiro para armazenar o ano (2 d�gitos).
 * @return true em sucesso, false em falha.
 */
bool RTC_Driver_GetDate(uint8_t* day, uint8_t* month, uint8_t* year);

/**
 * @brief Obt�m a hora atual do RTC.
 * @param hours Ponteiro para armazenar as horas.
 * @param minutes Ponteiro para armazenar os minutos.
 * @param seconds Ponteiro para armazenar os segundos.
 * @return true em sucesso, false em falha.
 */
bool RTC_Driver_GetTime(uint8_t* hours, uint8_t* minutes, uint8_t* seconds);

#endif // RTC_DRIVER_H