#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../includes/tools/message.h"
#include "../includes/tools/process.h"
#include "../includes/utils.h"

/**
 * A função main irá executar o cliente (tracer) e correrá os processos e envios necessários
 * de acordo com as suas opções de execução.
 * 
 * @param argc O número de argumentos com que o cliente foi executado.
 * @param argv Os argumentos com que o cliente foi executado.
 * 
 * @return O estado em que o programa foi concluído.
 * 
 * @author Guilherme Oliveira
 * @date 25/04/2023
*/
int main(int argc, char * argv[])
{
    /* Verificação do número de argumentos recebidos */
    argHandler(argc, 2, CLIENT_NAME);

    /* Caso o modo de execução seja execute */
    if (strcmp(argv[1], "execute") == 0)
    {
        /* Verificação do número de argumentos recebidos */
        argHandler(argc, 4, CLIENT_NAME);

        /* Caso a flag do execute seja a de um programa individual */
        if(strcmp(argv[2], "-u") == 0)
        {
            int res = executeProcess(argv[3]);
            errorHandler(res, CLIENT_NAME);
        }

        /* Caso a flag do execute seja desconhecida */
        else
            argHandler(-1, 0, CLIENT_NAME);
    }

    /* Caso o modo de execução seja status */
    else if (strcmp(argv[1], "status") == 0)
    {
        int res = messageResquest(TYPE_STATUSREQUEST, argv, argc, printStatusMessage);
        errorHandler(res, CLIENT_NAME);
    }

    /* Caso o modo de execução seja stats-time */
    else if (strcmp(argv[1], "stats-time") == 0)
    {
        int res = messageResquest(TYPE_STATSTIMEREQUEST, argv, argc, printStatsTimeMessage);
        errorHandler(res, CLIENT_NAME);
    }

    /* Caso o modo de execução seja stats-command */
    else if (strcmp(argv[1], "stats-command") == 0)
    {
        int res = messageResquest(TYPE_STATSCMDREQUEST, argv, argc, printStatsCmdMessage);
        errorHandler(res, CLIENT_NAME);
    }

    /* Caso o modo de execução seja stats-uniq */
    else if (strcmp(argv[1], "stats-uniq") == 0)
    {
        int res = messageResquest(TYPE_STATSUNIQREQUEST, argv, argc, printStatsUniqMessage);
        errorHandler(res, CLIENT_NAME);
    }

    /* Caso o modo de execução seja desconhecido */
    else
        argHandler(-1, 0, CLIENT_NAME);

    /* Conclusão do programa em sucesso */
    return 0;
}