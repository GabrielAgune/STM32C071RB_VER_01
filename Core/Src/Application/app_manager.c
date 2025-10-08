/*******************************************************************************
 * @file        app_manager.c
 * @brief       Gerenciador central da aplica��o (Arquitetura V8.7 - Refatorado)
 * @version     8.7
 * @author      Gabriel Agune
 * @details     Implementa o orquestrador principal da aplica��o. Gerencia
 * os estados de alto n�vel (Ativo, Parado) e delega a l�gica de
 * neg�cio para os handlers especializados. O c�digo agora segue
 * o Princ�pio da Responsabilidade �nica, com alta coes�o e baixo
 * acoplamento.
 ******************************************************************************/

#include "app_manager.h"


//================================================================================
// Vari�veis de Estado Globais do M�dulo
//================================================================================

static SystemState_t s_current_state = STATE_ACTIVE;
static volatile bool s_go_to_sleep_request = false;
static volatile bool s_wakeup_confirmed = false;

// --- Vari�veis para o modo de confirma��o de "acordar" ---
static uint32_t s_confirm_start_tick = 0;
static uint32_t s_countdown_last_tick = 0;

//================================================================================
// Prot�tipos de Fun��es Privadas
//================================================================================

static void Task_Handle_High_Frequency_Polling(void);
static void EnterStopMode(void);
static void HandleWakeUpSequence(void);
static bool Test_DisplayInfo(void);
static bool Test_Servos(void);
static bool Test_Capacimetro(void);
static bool Test_Balanca(void);
static bool Test_Termometro(void);
static bool Test_EEPROM(void);
static bool Test_RTC(void);

//================================================================================
// Implementa��o das Fun��es P�blicas
//================================================================================

static const DiagnosticStep_t s_diagnostic_steps[] = {
    {"Exibindo Logo e Versoes...",  LOGO,                3000, Test_DisplayInfo},
    {"Verificando Servos...",       BOOT_CHECK_SERVOS,   1200, Test_Servos},
    {"Verificando Medidor Freq...", BOOT_CHECK_CAPACI,   1200, Test_Capacimetro},
    {"Verificando Balanca...",      BOOT_BALANCE,        1000, Test_Balanca},
    {"Verificando Termometro...",   BOOT_THERMOMETER,    1000, Test_Termometro},
    {"Verificando Memoria EEPROM...", BOOT_MEMORY,       1100, Test_EEPROM},
    {"Verificando RTC...",          BOOT_CLOCK,          1100, Test_RTC},
};
static const size_t NUM_DIAGNOSTIC_STEPS = sizeof(s_diagnostic_steps) / sizeof(s_diagnostic_steps[0]);


void App_Manager_Init(void) {

    CLI_Init(&huart1);
		printf("Debug: CLI_Init OK\r\n"); HAL_Delay(10);
    DWIN_Driver_Init(&huart2, Controller_DwinCallback);
		printf("Debug: DWIN_Driver_Init OK\r\n"); HAL_Delay(10);
    EEPROM_Driver_Init(&hi2c1);
		printf("Debug: EEPROM_Driver_Init OK\r\n"); HAL_Delay(10);
    Gerenciador_Config_Init(&hcrc);
		printf("Debug: Gerenciador_Config_Init OK\r\n"); HAL_Delay(10);
    RTC_Driver_Init(&hrtc);
    Medicao_Init();
    DisplayHandler_Init();
    Servos_Init();
    Frequency_Init();
    ADS1232_Init();

    // Etapa 3: Carrega Configura��es
    printf("Sistema Integrado - Log de Inicializacao:\r\n");
    printf("1. Modulos de Software e Drivers... OK\r\n");
    printf("2. Gerenciador de Configuracoes... ");
    if (!Gerenciador_Config_Validar_e_Restaurar()) {
        printf("[AVISO] Restaurado com configs de fabrica.\r\n");
    } else {
        printf("[OK]\r\n");
    }
		CLI_TX_Pump();
    Medicao_Set_Densidade(71.0);
    Medicao_Set_Umidade(25.73);
}

