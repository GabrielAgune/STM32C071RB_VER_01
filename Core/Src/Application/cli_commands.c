/*******************************************************************************
 * @file        cli_commands.c
 * @brief       Implementação do executor de comandos da CLI.
 * @version     1.0
 * @author      Gabriel Agune
 * @details     Centraliza a lógica de todos os comandos da CLI em um único
 * módulo, seguindo o Princípio da Responsabilidade Única.
 ******************************************************************************/

#include "cli_commands.h"


//================================================================================
// Tipos e Estruturas
//================================================================================
typedef void (*CommandHandler)(char* args);

typedef struct {
    const char* name;
    CommandHandler handler;
} CliCommand_t;

//================================================================================
// Protótipos de Funções Privadas
//================================================================================

// --- Handlers de Comando Principais ---
static void Cmd_Help(char* args);
static void Cmd_Dwin(char* args);
static void Cmd_GetPeso(char* args);
static void Cmd_GetTemp(char* args);
static void Cmd_GetFreq(char* args);
static void Cmd_Service(char* args);
static void Cmd_SetTime(char* args);
static void Cmd_WhoAmI(char* args);
static void Cmd_SetDate(char* args);

// --- Handlers de Subcomando DWIN ---
static void Handle_Dwin_PIC(char* sub_args);
static void Handle_Dwin_INT(char* sub_args);
static void Handle_Dwin_INT32(char* sub_args);
static void Handle_Dwin_RAW(char* sub_args);

// --- Funções Auxiliares ---
static uint8_t HexCharToValue(char c);
static void TokenizeArguments(char* buffer, char** primary, char** secondary);
static void FindAndExecuteCommand(const CliCommand_t* command_table, size_t table_size,
                                  const char* command_name, char* args, const char* unknown_msg);

//================================================================================
// Tabela de Comandos e Constantes
//================================================================================

static const CliCommand_t s_command_table[] = {
    {"HELP",    Cmd_Help},       {"?",       Cmd_Help},
    {"DWIN",    Cmd_Dwin},       {"PESO",    Cmd_GetPeso},
    {"TEMP",    Cmd_GetTemp},    {"FREQ",    Cmd_GetFreq},
    {"SERVICE", Cmd_Service},    {"WHO_AM_I",Cmd_WhoAmI},
    {"TIME",    Cmd_SetTime},    {"DATE",    Cmd_SetDate},
};
static const size_t NUM_COMMANDS = sizeof(s_command_table) / sizeof(s_command_table[0]);

static const CliCommand_t s_dwin_subcommand_table[] = {
    {"PIC",   Handle_Dwin_PIC},    {"INT",   Handle_Dwin_INT},
    {"INT32", Handle_Dwin_INT32},  {"RAW",   Handle_Dwin_RAW}
};
static const size_t NUM_DWIN_SUBCOMMANDS = sizeof(s_dwin_subcommand_table) / sizeof(s_dwin_subcommand_table[0]);

static const char HELP_TEXT[] =
    "============================= CLI de Diagnostico ==========================|\r\n"
    "| HELP ou ?                | Mostra esta ajuda.                            |\r\n"
    "| WHO_AM_I                 | Exibe especificacoes do sistema.              |\r\n"
    "| TIME HH:MM:SS            | Define a hora do sistema.                     |\r\n"
    "| DATE DD/MM/AA            | Define a data do sistema.                     |\r\n"
    "| PESO                     | Mostra a leitura atual da balanca.            |\r\n"
    "| TEMP                     | Mostra a leitura do sensor de temperatura.    |\r\n"
    "| FREQ                     | Mostra a ultima leitura de frequencia.        |\r\n"
    "| SERVICE                  | Entra na tela de servico.                     |\r\n"
    "| DWIN PIC <id>            | Muda a tela (ex: DWIN PIC 1).                 |\r\n"
    "| DWIN INT <addr_h> <val>  | Escreve int16 no VP (ex: DWIN INT 2190 1234). |\r\n"
    "| DWIN RAW <bytes_hex>     | Envia bytes crus para o DWIN (ex: 5AA5...).   |\r\n"
    "===========================================================================|\r\n";

//================================================================================
// Função Pública de Execução
//================================================================================

void CLI_Commands_Execute(const char* command, char* args) {
    if (command == NULL || *command == '\0') {
        return;
    }
    FindAndExecuteCommand(s_command_table, NUM_COMMANDS, command, args, "Comando desconhecido");
}

//================================================================================
// Implementação dos Handlers de Comando
//================================================================================

static void Cmd_Help(char* args) {
    printf("%s", HELP_TEXT);
}

static void Cmd_Service(char* args) {
    DWIN_Driver_SetScreen(TELA_SERVICO);
}

static void Cmd_GetPeso(char* args) {
    DadosMedicao_t dados_atuais;
    Medicao_Get_UltimaMedicao(&dados_atuais);
    printf("Dados da Balanca:\r\n  - Peso: %.2f g\r\n", dados_atuais.Peso);
}

static void Cmd_GetTemp(char* args) {
    float temperatura = TempSensor_GetTemperature();
    printf("Temperatura interna do MCU: %.2f C\r\n", temperatura);
}

