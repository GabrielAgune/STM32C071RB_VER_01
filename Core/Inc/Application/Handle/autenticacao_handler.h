#ifndef AUTENTICACAO_HANDLER_H
#define AUTENTICACAO_HANDLER_H

#include "dwin_driver.h"
#include "controller.h"
#include "dwin_parser.h"
#include "gerenciador_configuracoes.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/**
 * @brief Processa o evento de tentativa de login vindo do controller.
 * Esta fun��o cont�m toda a l�gica, desde o parsing at� a atualiza��o da UI.
 * @param dwin_data Ponteiro para o buffer de dados brutos DWIN.
 * @param len Comprimento do buffer de dados.
 */
void Auth_ProcessLoginEvent(const uint8_t* dwin_data, uint16_t len);

/**
 * @brief Processa o evento de defini��o de nova senha vindo do controller.
 * @param dwin_data Ponteiro para o buffer de dados brutos DWIN.
 * @param len Comprimento do buffer de dados.
 */
void Auth_ProcessSetPasswordEvent(const uint8_t* dwin_data, uint16_t len);


#endif // AUTENTICACAO_HANDLER_H