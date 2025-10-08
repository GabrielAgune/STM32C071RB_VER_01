/*******************************************************************************
 * @file        cli_commands.h
 * @brief       Interface para o módulo executor de comandos da CLI.
 * @version     1.0
 * @author      Gabriel Agune
 * @details     Este módulo é responsável por interpretar e executar os comandos
 * recebidos pela CLI, desacoplando a lógica de comando do driver
 * de comunicação (cli_driver).
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
 * @param command Ponteiro para a string que contém o nome do comando (ex: "HELP").
 * @param args Ponteiro para a string que contém os argumentos do comando, ou NULL se não houver.
 */
void CLI_Commands_Execute(const char* command, char* args);

#endif // CLI_COMMANDS_H