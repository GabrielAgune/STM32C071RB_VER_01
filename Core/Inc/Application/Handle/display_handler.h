/*******************************************************************************
 * @file        display_handler.h
 * @brief       Interface do Handler de Display.
 * @version     2.0 (Refatorado)
 * @author      Gemini
 * @details     Gerencia as atualiza��es peri�dicas de dados no display DWIN e
 * as sequ�ncias de telas, como o processo de medi��o.
 ******************************************************************************/

#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H

#include "dwin_driver.h"
#include "controller.h"
#include "gerenciador_configuracoes.h"
#include "medicao_handler.h"
#include "rtc_driver.h"
#include "relato.h"
#include "temp_sensor.h"
#include "main.h" // Para HAL_GetTick
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Inicializa o handler de display.
 */
void DisplayHandler_Init(void);

/**
 * @brief Processa as l�gicas de atualiza��o de display que devem rodar no super-loop.
 * Isso inclui a m�quina de estados da medi��o e atualiza��es peri�dicas de VPs.
 */
void DisplayHandler_Process(void);

// --- Handlers de Eventos chamados pelo Controller ---
void Display_ProcessPrintEvent(uint16_t received_value);
void Display_SetRepeticoes(uint16_t received_value);
void Display_SetDecimals(uint16_t received_value);
void Display_SetUser(const uint8_t* dwin_data, uint16_t len, uint16_t received_value);
void Display_SetCompany(const uint8_t* dwin_data, uint16_t len, uint16_t received_value);
void Display_Adj_Capa(uint16_t received_value);
void Display_ShowAbout(void);
void Display_ShowModel(void);
void Display_Preset(uint16_t received_value);
void Display_Set_Serial(const uint8_t* dwin_data, uint16_t len, uint16_t received_value);

/**
 * @brief Inicia a sequ�ncia de telas para o processo de medi��o.
 * Esta fun��o � N�O-BLOQUEANTE e apenas inicia a m�quina de estados.
 */
void Display_StartMeasurementSequence(void);

// --- Getters/Setters para estado interno ---
void Display_SetPrintingEnabled(bool is_enabled);
bool Display_IsPrintingEnabled(void);

#endif // DISPLAY_HANDLER_H