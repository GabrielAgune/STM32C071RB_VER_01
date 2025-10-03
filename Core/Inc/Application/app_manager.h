// Core/Inc/Application/app_manager.h
// VERSÃO 8.2 (Refatorado por Dev STM)
// REMOVIDO: dependencia de scale_filter.h.
// ADICIONADO: Nova struct App_ScaleData_t.

#ifndef APP_MANAGER_H
#define APP_MANAGER_H

// Includes dos Periféricos Gerados pelo CubeMX
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "crc.h"
#include "rtc.h"
#include "tim.h"

// Includes dos Nossos Módulos e Drivers
#include "dwin_driver.h"
#include "eeprom_driver.h"
#include "ads1232_driver.h"
#include "pwm_servo_driver.h"
#include "pcb_frequency.h"
#include "temp_sensor.h"
#include "gerenciador_configuracoes.h"
#include "servo_controle.h"
#include "controller.h"
#include "cli_driver.h"

typedef enum {
  STATE_ACTIVE,
  STATE_STOPPED,
  STATE_CONFIRM_WAKEUP
} SystemState_t;

/**
 * @brief Inicializa todos os módulos da aplicação em uma sequência controlada.
 */
void App_Manager_Init(void);

/**
 * @brief Executa o loop de processamento principal da aplicação (Super-loop V8.2).
 */
void App_Manager_Process(void);

// Funções de Callback para serem chamadas pela UI/Controller
void App_Manager_Handle_Start_Process(void);
void App_Manager_Handle_New_Password(const char* new_password);
bool App_Manager_Run_Self_Diagnostics(uint8_t return_tela);

/**
 * @brief Solicita que o sistema entre no modo de baixo consumo (Stop).
 * Chamado pelo controller quando o botão de desligar é pressionado.
 */
void App_Manager_Request_Sleep(void);

/**
 * @brief Confirma que o usuário deseja acordar o sistema.
 * Chamado pelo controller quando o botão de confirmação é pressionado.
 */
void App_Manager_Confirm_Wakeup(void);

#endif // APP_MANAGER_H