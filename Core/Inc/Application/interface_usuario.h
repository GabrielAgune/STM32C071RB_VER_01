// Core/Inc/Application/interface_usuario.h

#ifndef INTERFACE_USUARIO_H
#define INTERFACE_USUARIO_H

#include "main.h"
#include "app_eventos.h" // A UI ainda precisa saber o que � um ServoStep_t

/**
 * @brief Inicializa o m�dulo da interface do usu�rio de forma n�o-bloqueante.
 */
void UI_Init(void);

/**
 * @brief Processa a m�quina de estados da UI, incluindo a sequ�ncia de splash screen.
 * Deve ser chamada repetidamente no loop principal.
 */
void UI_Process(void);

/**
 * @brief Atualiza os dados peri�dicos na tela (RTC e Temperatura).
 */
void UI_Update_Periodic(void);

// --- Fun��es de Comando (Controladas pelo App_Manager) ---

/**
 * @brief A��o a ser executada na UI ap�s uma autentica��o bem-sucedida.
 */
void UI_On_Auth_Success(void);

/**
 * @brief A��o a ser executada na UI ap�s uma falha de autentica��o.
 */
void UI_On_Auth_Failure(void);

/**
 * @brief Atualiza a tela da UI com base no passo atual do processo.
 * @param step O passo atual da sequ�ncia de servos.
 */
void UI_On_Process_Step_Changed(ServoStep_t step);

/**
 * @brief A��o a ser executada na UI quando o processo principal � finalizado.
 */
void UI_On_Process_Finished(void);

#endif // INTERFACE_USUARIO_H