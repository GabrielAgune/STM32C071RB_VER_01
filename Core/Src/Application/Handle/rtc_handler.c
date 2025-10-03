
#include "rtc_handler.h"

//================================================================================
// Defini��es Internas
//================================================================================

typedef enum {
    RTC_SET_OK,
    RTC_SET_FAIL_PARSE,
    RTC_SET_FAIL_HW
} RtcSetResult_t;

typedef struct {
    uint8_t day;
    uint8_t month;
    uint8_t year;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} RtcData_t;


//================================================================================
// Prot�tipos das Fun��es de L�gica Pura (Est�ticas)
//================================================================================

static RtcSetResult_t rtc_handle_set_date_and_time_logic(const uint8_t* dwin_data, uint16_t len, RtcData_t* out_data);
static RtcSetResult_t rtc_handle_set_time_logic(const uint8_t* dwin_data, uint16_t len, RtcData_t* out_data);
static void rtc_update_display(const RtcData_t* data);

//================================================================================
// Fun��es P�blicas (Processadores de Evento)
//================================================================================

void RTC_Handle_Set_Time(const uint8_t* dwin_data, uint16_t len)
{
    RtcData_t parsed_data;

    // 1. Chama a NOVA fun��o de l�gica que s� mexe na hora
    RtcSetResult_t result = rtc_handle_set_time_logic(dwin_data, len, &parsed_data);

    // 2. Age sobre o resultado
    if (result == RTC_SET_OK) {
        printf("RTC Handler: HORA atualizada com sucesso. Atualizando display.\r\n");
        rtc_update_display(&parsed_data);
    } else {
        printf("RTC Handler: Falha ao atualizar HORA. Nenhum feedback para o usuario.\r\n");
    }
}

void RTC_Handle_Set_Date_And_Time(const uint8_t* dwin_data, uint16_t len)
{
    RtcData_t parsed_data;
		
    RtcSetResult_t result = rtc_handle_set_date_and_time_logic(dwin_data, len, &parsed_data);

    if (result == RTC_SET_OK)
    {
        printf("RTC Handler: RTC atualizado com sucesso. Atualizando display.\r\n");
        rtc_update_display(&parsed_data);
    }
    else
    {
        printf("RTC Handler: Falha ao atualizar RTC. Nenhum feedback para o usuario.\r\n");
    }
}


//================================================================================
// Implementa��o da L�gica Pura e A��es de UI (Fun��es Est�ticas)
//================================================================================

static RtcSetResult_t rtc_handle_set_date_and_time_logic(const uint8_t* dwin_data, uint16_t len, RtcData_t* out_data)
{
    char parsed_string[32] = {0};
    
    // 1. Extrair a string do payload DWIN
    const uint8_t* payload = &dwin_data[6];
    uint16_t payload_len = len - 6;
    if (!DWIN_Parse_String_Payload_Robust(payload, payload_len, parsed_string, sizeof(parsed_string))) {
        printf("RTC Logic: Falha ao extrair string.\r\n");
        return RTC_SET_FAIL_PARSE;
    }
    printf("RTC Logic: Recebido string '%s'\r\n", parsed_string);

    // 2. Tentar extrair data e hora da string
    uint8_t d=0, m=0, y=0, h=0, min=0, s=0;
    bool date_found = false;
    bool time_found = false;

    if (sscanf(parsed_string, "%hhu/%hhu/%hhu %hhu:%hhu:%hhu", &d, &m, &y, &h, &min, &s) == 6) {
        date_found = true; time_found = true;
    } else if (sscanf(parsed_string, "%hhu/%hhu/%hhu", &d, &m, &y) == 3) {
        date_found = true;
    } else if (sscanf(parsed_string, "%hhu:%hhu:%hhu", &h, &min, &s) == 3) {
        time_found = true;
    }

    if (!date_found && !time_found) {
        printf("RTC Logic: Formato de string irreconhecivel.\r\n");
        return RTC_SET_FAIL_PARSE;
    }

    // 3. Se a atualiza��o for parcial, busca os dados atuais do hardware
    if (!time_found) { RTC_Driver_GetTime(&h, &min, &s); }
    if (!date_found) { RTC_Driver_GetDate(&d, &m, &y); }

    // 4. Aplica as novas configura��es ao hardware
    if (date_found) {
        if (!RTC_Driver_SetDate(d, m, y)) return RTC_SET_FAIL_HW;
    }
    if (time_found) {
        if (!RTC_Driver_SetTime(h, min, s)) return RTC_SET_FAIL_HW;
    }
    
    // 5. Preenche a struct de sa�da para a camada de UI usar
    out_data->day = d; out_data->month = m; out_data->year = y;
    out_data->hour = h; out_data->minute = min; out_data->second = s;
    
    return RTC_SET_OK;
}

static RtcSetResult_t rtc_handle_set_time_logic(const uint8_t* dwin_data, uint16_t len, RtcData_t* out_data)
{
    char parsed_string[32] = {0};

    // CORRE��O: O payload da string come�a no �ndice 8.
    const uint8_t* payload = &dwin_data[8];
    uint16_t payload_len = len - 8;
    if (!DWIN_Parse_String_Payload_Robust(payload, payload_len, parsed_string, sizeof(parsed_string))) {
        printf("RTC Logic (TimeOnly): Falha ao extrair string.\r\n");
        return RTC_SET_FAIL_PARSE;
    }
    printf("RTC Logic (TimeOnly): Recebido string '%s'\r\n", parsed_string);

    uint8_t h=0, min=0, s=0;

    // Tenta extrair APENAS a hora da string
    if (sscanf(parsed_string, "%hhu:%hhu:%hhu", &h, &min, &s) != 3) {
        printf("RTC Logic (TimeOnly): Formato de string invalido. Esperado HH:MM:SS.\r\n");
        return RTC_SET_FAIL_PARSE;
    }

    // Aplica a nova hora ao hardware
    if (!RTC_Driver_SetTime(h, min, s)) {
        return RTC_SET_FAIL_HW;
    }

    // Busca a data atual do hardware para poder atualizar o display corretamente
    uint8_t d, m, y;
    if (!RTC_Driver_GetDate(&d, &m, &y)) {
        return RTC_SET_FAIL_HW; // Se n�o conseguir ler a data, retorna erro.
    }
    
    // Preenche a struct de sa�da com a data atual e a nova hora
    out_data->day = d; out_data->month = m; out_data->year = y;
    out_data->hour = h; out_data->minute = min; out_data->second = s;

    return RTC_SET_OK;
}

/**
 * @brief Fun��o dedicada a enviar os dados de data/hora para o display
 */
static void rtc_update_display(const RtcData_t* data)
{
    char buffer_display[20];
    
    snprintf(buffer_display, sizeof(buffer_display), "%02d/%02d/%02d", data->day, data->month, data->year);
    DWIN_Driver_WriteString(DATA_SISTEMA, buffer_display, strlen(buffer_display));

    snprintf(buffer_display, sizeof(buffer_display), "%02d:%02d:%02d", data->hour, data->minute, data->second);
    DWIN_Driver_WriteString(HORA_SISTEMA, buffer_display, strlen(buffer_display));
}