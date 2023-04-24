#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
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

/**
 * A função errorChildHandler imprime uma mensagem de erro e termina o processo-filho como falhado.
 * 
 * @param err A variável a verificar a ocorrência do erro.
 * @param prog O nome do programa do erro (cliente ou servidor)
 * 
 * @author Guilherme Oliveira
 * @date 23/04/2023
*/
void errorChildHandler(int err, char prog[])
{
    /* Verifica a ocorrência de um erro */
    if (err < 0) {
        perror(prog);
        _exit(1);
    }
}

/**
 * A função destroyCharArr destroí um array de strings.
 * 
 * @param arr O array a ser destruído.
 * 
 * @author Guilherme Oliveira
 * @date 23/04/2023
*/
void destroyCharArr(char ** arr)
{
    /* Apenas liberta o array se este existir */
    if (arr)
    {
        /* Liberta todas as posições até agora preenchidas */
        for (int i = 0; arr[i]; i++)
            free(arr[i]);
                
        /* Liberta o array */
        free(arr);
    }
}

/**
 * A função getArgv cria um array de strings que irão armazenar os vários argumentos de um programa
 * definindo sempre a última posição como NULL.
 * 
 * @param cmd O comando a converter para array de argumentos.
 * 
 * @return O array de argumentos criado.
 * 
 * @author Guilherme Oliveira
 * @date 23/04/2023
*/
char ** getArgv(char cmd[])
{
    /* Variável que irá armazenar o array de resultado */
    char ** argv = malloc(sizeof(char*));

    /* Só executa a divisão pelo array caso o argv seja criado */
    if (argv)
    {
        /* Variável que guarda o tamanho atual do array */
        int size = 1;

        /* Última posição deverá ser sempre NULL */
        argv[0] = NULL;

        /* Executa a divisão dos argumentos e comando pelo array */
        for (char * str = strtok(cmd, " "); str; str = strtok(NULL, " "))
        {
            /* Aumenta o array */
            char ** tmp = realloc(argv, (size+1) * sizeof(char*));

            /* Verifica se o aumento foi bem sucedido */
            if (!tmp) {

                /* Liberta o array até agora */
                destroyCharArr(argv);

                /* Termina a função em erro */
                return NULL;
            }

            /* Torna o aumento efetivo */
            argv = tmp;
            argv[size] = NULL;

            /* Calcula o espaço necessário ao próximo argumento */
            int alloc = strlen(str) + 1;

            /* Cria o espaço necessário para armazenar o próximo argumento */
            argv[size - 1] = malloc(alloc * sizeof(char*));

            /* Verifica se a criação de espaço foi bem sucedida */
            if (!argv[size-1])
            {
                /* Liberta o array até agora */
                destroyCharArr(argv);

                /* Termina a função em erro */
                return NULL;
            }

            /* Copia a string para o espaço destinado à mesma */
            strncpy(argv[size-1], str, alloc);

            /* Aumenta o contador do tamanho do array */
            size++;
        }
    }

    /* Devolve o array criado */
    return argv;
}