/*******************************************************************************
 * @file        cli_commands.h
 * @brief       Interface para o m�dulo executor de comandos da CLI.
 * @version     1.0
 * @author      Gabriel Agune
 * @details     Este m�dulo � respons�vel por interpretar e executar os comandos
 * recebidos pela CLI, desacoplando a l�gica de comando do driver
 * de comunica��o (cli_driver).
 ******************************************************************************/

#ifndef CLI_COMMANDS_H
#define CLI_COMMANDS_H

#include "dwin_driver.h"
#include "medicao_handler.h"
#include "rtc_driver.h"
#include "temp_sensor.h"
#include "app_manager.h"
#include "relato.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * @brief Executa um comando CLI com base em uma string de comando e argumentos.
 * @param command Ponteiro para a string que cont�m o nome do comando (ex: "HELP").
 * @param args Ponteiro para a string que cont�m os argumentos do comando, ou NULL se n�o houver.
 */
void CLI_Commands_Execute(const char* command, char* args);

#endif // CLI_COMMANDS_H