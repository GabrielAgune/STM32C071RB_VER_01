/*******************************************************************************
 * @file        pwm_servo_driver.c
 * @brief       Driver de baixo n�vel para controle de servomotores com PWM.
 * @details     Este m�dulo abstrai o controle de um canal de timer em modo PWM
 * para controlar a posi��o de um servomotor. Ele converte um �ngulo
 * em graus para o valor de pulso correspondente no registrador do timer.
 ******************************************************************************/

#include "pwm_servo_driver.h"

//==============================================================================
// Fun��es Privadas (Helpers)
//==============================================================================

// Mapeia um �ngulo para o valor do registrador de compara��o (CCR).
static uint32_t map_angle_to_ccr(Servo_t *servo, float angle)
{
    if (servo == NULL)
    {
        return 0; // Retorno seguro
    }

    // Garante que o �ngulo esteja dentro do limite de 0-180 graus para seguran�a.
    if (angle < 0.0f)
    {
        angle = 0.0f;
    }
    if (angle > 180.0f)
    {
        angle = 180.0f;
    }

    // Interpola��o Linear: converte o �ngulo na faixa [0, 180] para um pulso na faixa [min_pulse_us, max_pulse_us].
    uint16_t pulse_range_us = servo->max_pulse_us - servo->min_pulse_us;
    uint16_t target_pulse_us = servo->min_pulse_us + (uint16_t)((angle / 180.0f) * pulse_range_us);

    return target_pulse_us;
}

//==============================================================================
// Fun��es P�blicas (API do Driver)
//==============================================================================

// Inicializa e inicia a gera��o de PWM para um servo espec�fico.
HAL_StatusTypeDef PWM_Servo_Init(Servo_t *servo)
{
    if (servo == NULL || servo->htim == NULL)
    {
        return HAL_ERROR;
    }

    // Inicia o sinal PWM no canal do timer especificado na estrutura do servo.
    return HAL_TIM_PWM_Start(servo->htim, servo->channel);
}

// Define a posi��o do servo em um �ngulo espec�fico.
void PWM_Servo_SetAngle(Servo_t *servo, float angle)
{
    if (servo == NULL || servo->htim == NULL)
    {
        return;
    }

    // Converte o �ngulo desejado para o valor bruto do registrador CCR.
    uint32_t ccr_value = map_angle_to_ccr(servo, angle);

    // Define o valor de compara��o do timer, o que altera a largura do pulso
    // e, consequentemente, move o servo para a posi��o desejada.
    __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, ccr_value);
}

// Para a gera��o de PWM para um servo espec�fico.
HAL_StatusTypeDef PWM_Servo_DeInit(Servo_t *servo)
{
    if (servo == NULL || servo->htim == NULL)
    {
        return HAL_ERROR;
    }

    // Para o sinal PWM no canal do timer especificado.
    return HAL_TIM_PWM_Stop(servo->htim, servo->channel);
}