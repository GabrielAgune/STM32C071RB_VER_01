/*******************************************************************************
 * @file        medicao_handler.h
 * @brief       Interface do Handler de Medi��es.
 * @version     2.0 
 * @author      Gabriel Agune
 * @details     Centraliza toda a l�gica de aquisi��o e c�lculo de dados de
 * medi��o (peso, frequ�ncia, temperatura, etc.).
 ******************************************************************************/

#ifndef MEDICAO_HANDLER_H
#define MEDICAO_HANDLER_H

#include <stdint.h>
#include <stdbool.h>

// Estrutura de dados que armazena a �ltima medi��o completa.
typedef struct {
    float Peso;
    float Frequencia;
    float Escala_A;
    float Temp_Instru;
    float Densidade;
    float Umidade;
} DadosMedicao_t;

/**
 * @brief Inicializa o handler de medi��o.
 */
void Medicao_Init(void);

/**
 * @brief Processa as l�gicas de medi��o que devem rodar no super-loop.
 * Isso inclui a leitura da balan�a e a atualiza��o peri�dica da frequ�ncia.
 */
void Medicao_Process(void);

/**
 * @brief Obt�m uma c�pia da �ltima medi��o consolidada.
 * @param[out] dados Ponteiro para a estrutura onde os dados ser�o copiados.
 */
void Medicao_Get_UltimaMedicao(DadosMedicao_t* dados);

// --- Fun��es de atualiza��o para valores definidos externamente ---

/**
 * @brief Atualiza a temperatura do instrumento lida pelo sensor do MCU.
 */
void Medicao_Set_Temp_Instru(float temp_instru);

/**
 * @brief Define a densidade do gr�o atual (usado para c�lculos futuros).
 */
void Medicao_Set_Densidade(float densidade);

/**
 * @brief Define a umidade do gr�o atual (usado para c�lculos futuros).
 */
void Medicao_Set_Umidade(float umidade);


#endif // MEDICAO_HANDLER_H