#ifndef SERVO_CONTROLE_H
#define SERVO_CONTROLE_H

#include "main.h"

/**
 * @brief Inicializa o m�dulo de controle dos servos.
 */
void Servos_Init(void);

/**
 * @brief Processa a m�quina de estados e atualiza a posi��o dos servos.
 * Deve ser chamada repetidamente no loop principal.
 */
void Servos_Process(void);

/**
 * @brief Inicia a sequ�ncia de movimento dos servos.
 */
void Servos_Start_Sequence(void);

/**
 * @brief Decrementa os temporizadores internos de controle dos servos.
 * Esta fun��o DEVE ser chamada a cada 1ms por uma interrup��o de timer (ex: SysTick).
 */
void Servos_Tick_ms(void);

#endif // SERVO_CONTROLE_H
