#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../../includes/tools/message.h"
#include "../../includes/utils.h"

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

    /* Verifica se o malloc foi bem sucedido */
    if (new)
    {
        /* Associação das propriedades à mensagem */
        new->pid = pid;
        new->type = type;
        new->time = time;

        /* Cópia do texto para o conteúdo da mensagem */
        strncpy(new->msg, msg, MSG_SIZE);
    }

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
 * A função messageListen abre a leitura do descritor de leitura do fifo e destribui as mensagens
 * que recebe de acordo com as suas necessidades e informações. 
 * 
 * @param listener O descritor de leitura de onde o servidor irá ler as mensagens.
 * 
 * @return O resultado da operação (sucesso ou erro).
 * 
 * @author Guilherme Oliveira
 * @date 16/04/2023
*/
int messageListen(int listener)
{
    /* Variáveis auxiliares de leitura */
    Message buffer = malloc(sizeof(NPMessage)); //!< Buffer de leitura de uma mensagem
    ssize_t bytes_read;                         //!< Número de bytes lidos pelo listener

    /* Verificação da alocação de memória */
    if (buffer == NULL)
        return -1;

    /* Pronto para ouvir sempre que for necessário receber uma mensagem */
    while ((bytes_read = read(listener, buffer, sizeof(NPMessage))) > 0)
    {
        /* Imprime informação de debug */
        printf("Message received from PID %d of type %c at %ld ms with the content %s\n",
                buffer->pid,
                buffer->type,
                buffer->time,
                buffer->msg);
    }

    /* Destruição da variável de buffer de mensagens */
    destroyMessage(buffer);

    /* Verifica se as leituras foram bem sucessidas */
    if (bytes_read < 0)
        return -1;

    /* Termina em sucesso */
    return 0;
}

/**
 * A função messageSend envia uma mensagem ao servidor criando um descritor de escrita
 * para o fifo e escrevendo através dele a informação necessária à mensagem.
 * 
 * @param msg A mensagem que se pretende enviar.
 * 
 * @return O resultado da operação de envio (sucesso ou erro).
 * 
 * @author Guilherme Oliveira
 * @date 17/04/2023
*/
int messageSend(Message msg)
{
    /* Abertura do fifo para escrita */
    int sender = open(FIFO_PATH, O_WRONLY);

    /* Verificação de abertura do fifo */
    if (sender < 0)
        return -1;

    /* Escrita da mensagem no fifo */
    ssize_t bytes_written = write(sender, msg, sizeof(NPMessage));

    /* Fecha o fifo de escrita */
    close(sender);

    /* Verificação da escrita no fifo */
    if (bytes_written < 0)
        return -1;

    /* Termina em sucesso */
    return 0;
}