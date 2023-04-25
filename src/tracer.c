#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../includes/tools/message.h"
#include "../includes/tools/process.h"
#include "../includes/utils.h"

/**
 * 
 * 
 * @return int 
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
        int res = processStatusResquest();
        errorHandler(res, CLIENT_NAME);
    }

    /* Caso o modo de execução seja desconhecido */
    else
        argHandler(-1, 0, CLIENT_NAME);

    /* Conclusão do programa em sucesso */
    return 0;
}