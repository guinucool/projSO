#include <stdlib.h>
#include <sys/time.h>
#include "../includes/utils.h"

/**
 * A função getTimeMilliSeconds devolve o timestamp atual em milisegundos.
 * 
 * @return O tempo em milisegundos ou -1 em caso de erro.
 * 
 * @author Guilherme Oliveira
 * @date 16/04/2023
*/
long getTimeMilliseconds()
{
    /* A estrutura que armazena o tempo em segundos e microsegundos */
    struct timeval current_time;
    
    /* Função que obtém o timestamp de acordo com a estrutura (devolve -1 em caso de erro) */
    if (gettimeofday(&current_time, NULL) != 0)
        return -1;

    /* Variável que irá armazenar o timestamp em milisegundos */
    long milli;

    /* Conversão do timeval para milisegundos */
    milli = current_time.tv_sec * 1000;
    milli += current_time.tv_usec / 1000;

    /* Devolve o tempo em milisegundos */
    return milli;
}