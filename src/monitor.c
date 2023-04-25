#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../includes/tools/message.h"
#include "../includes/utils.h"

/**
 * A função main irá executar o servidor (monitor) e criar o necessário à
 * sua execução (FIFO e os descritores de escrita e leitura). 
 * 
 * @return O estado em que o programa foi concluído.
 * 
 * @author Guilherme Oliveira
 * @date 23/04/2023
*/
int main()
{
    /* Criação e verificação do fifo de comunicação entre servidor e cliente */
    int create = mkfifo(FIFO_PATH, 0666);
    errorHandler(create, SERVER_NAME);

    /* Abertura e verificação do fifo para leitura */
    int listener = open(FIFO_PATH, O_RDONLY);
    errorHandler(listener, SERVER_NAME);

    /* Abertura e verificação do fifo para escrita (de forma a manter o read bloqueado enquanto não houver escrita) */
    int blocker = open(FIFO_PATH, O_WRONLY);
    errorHandler(blocker, SERVER_NAME);
    
    /* Leitura das mensagens recebidas dos clientes */
    int res = messageListen(listener, printDebugMessage);

    /* Fecho dos escritores e do fifo */
    close(listener);
    close(blocker);
    unlink(FIFO_PATH);

    /* Verificação do processo de leitura das mensagens */
    errorHandler(res, SERVER_NAME);

    /* Conclusão do programa em sucesso */
    return 0;
}