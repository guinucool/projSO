#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../../includes/tools/message.h"
#include "../../includes/tools/process.h"
#include "../../includes/tools/dynarray.h"
#include "../../includes/tools/register.h"
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
 * @param printer A função que imprime uma mensagem de acordo com o pretendido quando a recebe.
 * @param pidpath O caminho para a pasta de pids (caso seja necessário).
 * 
 * @return O resultado da operação (sucesso ou erro).
 * 
 * @author Guilherme Oliveira
 * @date 16/04/2023
*/
int messageListen(int listener, void (*printer)(Message), char * pidpath)
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
        /* Imprime informação da mensagem */
        printer(buffer);

        /* Executa o processo de adição do processo à fila, caso seja o caso */
        if (buffer->type == TYPE_PROCESS_START)
            addProcessQueue(buffer->pid, buffer->msg, buffer->time);

        /* Executa o término do processo na fila, caso seja o caso */
        if (buffer->type == TYPE_PROCESS_END || buffer->type == TYPE_PROCESS_FAIL)
            removeProcessQueue(buffer->pid, buffer->time, pidpath, buffer->type);

        /* Executa um pedido status da fila de processos, caso seja o caso */
        if (buffer->type == TYPE_STATUSREQUEST)
            mapProcessQueue(buffer->pid);

        /* Executa um pedido stats-time de processos, caso seja o caso */
        if (buffer->type == TYPE_STATSTIMEREQUEST)
            registerStats(buffer->pid, TYPE_STATSTIME, buffer->msg, pidpath);

        /* Executa um pedido stats-command de processos, caso seja o caso */
        if (buffer->type == TYPE_STATSCMDREQUEST)
            registerStats(buffer->pid, TYPE_STATSCMD, buffer->msg, pidpath);

        /* Executa um pedido stats-uniq de processos, caso seja o caso */
        if (buffer->type == TYPE_STATSUNIQREQUEST)
            registerStats(buffer->pid, TYPE_STATSUNIQ, buffer->msg, pidpath);

        /* Desliga o servidor */
        if (buffer->type == TYPE_SHUTDOWN)
            break;
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
 * A função messageWrite escreve uma mensagem num fifo escrevendo nele a informação necessária à mensagem.
 * 
 * @param msg A mensagem que se pretende enviar.
 * @param fifo O fifo para o qual se pretende escrever a mensagem.
 * 
 * @return O resultado da operação de escrita (sucesso ou erro).
 * 
 * @author Guilherme Oliveira
 * @date 17/04/2023
*/
int messageWrite(Message msg, int fifo)
{
    /* Escrita da mensagem no fifo */
    ssize_t bytes_written = write(fifo, msg, sizeof(NPMessage));

    /* Verificação da escrita no fifo */
    if (bytes_written < 0)
        return -1;

    /* Termina em sucesso */
    return 0;
}

/**
 * A função messageSend envia uma mensagem para um fifo, criando a variável mensagem necessário
 * ao envio e posteriormente escrevendo-a no fifo.
 * 
 * @param pid O PID da mensagem a enviar.
 * @param type O tipo de mensagem a enviar.
 * @param msg O conteúdo da mensagem a enviar.
 * @param time O timestamp associado ao conteúdo da mensagem a enviar.
 * @param fifo O fifo para onde enviar a mensagem.
 * 
 * @return O resultado da operação de envio (sucesso ou erro).
 * 
 * @author Guilherme Oliveira
 * @date 23/04/2023
*/
int messageSend(pid_t pid, char type, char content[], long time, int fifo)
{
    /* Cria a mensagem que pretende enviar ao servidor */
    Message msg = createMessage(pid, type, content, time);

    /* Verifica o sucesso da criação da mensagem a enviar ao servidor */
    if (msg == NULL)
        return -1;

    /* Envio da mensagem para o servidor */
    int res = messageWrite(msg, fifo);

    /* Destroí a mensagem enviada */
    destroyMessage(msg);

    /* Verificação do sucesso do envio */
    if (res < 0)
        return -1;

    /* Termina o processo de notificação em sucesso */
    return 0;
}

