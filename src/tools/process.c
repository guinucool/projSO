#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "../../includes/tools/message.h"
#include "../../includes/tools/process.h"
#include "../../includes/utils.h"

/**
 * @struct __PROCESS__
 * @brief Estrutura que define um processo.
 * 
 * Esta estrutura define a informação necessária ao armazenamento da informação
 * acerca de um processo que se encontra em execução.
*/
typedef struct __PROCESS__ {
    pid_t pid;
    char exec[EXEC_MAX];
    long time;
    char finished;
}*Process, NPProcess;

/**
 * A função executeProcess executa um processo, enviando a informação necessária ao servidor, assim como,
 * criando o processo-filho necessário à sua execução.
 * 
 * @param cmd O comando a ser executado pelo processo.
 * 
 * @return O sucesso da operação (sucesso ou erro).
 * 
 * @author Guilherme Oliveira
 * @date 24/04/2023
*/
int executeProcess(char cmd[])
{
    /* Variável que irá armazenar os argumentos do programa */
    char ** exec = getArgv(cmd);

    /* Verificação do sucesso na criação do array que irá guardar os argumentos do array */
    if (exec == NULL)
        return -1;

    /* Descobre o timestamp de início do processo */
    long start = getTimeMilliseconds();

    /* Cria o processo-filho que executa o programa */
    if (fork() == 0)
    {
        /* Descobre o pid do processo-filho */
        pid_t pid = getpid();

        /* Notificação de um processo ao servidor e verificação do sucesso da notificação */
        int notify = messageSend(pid, TYPE_PROCESS_START, exec[0], start, FIFO_PATH);
        errorChildHandler(notify, CLIENT_NAME);

        /* Imprime notificação do programa a correr */
        printf("Running PID %d\n", pid);

        /* Executa e verifica a execução do programa */
        int res = execvp(exec[0], exec);
        destroyCharArr(exec);
        errorChildHandler(res, CLIENT_NAME);

        /* Retorna o resultado do programa */
        _exit(0);
    }

    /* Variável que irá verificar o estado final do programa que foi executado */
    int status;

    /* Espera que o processo-filho termine a execução */
    pid_t cpid = wait(&status);

    /* Descobre o timestamp no fim da execução do processo */
    long end = getTimeMilliseconds();

    /* Verifica o estado de conclusão do processo */
    if (WIFEXITED(status)) {

        /* Verifica se algum processo dentro do filho falhou*/
        if(WEXITSTATUS(status))
            exit(1);
    }
    else
        return -1;

    /* Imprime o tempo que o programa demorou */
    printf("Ended in %ld ms\n", (end - start));

    /* Notificação do final do processo ao servidor */
    int notify = messageSend(cpid, TYPE_PROCESS_END, exec[0], end, FIFO_PATH);

    /* Destroí o array auxiliar de argumentos */
    destroyCharArr(exec);
    
    /* Verificação do sucesso da notificação */
    if (notify < 0)
        return -1;

    /* Termina o processo em sucesso */
    return 0;
}