#include "medicao_handler.h"
#include <string.h> // Para memset e memcpy

// --- ARMAZENAMENTO DOS DADOS (APENAS EM RAM) ---
// Esta vari�vel est�tica guarda os dados da medi��o atual.
static DadosMedicao_t s_medida_atual;


/**
 * @brief Zera a struct de medi��o, preparando para uma nova leitura.
 */
void Medicao_Init(void)
{
    memset(&s_medida_atual, 0, sizeof(DadosMedicao_t));
}

// --- Implementa��o dos "Setters" ---

void Medicao_Set_Frequencia(float valor) { s_medida_atual.Frequencia = valor; }
void Medicao_Set_Escala_A(float valor)   { s_medida_atual.Escala_A = valor; }
void Medicao_Set_Peso(float valor)       { s_medida_atual.Peso = valor; }
void Medicao_Set_Temp_Sample(float valor){ s_medida_atual.Temp_Sample = valor; }
void Medicao_Set_Temp_Instru(float valor){ s_medida_atual.Temp_Instru = valor; }
void Medicao_Set_Densidade(float valor)  { s_medida_atual.Densidade = valor; }
void Medicao_Set_Umidade(float valor)    { s_medida_atual.Umidade = valor; }


// --- Implementa��o do "Getter" ---

/**
 * @brief Copia os dados da medi��o atual para a struct fornecida.
 */
void Medicao_Get_UltimaMedicao(DadosMedicao_t* dados_out)
{
    if (dados_out != NULL)
    {
        // Copia a struct interna para o ponteiro fornecido pelo chamador.
        // Isso garante que o chamador tenha sua pr�pria c�pia e n�o modifique a original.
        memcpy(dados_out, &s_medida_atual, sizeof(DadosMedicao_t));
    }
}