void App_Manager_Process(void) {

    switch (s_current_state) {
        case STATE_ACTIVE:
            Task_Handle_High_Frequency_Polling();
            Medicao_Process();
            DisplayHandler_Process();
            Gerenciador_Config_Run_FSM();

            if (s_go_to_sleep_request) {
                s_go_to_sleep_request = false;
                s_current_state = STATE_STOPPED;
            }
            break;

        case STATE_STOPPED:
            EnterStopMode();
            HandleWakeUpSequence();
            s_current_state = STATE_CONFIRM_WAKEUP;
            break;

        case STATE_CONFIRM_WAKEUP:
            if (s_wakeup_confirmed) {
                s_wakeup_confirmed = false;
                s_current_state = STATE_ACTIVE;
                printf("Confirmado! Retornando ao modo ativo.\r\n");
                App_Manager_Run_Self_Diagnostics(PRINCIPAL);
                break;
            }

            // L�gica de timeout para a tela de confirma��o
            if (HAL_GetTick() - s_confirm_start_tick > 5000) {
                printf("Timeout! Voltando para o modo Stop.\r\n");
                s_current_state = STATE_STOPPED;
                break;
            }

            // Atualiza o contador regressivo na tela
            if (HAL_GetTick() - s_countdown_last_tick >= 1000) {
                s_countdown_last_tick = HAL_GetTick();
                uint32_t elapsed_ms = HAL_GetTick() - s_confirm_start_tick;
                uint32_t remaining_seconds = (elapsed_ms > 5000) ? 0 : (5 - (elapsed_ms / 1000));
                DWIN_Driver_WriteInt(VP_REGRESSIVA, remaining_seconds);
            }
						DWIN_TX_Pump();
            DWIN_Driver_Process();
            break;
    }
}

void App_Manager_Request_Sleep(void) {
    HAL_Delay(500); // Debounce de software para o bot�o de desligar
    s_go_to_sleep_request = true;
}

void App_Manager_Confirm_Wakeup(void) {
    s_wakeup_confirmed = true;
}

bool App_Manager_Run_Self_Diagnostics(uint8_t return_tela) {
    printf("\r\n>>> INICIANDO AUTODIAGNOSTICO <<<\r\n");

    for (size_t i = 0; i < NUM_DIAGNOSTIC_STEPS; i++)
    {
        const DiagnosticStep_t* step = &s_diagnostic_steps[i];

        printf("Diagnostico: %s\r\n", step->description);
        DWIN_Driver_SetScreen(step->screen_id);
        DWIN_TX_Pump(); // Garante que o comando seja enviado
        
        // Espera o tempo de exibi��o da tela
        HAL_Delay(step->display_time_ms);

        // Executa a fun��o de teste associada ao passo
        if (step->execute_test != NULL)
        {
            if (!step->execute_test())
            {
                // A fun��o de teste j� imprimiu a falha e mostrou a tela de erro
                printf(">>> AUTODIAGNOSTICO FALHOU! <<<\r\n");
                return false; // Interrompe em caso de falha
            }
        }
    }

    printf(">>> AUTODIAGNOSTICO COMPLETO <<<\r\n\r\n");
    DWIN_Driver_SetScreen(return_tela);
    DWIN_TX_Pump();
    
    return true;
}

//================================================================================
// Implementa��o das Fun��es Privadas
//================================================================================

/**
 * @brief Agrupa as chamadas de polling de I/O de alta frequ�ncia.
 */
static void Task_Handle_High_Frequency_Polling(void) {
    CLI_TX_Pump();
    DWIN_TX_Pump();
    DWIN_Driver_Process();
    CLI_Process();
    Servos_Process();
}

/**
 * @brief Executa a sequ�ncia para colocar o MCU em modo de baixo consumo.
 */
static void EnterStopMode(void) {
    printf("Entrando em modo Stop...\r\n");

    // Garante que todas as mensagens de log pendentes sejam enviadas
    while (CLI_Driver_IsTxBusy() || DWIN_Driver_IsTxBusy()) {
        CLI_TX_Pump();
        DWIN_TX_Pump();
    }
    HAL_Delay(10);

    HAL_GPIO_WritePin(DISPLAY_PWR_CTRL_GPIO_Port, DISPLAY_PWR_CTRL_Pin, GPIO_PIN_SET);
    HAL_Delay(800);
    HAL_GPIO_WritePin(HAB_TOUCH_GPIO_Port, HAB_TOUCH_Pin, GPIO_PIN_SET);
    HAL_Delay(800);

    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF1);
    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
}

/**
 * @brief Executa a sequ�ncia de hardware e software ap�s o MCU acordar.
 */