/**
 * A função messageRequest executa para o servidor um pedido de status/stats de processos e prepara
 * um processo que irá ouvir e imprimir os resultados vindos do servidor.
 * 
 * @param type O tipo de informação pretendida do servidor.
 * @param argv Os argumentos recebidos pelo tracer (para saber quais são os filtros).
 * @param argc O número de argumentos recebidos.
 * @param printer A função que irá imprimir a informação recebida do servidor.
 * 
 * @return O resultado da operação (sucesso ou erro).
 * 
 * @author Guilherme Oliveira
 * @date 25/04/2023
*/
int messageResquest(char type, char * argv[], int argc, void (*printer)(Message))
{    
    /* Descobre o pid do processo em execução neste momento */
    pid_t pid = getpid();

    /* Cria o caminho para o fifo que irá criar para comunicar com o servidor */
    char path[PATH_SIZE];
    snprintf(path, PATH_SIZE, "tmp/%d", pid);

    /* Cira o fifo de comunicação com o servidor */
    int create = mkfifo(path, 0666);

    /* Verificação se a criação foi bem sucedida */
    if (create < 0)
        return -1;

    /* Abertura do fifo para escrita */
    int sender = open(FIFO_PATH, O_WRONLY);

    /* Verificação de abertura do fifo */
    if (sender < 0)
        return -1;

    /* Cria um array dinâmico auxiliar */
    char ** args = createDArray();

    /* Converte os argumentos num array dinâmico */
    for (int i = 2; i < argc && type != TYPE_STATUSREQUEST && args; i++)
        args = insertDArray(args, argv[i]);

    /* Verifica se a converção dos argumentos foi bem sucedida */
    if (args == NULL)
        return -1;

    /* Variável que armazena a mensagem a enviar */
    char msg[MSG_SIZE] = { 0 };

    /* Cria a mensagem a enviar */
    darrayToString(args, msg, " ", MSG_SIZE);

    /* Liberta o array dinâmico */
    destroyDArray(args);

    /* Envia mensagem de pedido para o servidor */
    int send = messageSend(pid, type, msg, getTimeMilliseconds(), sender);

    /* Fecha o fifo de escrita */
    close(sender);

    /* Verificação do envio da mensagem */
    if (send < 0)
        return -1;

    /* Cria o descritor de leitura do fifo */
    int listener = open(path, O_RDONLY);

    /* Verificação da abertura do descritor */
    if (listener < 0)
        return -1;

    /* Leitura das mensagens recebidas do servidor */
    int res = messageListen(listener, printer, NULL);

    /* Fecha o descritor e o fifo */
    close(listener);
    unlink(path);

    /* Verificação do resultado da leitura das mensagens do servidor */
    if (res < 0)
        return -1;

    /* Termino do processo em sucesso */
    return 0;
}

/**
 * A função printDebugMessage imprime uma mensagem para o stdout em formato de debug.
 * 
 * @param msg A mensagem a se imprimir.
 * 
 * @author Guilherme Oliveira
 * @date 25/04/2023
*/
void printDebugMessage(Message msg)
{
    /* Imprime o output que se pretende */
    printf("Message received from PID %d of type %c generated at %ld ms with the content %s\n",
            msg->pid,
            msg->type,
            msg->time,
            msg->msg);
}

/**
 * A função printStatusMessage imprime uma mensagem para o stdout em formato de status.
 * 
 * @param msg A mensagem a se imprimir.
 * 
 * @author Guilherme Oliveira
 * @date 25/04/2023
*/
void printStatusMessage(Message msg)
{
    /* Imprime o output que se pretende */
    printf("%d %s %ld ms\n", msg->pid, msg->msg, msg->time);
}

/**
 * A função printStatsTimeMessage imprime uma mensagem para o stdout em formato de stats-time.
 * 
 * @param msg A mensagem a se imprimir.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
void printStatsTimeMessage(Message msg)
{
    /* Imprime o output que se pretende */
    printf("Total execution time is %ld ms\n", msg->time);
}

/**
 * A função printStatsCmdMessage imprime uma mensagem para o stdout em formato de stats-command.
 * 
 * @param msg A mensagem a se imprimir.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
void printStatsCmdMessage(Message msg)
{
    /* Imprime o output que se pretende */
    printf("%s was executed %ld times\n", msg->msg, msg->time);
}

/**
 * A função printStatsUniqMessage imprime uma mensagem para o stdout em formato de stats-uniq.
 * 
 * @param msg A mensagem a se imprimir.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
void printStatsUniqMessage(Message msg)
{
    /* Imprime o output que se pretende */
    printf("%s\n", msg->msg);
}

/**
 * A função shutdownRequest envia um pedido para desligar o servidor.
 * 
 * @return O resultado da operação (sucesso ou erro).
 * 
 * @author Guilherme Oliveira
 * @date 13/05/2023
*/
int shutdownRequest()
{
    /* Abertura do fifo para escrita */
    int sender = open(FIFO_PATH, O_WRONLY);

    /* Verificação de abertura do fifo */
    if (sender < 0)
        return -1;

    /* Envia mensagem de pedido para o servidor */
    int send = messageSend(getpid(), TYPE_SHUTDOWN, MSG_CONTENT_EMPTY, getTimeMilliseconds(), sender);

    /* Fecha o fifo de escrita */
    close(sender);

    /* Verificação do envio da mensagem */
    if (send < 0)
        return -1;

    /* Devolve um sucesso */
    return 0;
}