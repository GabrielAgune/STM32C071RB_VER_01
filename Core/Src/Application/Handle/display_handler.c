#include "display_handler.h"

// Valor padr�o enviado pelo DWIN ao entrar em uma tela de edi��o
#define DWIN_VP_ENTRADA_TELA 0x0050 

//================================================================================
// Vari�veis Est�ticas (Estado do M�dulo)
//================================================================================

// A vari�vel de estado agora vive em seguran�a aqui.
static bool s_printing_enabled = true;

//================================================================================
// Fun��es P�blicas (Processadores de Evento)
//================================================================================

void Display_Handle_ON_OFF(int16_t received_value)
{
    // L�gica simples, pode permanecer aqui.
    if (received_value == 0x0010) {
        DWIN_Driver_WriteRawBytes(CMD_AJUSTAR_BACKLIGHT_10, sizeof(CMD_AJUSTAR_BACKLIGHT_10));
        DWIN_Driver_SetScreen(SYSTEM_STANDBY);
        printf("Display Handler: Desliga backlight\n\r");
    } else {
        DWIN_Driver_WriteRawBytes(CMD_AJUSTAR_BACKLIGHT_100, sizeof(CMD_AJUSTAR_BACKLIGHT_100));
        DWIN_Driver_SetScreen(PRINCIPAL);
        printf("Display Handler: Religa backlight\n\r");
    }
}

void Display_ProcessPrintEvent(uint16_t received_value)
{
    if (!s_printing_enabled) return;

    // L�gica de decis�o
    if (received_value == 0x0000) // Valor para "mostrar resultado na tela"
    {
        // A��o de UI: Atualizar a tela de resultados
        Config_Grao_t dados_grao;
        uint8_t indice_grao;
        Gerenciador_Config_Get_Grao_Ativo(&indice_grao);
        Gerenciador_Config_Get_Dados_Grao(indice_grao, &dados_grao);

        DadosMedicao_t dados_medicao;
        Medicao_Get_UltimaMedicao(&dados_medicao);
        
        uint16_t casas_decimais = Gerenciador_Config_Get_NR_Decimals();

        DWIN_Driver_WriteString(GRAO_A_MEDIR, dados_grao.nome, MAX_NOME_GRAO_LEN);
        DWIN_Driver_WriteInt(CURVA, dados_grao.id_curva);
        DWIN_Driver_WriteInt(UMI_MIN, (dados_grao.umidade_min) * 10);
        DWIN_Driver_WriteInt(UMI_MAX, (dados_grao.umidade_max) * 10);

        if (casas_decimais == 1) {
            DWIN_Driver_WriteInt(UMIDADE_1_CASA, (int16_t)(dados_medicao.Umidade * 10.0f));
            DWIN_Driver_SetScreen(MEDE_RESULT_01);
        } else { // Assume 2 ou mais
            DWIN_Driver_WriteInt(UMIDADE_2_CASAS, (int16_t)(dados_medicao.Umidade * 100.0f));
            DWIN_Driver_SetScreen(MEDE_RESULT_02);
        }
    }
    else // Valor para "imprimir relat�rio f�sico"
    {
        // A��o de UI: Chamar o m�dulo de impress�o
        Relatorio_Printer();
    }
}

void Telas_Mede(void)
{
	DWIN_Driver_SetScreen(MEDE_ENCHE_CAMARA);
	DWIN_TX_Pump();
	HAL_Delay(1000);
	DWIN_Driver_SetScreen(MEDE_AJUSTANDO);
	DWIN_TX_Pump();
	HAL_Delay(1000);
	DWIN_Driver_SetScreen(MEDE_RASPA_CAMARA);
	DWIN_TX_Pump();
	HAL_Delay(1000);
	DWIN_Driver_SetScreen(MEDE_PESO_AMOSTRA);
	DWIN_TX_Pump();
	HAL_Delay(1000);
	DWIN_Driver_SetScreen(MEDE_TEMP_SAMPLE);
	DWIN_TX_Pump();
	HAL_Delay(1000);
	DWIN_Driver_SetScreen(MEDE_UMIDADE);
	DWIN_TX_Pump();
	HAL_Delay(1000);
	Display_ProcessPrintEvent(0x0000);
}	
	

void Display_SetRepeticoes(uint16_t received_value)
{
    char buffer[40];
    if (received_value == DWIN_VP_ENTRADA_TELA) {
        // L�gica: Apenas buscar o valor atual
        uint16_t atual = Gerenciador_Config_Get_NR_Repetition();
        sprintf(buffer, "Atual NR_Repetition: %u", atual);
        
        // A��o de UI: Mostrar valor e ir para a tela de edi��o
        DWIN_Driver_WriteString(VP_MESSAGES, buffer, strlen(buffer)); 
        DWIN_Driver_SetScreen(TELA_SETUP_REPETICOES);
    } else {
        // L�gica: Salvar o novo valor
        Gerenciador_Config_Set_NR_Repetitions(received_value);
        sprintf(buffer, "Novo NR_Repetition: %u", received_value);

        // A��o de UI: Mostrar confirma��o
        DWIN_Driver_WriteString(VP_MESSAGES, buffer, strlen(buffer));
    }
}

void Display_SetDecimals(uint16_t received_value)
{
    char buffer[40];
    if (received_value == DWIN_VP_ENTRADA_TELA) {
        // L�gica
        uint16_t atual = Gerenciador_Config_Get_NR_Decimals();
        sprintf(buffer, "Atual NR_Decimals: %u", atual);
        
        // A��o de UI
        DWIN_Driver_WriteString(VP_MESSAGES, buffer, strlen(buffer)); 
        DWIN_Driver_SetScreen(TELA_SET_DECIMALS);
    } else {
        // L�gica
        Gerenciador_Config_Set_NR_Decimals(received_value);
        sprintf(buffer, "Novo NR_Decimals: %u", received_value);

        // A��o de UI
        DWIN_Driver_WriteString(VP_MESSAGES, buffer, strlen(buffer));
    }
}

