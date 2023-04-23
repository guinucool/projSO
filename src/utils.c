#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include "../includes/utils.h"

/* Variável que irá armazenar o tipo de erro */
extern int errno;

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

/**
 * A função errorHandler imprime uma mensagem de erro e termina o programa como falhado.
 * 
 * @param err A variável a verificar a ocorrência do erro.
 * @param prog O nome do programa do erro (cliente ou servidor)
 * 
 * @author Guilherme Oliveira
 * @date 23/04/2023
*/
void errorHandler(int err, char prog[])
{
    /* Verifica a ocorrência de um erro */
    if (err < 0) {
        perror(prog);
        exit(1);
    }
}

/**
 * A função argHandler verifica se o número de argumentos está de acordo com o necessário
 * e imprime a mensagem de erro de acordo.
 * 
 * @param err O número de argumentos recebido.
 * @param arg O número de argumentos suposto.
 * @param prog O nome do programa do erro (cliente ou servidor).
 * 
 * @author Guilherme Oliveira
 * @date 23/04/2023
*/
void argHandler(int err, int arg, char prog[])
{
    /* Verifica a existência de argumentos suficientes */
    if (err < arg) {
        errno = EINVAL;
        errorHandler(-1, prog);
    }
}