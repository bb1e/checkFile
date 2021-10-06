/**
 * @file memory.h
 * @brief Macros para gestão de memória
 *
 * Macros que a serem empregues nas aplicações que requerem o uso das
 * funções malloc e free
 * @version 1
 */
#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdlib.h>

void *eipa_malloc(size_t size, const int line, const char *file);
void eipa_free(void **ptr, const int line, const char *file);
void *swap_bytes(void *source, void *dest, size_t num_bytes);

/**
 * Macro para alocar memória.
 *
 * @return retorna o bloco de memória alocado
 */
#define MALLOC(size) eipa_malloc((size), __LINE__, __FILE__)

/**
 * Macro para libertar memória. Coloca o ponteiro a NULL.
 *
 * @return Não retorna nada
 */
#define FREE(ptr) eipa_free((void **)(&(ptr)), __LINE__, __FILE__)

#endif /* _MEMORY_H_ */