void Display_SetUser(const uint8_t* dwin_data, uint16_t len, uint16_t received_value)
{
    char buffer_display[50];
    if (received_value == DWIN_VP_ENTRADA_TELA) {
        char nome_atual[21] = {0}; 
        Gerenciador_Config_Get_Usuario(nome_atual, sizeof(nome_atual));
        sprintf(buffer_display, "Atual Usuario: %s", nome_atual);

        DWIN_Driver_WriteString(VP_MESSAGES, buffer_display, strlen(buffer_display)); 
        DWIN_Driver_SetScreen(TELA_USER);
    } else {
        char novo_nome[21] = {0}; 
        const uint8_t* payload = &dwin_data[6];
        uint16_t payload_len = len - 6;

        if (DWIN_Parse_String_Payload_Robust(payload, payload_len, novo_nome, sizeof(novo_nome)) && strlen(novo_nome) > 0) {
            Gerenciador_Config_Set_Usuario(novo_nome);
            sprintf(buffer_display, "Novo Usuario: %s", novo_nome);
            DWIN_Driver_WriteString(VP_MESSAGES, buffer_display, strlen(buffer_display));
        }
    }
}

void Display_SetCompany(const uint8_t* dwin_data, uint16_t len, uint16_t received_value)
{
    char buffer_display[50];
    if (received_value == DWIN_VP_ENTRADA_TELA) {
        char empresa_atual[21] = {0};
        Gerenciador_Config_Get_Company(empresa_atual, sizeof(empresa_atual));
        sprintf(buffer_display, "Atual Empresa: %s", empresa_atual);

        DWIN_Driver_WriteString(VP_MESSAGES, buffer_display, strlen(buffer_display)); 
        DWIN_Driver_SetScreen(TELA_COMPANY);
    } else {
        char nova_empresa[21] = {0}; 
        const uint8_t* payload = &dwin_data[6];
        uint16_t payload_len = len - 6;
        
        if (DWIN_Parse_String_Payload_Robust(payload, payload_len, nova_empresa, sizeof(nova_empresa)) && strlen(nova_empresa) > 0) {
            Gerenciador_Config_Set_Company(nova_empresa);
            sprintf(buffer_display, "Nova Empresa: %s", nova_empresa);
            DWIN_Driver_WriteString(VP_MESSAGES, buffer_display, strlen(buffer_display));
        }
    }
}

void Display_Adj_Capa(uint16_t received_value)
{
	DWIN_Driver_WriteString(VP_MESSAGES, "AdjustFrequency: 3000.0KHz+/-2.0", strlen("AdjustFrequency: 3000.0KHz+/-2.0"));
	Controller_SetScreen(TELA_MONITOR_SYSTEM);
}


void Display_ShowAbout(void)
{
    DWIN_Driver_WriteString(VP_MESSAGES, "G620_Teste_Gab", strlen("G620_Teste_Gab"));
    DWIN_Driver_SetScreen(TELA_ABOUT_SYSTEM);
}

void Display_ShowModel(void)
{
    DWIN_Driver_WriteString(VP_MESSAGES, "G620_Teste_Gab", strlen("G620_Teste_Gab"));
    DWIN_Driver_SetScreen(TELA_MODEL_OEM);
}

void Display_Preset(uint16_t received_value)
{
	char buffer_display[50];
	if (received_value == 0x0000)
	{
		DWIN_Driver_WriteString(VP_MESSAGES, "Preset redefine os ajustes!", strlen("Preset redefine os ajustes!"));
		DWIN_Driver_SetScreen(TELA_PRESET_PRODUCT);
	}
	else
	{
		Carregar_Configuracao_Padrao();
		
		DWIN_Driver_WriteString(VP_MESSAGES, "Preset Completo!", strlen("Preset Completo!"));
	}
	
}

void Display_Set_Serial(const uint8_t* dwin_data, uint16_t len, uint16_t received_value)
{
	if (received_value == 0x0000)
	{
		Controller_SetScreen(TELA_SET_SERIAL);
		char serial_atual[17] = {0};
		char buffer_display[50] = {0};

		Gerenciador_Config_Get_Serial(serial_atual, sizeof(serial_atual));

		sprintf(buffer_display, "%s", serial_atual);
		DWIN_Driver_WriteString(VP_MESSAGES, buffer_display, strlen(buffer_display)); 
	}
	else
	{
		char novo_serial[17] = {0}; 
		const uint8_t* payload = &dwin_data[8];
		uint16_t payload_len = len - 8;
		if (DWIN_Parse_String_Payload_Robust(payload, payload_len, novo_serial, sizeof(novo_serial)))
		{
			if (strlen(novo_serial) > 0)
			{
				printf("Display Handler: Recebido novo serial: '%s'\n", novo_serial);
				Gerenciador_Config_Set_Serial(novo_serial);
				char buffer_display[50] = {0};
				sprintf(buffer_display, "%s", novo_serial);
				DWIN_Driver_WriteString(VP_MESSAGES, buffer_display, strlen(buffer_display));
			}
		}
	}
}

//================================================================================
// Implementa��o dos Getters/Setters de Estado
//================================================================================

void Display_SetPrintingEnabled(bool is_enabled)
{
    s_printing_enabled = is_enabled;
    printf("Display Handler: Impressao %s\r\n", s_printing_enabled ? "HABILITADA" : "DESABILITADA");
}

bool Display_IsPrintingEnabled(void)
{
    return s_printing_enabled;
}