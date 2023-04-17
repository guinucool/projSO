#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../../includes/tools/message.h"

/**
 * @struct __MESSAGE__
 * @brief Estrutura que define uma mensagem.
 * 
 * Esta estrutura define o tipo de mensagem que é enviado
 * entre o servidor e o cliente. 
*/
typedef struct __MESSAGE__ {
    pid_t pid;
    char type;
    char msg[MSG_SIZE];
    long time;
}*Message, NPMessage;

/**
 * A função createMessage cria e aloca o espaço necessário a uma variável do tipo Message.
 * 
 * @param pid O pid associado ao pedido da mensagem.
 * @param type O tipo de pedido da mensagem.
 * @param msg O conteúdo do pedido da mensagem.
 * @param time O timestamp em milisegundos do pedido associado à mensagem.
 * 
 * @return A variável do tipo Message criada.
 * 
 * @author Guilherme Oliveira
 * @date 17/04/2023
*/
Message createMessage(pid_t pid, char type, char msg[], long time)
{
    /* Criação e alocação do espaço necessário */
    Message new = malloc(sizeof(NPMessage));

    /* Associação das propriedades à mensagem */
    new->pid = pid;
    new->type = type;
    new->time = time;

    /* Cópia do texto para o conteúdo da mensagem */
    strncpy(new->msg, msg, MSG_SIZE);

    /* Devolve o resultado da criação */
    return new;
}

/**
 * A função destroyMessage destroí uma variável do tipo Message.
 * 
 * @param msg A mensagem a ser destruída.
 * 
 * @author Guilherme Oliveira
 * @date 17/04/2023
*/
void destroyMessage(Message msg)
{
    /* Liberta o espaço ocupado pela variável caso ela exista */
    if (msg)
        free(msg);
}

/**
 * @brief 
 * 
 * @return int 
 * 
 * @author Guilherme Oliveira
 * @date 16/04/2023
*/
int messageListen()
{
    /* Criação do fifo de comunicação entre servidor e cliente */
    int create = mkfifo("/tmp/serverListen", 0666);

    /* Verificação de criação do fifo */
    if (create < 0)
        return -1;

    /* Variáveis auxiliares de leitura */
    Message buffer = malloc(sizeof(NPMessage));
    int bytes_read;

    /* Verificação da alocação de memória */
    if (buffer == NULL)
        return -1;

    /* Pronto para ouvir sempre que for necessário receber uma mensagem */
    while (1)
    {
        /* Abertura do fifo para leitura */
        int listener = open("/tmp/serverListen", O_RDONLY);

        /* Verificação de abertura do fifo */
        if (listener < 0)
            return -1;

        /* Lê o fifo assim que houver conteúdo para ler */
        bytes_read = read(listener, buffer, sizeof(NPMessage));

        /* Verifica se a leitura foi bem sucessida */
        if (bytes_read < 0)
            return -1;

        /* Imprime informação de debug */
        printf("Message received from PID %d of type %c at %ld ms with the content %s\n",
                buffer->pid,
                buffer->type,
                buffer->time,
                buffer->msg);

        /* Fecha o fifo de leitura */
        close(listener);
    }

    /* Destruição da variável de buffer de mensagens */
    destroyMessage(buffer);

    /* Apaga o fifo criado */
    unlink("/tmp/serverListen");

    /* Termina em sucesso */
    return 0;
}

/**
 * @brief
 * 
 * @param msg 
 * 
 * @return int 
 * 
 * @author Guilherme Oliveira
 * @date 17/04/2023
*/
int messageSend(Message msg)
{
    /* Abertura do fifo para escrita */
    int sender = open("/tmp/serverListen", O_WRONLY);

    /* Verificação de abertura do fifo */
    if (sender < 0)
        return -1;

    /* Escrita da mensagem no fifo */
    int bytes_written = write(sender, msg, sizeof(NPMessage));

    /* Verificação da escrita no fifo */
    if (bytes_written < 0)
        return -1;

    /* Fecha o fifo de escrita */
    close(sender);

    /* Termina em sucesso */
    return 0;
}