static void Cmd_GetFreq(char* args) {
    DadosMedicao_t dados_atuais;
    Medicao_Get_UltimaMedicao(&dados_atuais);
    printf("Dados de Frequencia:\r\n");
    printf("  - Pulsos (em 1s): %.1f\r\n", dados_atuais.Frequencia);
    printf("  - Escala A (calc): %.2f\r\n", dados_atuais.Escala_A);
}

static void Cmd_Dwin(char* args) {
    if (args == NULL) {
        printf("Subcomando DWIN faltando. Use 'HELP'.\r\n");
        return;
    }
    char* sub_command;
    char* sub_args;
    TokenizeArguments(args, &sub_command, &sub_args);

    FindAndExecuteCommand(s_dwin_subcommand_table, NUM_DWIN_SUBCOMMANDS,
                          sub_command, sub_args, "Subcomando DWIN desconhecido");
}

static void Cmd_SetTime(char* args) {
    if (args == NULL) {
        printf("Erro: Faltam argumentos. Uso: TIME HH:MM:SS\r\n");
        return;
    }
    uint8_t h, m, s;
    if (sscanf(args, "%hhu:%hhu:%hhu", &h, &m, &s) == 3 && h < 24 && m < 60 && s < 60) {
        if (RTC_Driver_SetTime(h, m, s)) {
            printf("OK. RTC atualizado para %02u:%02u:%02u\r\n", h, m, s);
        } else {
            printf("Erro: Falha ao setar a hora no hardware do RTC.\r\n");
        }
    } else {
        printf("Erro: Formato ou valores invalidos. Uso: TIME HH(0-23):MM(0-59):SS(0-59).\r\n");
    }
}

static void Cmd_SetDate(char* args) {
    if (args == NULL) {
        printf("Erro: Faltam argumentos. Uso: DATE DD/MM/AA\r\n");
        return;
    }
    uint8_t d, m, a;
    if (sscanf(args, "%hhu/%hhu/%hhu", &d, &m, &a) == 3 && d >= 1 && d <= 31 && m >= 1 && m <= 12) {
        if (RTC_Driver_SetDate(d, m, a)) {
            printf("OK. RTC atualizado para %02u/%02u/%02u\r\n", d, m, a);
        } else {
            printf("Erro: Falha ao setar a data no hardware do RTC.\r\n");
        }
    } else {
        printf("Erro: Formato ou valores invalidos. Uso: DATE DD(1-31)/MM(1-12)/AA(00-99).\r\n");
    }
}

static void Cmd_WhoAmI(char* args) {
    Who_am_i();
}

static void Handle_Dwin_PIC(char* sub_args) {
    if (sub_args == NULL) {
        printf("Uso: DWIN PIC <id>");
        return;
    }
    DWIN_Driver_SetScreen(atoi(sub_args));
    printf("Comando DWIN PIC enfileirado.");
}

static void Handle_Dwin_INT(char* sub_args) {
    if (sub_args == NULL) {
        printf("Uso: DWIN INT <addr_hex> <valor>");
        return;
    }
    char* val_str;
    char* addr_str;
    TokenizeCommand(sub_args, &addr_str, &val_str);

    if (val_str == NULL) {
        printf("Valor faltando.");
        return;
    }
    uint16_t vp = strtol(addr_str, NULL, 16);
    int16_t val = atoi(val_str);
    DWIN_Driver_WriteInt(vp, val);
    printf("Enfileirado (int16) %d em 0x%04X", val, vp);
}

static void Handle_Dwin_INT32(char* sub_args) {
    if (sub_args == NULL) {
        printf("Uso: DWIN INT32 <addr_hex> <valor>");
        return;
    }
    char* val_str;
    char* addr_str;
    TokenizeCommand(sub_args, &addr_str, &val_str);

    if (val_str == NULL) {
        printf("Valor faltando.");
        return;
    }
    uint16_t vp = strtol(addr_str, NULL, 16);
    int32_t val = atol(val_str);
    DWIN_Driver_WriteInt32(vp, val);
    printf("Enfileirado (int32) %ld em 0x%04X", (long)val, vp);
}

static void Handle_Dwin_RAW(char* sub_args) {
    if (sub_args == NULL) {
        printf("Uso: DWIN RAW <byte_hex> ...");
        return;
    }
    uint8_t raw_buffer[CLI_RX_BUFFER_SIZE / 2];
    int byte_count = 0;
    char* ptr = sub_args;

    while (*ptr != '\0' && byte_count < (CLI_RX_BUFFER_SIZE / 2)) {
        while (isspace((unsigned char)*ptr)) ptr++;
        if (*ptr == '\0') break;

        char high_c = *ptr++;
        if (*ptr == '\0' || isspace((unsigned char)*ptr)) {
            printf("\nErro: Numero impar de caracteres hex.");
            return;
        }
        char low_c = *ptr++;
        uint8_t high_v = HexCharToValue(high_c);
        uint8_t low_v = HexCharToValue(low_c);

        if (high_v == 0xFF || low_v == 0xFF) {
            printf("\nErro: Caractere invalido na string hex.");
            return;
        }
        raw_buffer[byte_count++] = (high_v << 4) | low_v;
    }

    printf("Enfileirando %d bytes para DWIN:", byte_count);
    for (int i = 0; i < byte_count; i++) {
        printf(" %02X", raw_buffer[i]);
    }
    DWIN_Driver_WriteRawBytes(raw_buffer, byte_count);
}
