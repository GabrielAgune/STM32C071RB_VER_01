/*******************************************************************************
 * @file        Controller.c
 * @brief       Controlador do Sistema em uma arquitetura MVC.
 * @version     Revisado v1.0
 ******************************************************************************/

#include "controller.h"

//================================================================================
// Defini��es, Enums, Vari�veis Est�ticas e Prot�tipos
//================================================================================


// Vari�veis de estado para a l�gica LOCAL (Sele��o de Gr�o)
static int8_t s_indice_grao_selecionado = 0;
static bool s_em_tela_de_selecao = false;
static int16_t received_value = 0;
static uint16_t s_current_screen_id = PRINCIPAL; 
static void Handle_Escape_Navigation(void);


//================================================================================
// Fun��es P�blicas (Getters/Setters)
//================================================================================

/**
 * @brief (V8.3) Retorna a tela que o controlador acredita estar ativa.
 */
uint16_t Controller_GetCurrentScreen(void)
{
    return s_current_screen_id;
}

/**
 * @brief Wrapper P�BLICO para DWIN_Driver_SetScreen que rastreia a tela atual.
 */
void Controller_SetScreen(uint16_t screen_id)
{
    s_current_screen_id = screen_id;
}


//================================================================================
// Fun��o de Callback
//================================================================================
void Controller_DwinCallback(const uint8_t* data, uint16_t len)
{
    if (len < 6 || data[0] != 0x5A || data[1] != 0xA5) {
        return; 
    }

    if (data[3] == 0x83) { 
        uint16_t vp_address = (data[4] << 8) | data[5];
        
        if (len >= 8) {
            if (vp_address != SENHA_CONFIG && vp_address != SET_SENHA && vp_address != SET_TIME)
            {
                 uint8_t payload_len = data[2]; 
                 if (len >= (3 + payload_len)) {
                    received_value = (data[3 + payload_len - 2] << 8) | data[3 + payload_len - 1];
                 }
            } else {
                 received_value = 0; 
            }
        }
        //================================================================
        // DESPACHANTE DE COMANDOS
        //================================================================
        switch (vp_address) {

						//Tela inicial
						case DESCARTA_AMOSTRA		:		Telas_Mede();                                                                          break;
						case SELECT_GRAIN				:		Graos_Handle_Entrada_Tela();                                                        	 break;
						case PRINT							:		Display_ProcessPrintEvent(received_value);                                             break;
            case OFF								:		App_Manager_Request_Sleep();                                                           break;
						case WAKEUP_CONFIRM_BTN :   App_Manager_Confirm_Wakeup();                                                          break;
						
						//Menu Configurar
            case SENHA_CONFIG				:		Auth_ProcessLoginEvent(data, len);                                                     break;
            case SET_TIME						:		RTC_Handle_Set_Time(data, len);                                                        break;
						case NR_REPETICOES      :   Display_SetRepeticoes(received_value);                                                 break;
						case DECIMALS           :   Display_SetDecimals(received_value);                                                   break;
						case DES_HAB_PRINT      :   Display_SetPrintingEnabled(received_value == 0x01);                                    break;
						case SET_SENHA					:		Auth_ProcessSetPasswordEvent(data, len);                                               break;
						case DIAGNOSTIC         :   App_Manager_Run_Self_Diagnostics(TELA_AUTO_DIAGNOSIS);                                 break;
						case USER               :   Display_SetUser(data, len, received_value);                                            break;
						case COMPANY            :   Display_SetCompany(data, len, received_value);                                         break;
						case ABOUT_SYS          :   Display_ShowAbout();                                                                   break;
						
						//Menu Servico
						case PRESET_PRODUCT     :   Display_Preset(received_value);                                                        break;
						case SET_DATE_TIME      :   RTC_Handle_Set_Date_And_Time(data, len);                                               break;
						case MODEL_OEM          :   Display_ShowModel();                                                                   break;						
						case ADJUST_SCALE       :                                                                                          break;
						case ADJUST_TERMO       :                                                                                          break;
						case ADJUST_CAPA        :   Display_Adj_Capa(received_value);                                                      break;
						case SET_SERIAL         :   Display_Set_Serial(data, len, received_value);                                         break;
						case SET_UNITS          :                                                                                          break;
						case MONITOR						:		Controller_SetScreen(TELA_MONITOR_SYSTEM);                                             break;
						case SERVICE_REPORT     :                                                                                          break;
						case SYSTEM_BURNIN      :                                                                                          break;
						
						case TECLAS							:		Graos_Handle_Navegacao(received_value);           																		 break;
						case ESCAPE							:		Handle_Escape_Navigation();																												     break;
						
            default:                                                                 
							break;
        }
    }
}



static void Handle_Escape_Navigation(void)
{

    if (s_current_screen_id != PRINCIPAL) 
    {
         Controller_SetScreen(PRINCIPAL); 
				 DWIN_Driver_SetScreen(TELA_SERVICO);
         printf("CONTROLLER: Saindo do Monitor -> Tela de Servico.\r\n");
    }
    
}