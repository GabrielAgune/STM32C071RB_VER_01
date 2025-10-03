#ifndef DWIN_PARSER_H
#define DWIN_PARSER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief (V8.3) Parser de string robusto para payloads DWIN.
 * Extrai uma string de um payload, ignorando 0xFF e caracteres n�o imprim�veis.
 * Garante a termina��o nula.
 *
 * @param payload Ponteiro para o in�cio do payload (ap�s o cabe�alho do VP).
 * @param payload_len Comprimento do payload.
 * @param out_buffer Buffer de sa�da para a string.
 * @param max_len Tamanho m�ximo do buffer de sa�da (incluindo nulo).
 * @return true se o parsing foi bem-sucedido (mesmo que a string esteja vazia), false em caso de erro de par�metro.
 */
bool DWIN_Parse_String_Payload_Robust(const uint8_t* payload, uint16_t payload_len, char* out_buffer, uint8_t max_len);

#endif // DWIN_PARSER_H