static void HandleWakeUpSequence(void) {
    // O c�digo continua daqui quando a interrup��o de toque (EXTI) acorda o MCU
    SystemClock_Config();

    // Reinicializa perif�ricos que perdem configura��o no modo Stop
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    DWIN_Driver_Init(&huart2, Controller_DwinCallback);

    printf("\r\n>>> TOQUE DETECTADO! Entrando em modo de confirmacao... <<<\r\n");

    HAL_GPIO_WritePin(HAB_TOUCH_GPIO_Port, HAB_TOUCH_Pin, GPIO_PIN_RESET);
    HAL_Delay(800);
    HAL_GPIO_WritePin(DISPLAY_PWR_CTRL_GPIO_Port, DISPLAY_PWR_CTRL_Pin, GPIO_PIN_RESET);
    HAL_Delay(800);

    DWIN_Driver_SetScreen(TELA_CONFIRM_WAKEUP);

    // Garante que o comando para mudar de tela seja enviado
    while (DWIN_Driver_IsTxBusy()) {
        DWIN_TX_Pump();
    }

    s_confirm_start_tick = HAL_GetTick();
    s_countdown_last_tick = s_confirm_start_tick;
    s_wakeup_confirmed = false;
}

// --- Implementa��o das Fun��es de Teste Individuais ---

/** @brief Mostra as informa��es de vers�o no display. Sempre retorna sucesso. */
static bool Test_DisplayInfo(void)
{
    char nr_serial_buffer[17];
    Gerenciador_Config_Get_Serial(nr_serial_buffer, sizeof(nr_serial_buffer));
    
    DWIN_Driver_WriteString(VP_HARDWARE, HARDWARE, strlen(HARDWARE));
    DWIN_Driver_WriteString(VP_FIRMWARE, FIRMWARE, strlen(FIRMWARE));
    DWIN_Driver_WriteString(VP_FIRM_IHM, FIRM_IHM, strlen(FIRM_IHM));
    DWIN_Driver_WriteString(VP_SERIAL, nr_serial_buffer, strlen(nr_serial_buffer));
    
    // Espera a transmiss�o terminar para n�o sobrecarregar o buffer do DWIN
    while (DWIN_Driver_IsTxBusy())
    {
        DWIN_TX_Pump();
    }
    return true;
}

/** @brief L�gica de teste para os servos (atualmente apenas visual). */
static bool Test_Servos(void) {
    // Se houvesse uma forma de verificar o feedback dos servos, a l�gica estaria aqui.
    // Como � um teste visual, simplesmente retornamos sucesso.
    return true;
}

/** @brief L�gica de teste para o capac�metro (atualmente apenas visual). */
static bool Test_Capacimetro(void) {
    // Poderia ler a frequ�ncia e verificar se est� dentro de uma faixa esperada.
    return true;
}

/** @brief Executa a tara da balan�a como forma de teste. */
static bool Test_Balanca(void) {
    ADS1232_Tare(); // A pr�pria tara � um bom teste funcional.
    // Para um teste mais robusto, poder�amos verificar se o valor ap�s a tara � pr�ximo de zero.
    return true;
}

/** @brief L� a temperatura inicial e a armazena. */
static bool Test_Termometro(void) {
    float temp_inicial = TempSensor_GetTemperature(); 
    Medicao_Set_Temp_Instru(temp_inicial); // Usa o handler correto para armazenar o dado
    printf("   ... Temperatura: %.2f C\r\n", temp_inicial);
    // Um teste real poderia verificar se a temperatura est� em uma faixa plaus�vel (ex: > 0 e < 100)
    return true;
}

/** @brief Verifica se a comunica��o com a mem�ria EEPROM est� ativa. */
static bool Test_EEPROM(void) {
    if (!EEPROM_Driver_IsReady()) {
        printf("   ... FALHA! EEPROM nao responde.\r\n");
        DWIN_Driver_SetScreen(MSG_ERROR); // Tela de erro gen�rica
        DWIN_TX_Pump();
        return false;
    }
    printf("   ... EEPROM OK.\r\n");
    return true;
}

/** @brief L�gica de teste para o RTC (atualmente apenas visual). */
static bool Test_RTC(void) {
    // O RTC j� foi inicializado. Um teste real poderia ler a data/hora e verificar se s�o v�lidas.
    return true;
}