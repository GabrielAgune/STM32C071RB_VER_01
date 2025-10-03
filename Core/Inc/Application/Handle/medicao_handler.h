#ifndef MEDICAO_HANDLER_H
#define MEDICAO_HANDLER_H

#include <stdint.h>
#include <stdbool.h>

// Estrutura para armazenar os resultados de uma �nica medi��o
typedef struct {
    float Frequencia;
    float Escala_A;
    float Peso;
    float Temp_Sample;
    float Temp_Instru;
    float Densidade;
    float Umidade;
} DadosMedicao_t;


// --- FUN��ES "SET" ---
// Usadas durante o processo de medi��o para atualizar os valores.

void Medicao_Init(void); // Zera os dados para uma nova medi��o
void Medicao_Set_Frequencia(float valor);
void Medicao_Set_Escala_A(float valor);
void Medicao_Set_Peso(float valor);
void Medicao_Set_Temp_Sample(float valor);
void Medicao_Set_Temp_Instru(float valor);
void Medicao_Set_Densidade(float valor);
void Medicao_Set_Umidade(float valor);


// --- FUN��O "GET" ---
// Usada por outros m�dulos (como o relatorio_handler) para ler o resultado final.

/**
 * @brief Obt�m uma c�pia completa dos dados da �ltima medi��o.
 * @param dados_out Ponteiro para uma struct que ser� preenchida com os dados.
 */
void Medicao_Get_UltimaMedicao(DadosMedicao_t* dados_out);


#endif // MEDICAO_HANDLER_H