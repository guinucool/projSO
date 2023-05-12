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
 * @struct __PROCESS__
 * @brief Estrutura que define um processo.
 * 
 * Esta estrutura define a informação necessária ao armazenamento da informação
 * acerca de um processo que se encontra em execução.
*/
typedef struct __PROCESS__ {
    pid_t pid;
    char exec[EXEC_MAX];
    long start;
    char finished;
}*Process, NPProcess;

/**
 * A função createProcess cria e aloca o espaço necessário a uma variável do tipo Process.
 * 
 * @param pid O pid associado ao processo que a variável descreve.
 * @param exec O que o processo está a executar.
 * @param start O timestamp de início do processo.
 * 
 * @return O processo criado e alocado.
 * 
 * @author Guilherme Oliveira
 * @date 24/04/2023
*/
Process createProcess(pid_t pid, char exec[], long start)
{
    /* Criação e alocação do espaço necessário a uma variável de processo */
    Process new = malloc(sizeof(NPProcess));

    /* Verifica se a alocação de espaço foi bem sucedida */
    if (new)
    {
        /* Definição das propriedades de um processo */
        new->pid = pid;
        new->start = start;
        new->finished = 0;

        /* Cópia dos nomes dos programas em execução no processo */
        strncpy(new->exec, exec, EXEC_MAX);
    }

    /* Devolve a nova variável de processo criada */
    return new;
}

/**
 * A função destroyProcess destroí uma variável do tipo Process, libertando o espaço usado por esta.
 * 
 * @param process A variável a ser destruída.
 * 
 * @author Guilherme Oliveira
 * @date 24/04/2023
*/
void destroyProcess(Process process)
{
    /* Verifica se a variável que deve ser libertada existe */
    if (process)
        free(process);
}

/**
 * A função enqueueProcess adiciona um processo à fila de processos.
 * 
 * @param process O processo a ser colocado na fila.
 * @param queue A fila onde o processo deve ser inserido.
 * 
 * @return O resultado da operação (sucesso ou erro).
 * 
 * @author Guilherme Oliveira
 * @date 24/04/2023
*/
int enqueueProcess(Process process, int queue)
{
    /* Escreve o processo na fila de processos */
    ssize_t bytes_written = write(queue, process, sizeof(NPProcess));

    /* Verificação da escrita do processo na fila */
    if (bytes_written < 0)
        return -1;

    /* Termina o processo em sucesso */
    return 0;
}

/**
 * A função dequeueProcess substitui um processo ativo por um processo terminado na fila.
 * 
 * @param process O processo a ser substituido.
 * @param queue A fila onde vai ser substituido.
 * 
 * @return O resultado da operação (sucesso ou insucesso).
 * 
 * @author Guilherme Oliveira
 * @date 25/04/2023
*/
int dequeueProcess(Process process, int queue)
{
    /* Termina o processo em questão */
    process->finished = 1;

    /* Coloca a posição de escrita de volta no início da antiga posição do processo em execução */
    int offset = lseek(queue, -sizeof(NPProcess), SEEK_CUR);

    /* Verifica a colocação */
    if (offset < 0)
        return -1;

    /* Coloca o processo terminado "de volta" na fila */
    int res = enqueueProcess(process, queue);

    /* Verifica a colocação do processo */
    if (res < 0)
        return -1;

    /* Termina o processo em sucesso */
    return 0;
}

/**
 * A função addProcessQueue cria e adiciona um processo ativo ao final fila de espera.
 * 
 * @param pid O pid do processo a ser adicionado.
 * @param exec O programa executado no processo.
 * @param start O timestamp de início do processo.
 * 
 * @author Guilherme Oliveira
 * @date 25/04/2023
*/
void addProcessQueue(pid_t pid, char exec[], long start)
{
    /* Cria o processo-filho que irá adicionar um processo à fila */
    if (fork() == 0)
    {
        /* Cria o processo que vai adicionar à fila */
        Process process = createProcess(pid, exec, start);

        /* Verifica se a criação de processo foi bem sucedida */
        if (process == NULL)
            errorChildHandler(-1, SERVER_NAME);

        /* Abre o descritor de escrita da fila de processos em execução */
        int queue = open(QUEUE_PATH, O_APPEND | O_CREAT | O_WRONLY, 0640);

        /* Coloca o processo na fila */
        int res = enqueueProcess(process, queue);

        /* Destroí a variável auxiliar e fecha o descritor */
        destroyProcess(process);
        close(queue);

        /* Verifica a colocação */
        errorChildHandler(res, SERVER_NAME);

        /* Termina o filho em sucesso */
        _exit(0);
    }
}

/**
 * A função removeProcessQueue desativa um processo que se encontra presente na fila de processos e armazena-o.
 * 
 * @param pid O pid do processo a remover.
 * @param pidpath O caminho para a pasta de pids onde o registo do processo vai ser armazenado.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
void removeProcessQueue(pid_t pid, long end, char * pidpath, char type)
{
    /* Cria um processo-filho que irá desativar o processo na fila */
    if (fork() == 0)
    {
        /* Abertura e verificação da abertura do descritor da fila de espera */
        int queue = open(QUEUE_PATH, O_CREAT | O_RDWR, 0640);
        errorChildHandler(queue, SERVER_NAME);

        /* Cria um buffer de processo e uma variável de número de leitura de bytes auxiliar */
        Process process = malloc(sizeof(NPProcess));
        ssize_t bytes_read;

        /* Verificação do número de bytes lidos */
        if (process == NULL)
            errorChildHandler(-1, SERVER_NAME);

        /* Lê a fila de espera até encontrar a posição em que o processo pretendido está */
        while ((bytes_read = read(queue, process, sizeof(NPProcess))) > 0 && process->pid != pid);
        
        /* Caso o processo tenha sido encontrado */
        if (bytes_read > 0)
        {
            /* Desativa o processo e verifica o sucesso nessa operação */
            errorChildHandler(dequeueProcess(process, queue), SERVER_NAME);

            /* Regista o processo e verifica o sucesso nessa operação */
            if (type == TYPE_PROCESS_END)
                errorChildHandler(registerProcess(process->pid, process->exec, end - process->start, pidpath), SERVER_NAME);
        }

        /* Destroí o processo e fecha a fila */
        destroyProcess(process);
        close(queue);

        /* Verifica se houve algum erro na leitura */
        errorChildHandler(bytes_read, SERVER_NAME);

        /* Termina o processo em sucesso */
        _exit(0);
    }
}

/**
 * A função mapProcessQueue mapeia a fila de processo enviando os vários processos ativos de volta para o cliente.
 * 
 * @param pid O pid do processo com o qual se pretende comunicar.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
void mapProcessQueue(pid_t pid)
{
    /* Cria o processo-filho que irá mapear a fila e proceder à alterações pretendida */
    if (fork() == 0)
    {
        /* Variável que irá armazenar o path do fifo de comunicação com o cliente */
        char path[PATH_SIZE];
        snprintf(path, PATH_SIZE, "tmp/%d", pid);

        /* Abre e verifica o descritor da fila */
        int queue = open(QUEUE_PATH, O_CREAT | O_RDONLY, 0640);
        errorChildHandler(queue, SERVER_NAME);

        /* Abertura e verificação da abertura do fifo de escrita */
        int statusfifo = open(path, O_WRONLY);
        errorChildHandler(statusfifo, SERVER_NAME);

        /* Variáveis auxiliar que armazenam o bytes lidos */
        Process process = malloc(sizeof(NPProcess));
        ssize_t bytes_read;

        /* Verifica se a variável de processo foi criado com sucesso */
        if (process == NULL)
            errorChildHandler(-1, SERVER_NAME);

        /* Começa a fazer o mapeamento da fila */
        while ((bytes_read = read(queue, process, sizeof(NPProcess))) > 0)
        {
            /* Confirma se o processo encontrado está em execução */
            if (!(process->finished))
            {
                /* Calcula a duração do processo até ao momento */
                long duration = getTimeMilliseconds() - process->start;

                /* Envia e verifica o envio da mensagem de volta ao cliente */
                int res = messageSend(process->pid, TYPE_REPLY, process->exec, duration, statusfifo);
                errorChildHandler(res, SERVER_NAME);
            }
        }

        /* Fecha o descritor do fifo de status */
        close(statusfifo);

        /* Fecha o descritor da fila e apaga a variável auxiliar */
        destroyProcess(process);
        close(queue);

        /* Verifica se o processo de leitura não falhou */
        errorChildHandler(bytes_read, SERVER_NAME);
        
        /* Termina o processo em sucesso */
        _exit(0);
    }
}

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
    /* Abertura do fifo para escrita */
    int sender = open(FIFO_PATH, O_WRONLY);

    /* Verificação de abertura do fifo */
    if (sender < 0)
        return -1;

    /* Variável que irá armazenar os argumentos do programa */
    char ** exec = stringToDArray(cmd, " ");

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

        /* Notificação de um processo ao servidor e verificação do sucesso da notificação, tal como fecho do descritor de leitura (dentro do filho) */
        int notify = messageSend(pid, TYPE_PROCESS_START, exec[0], start, sender);
        close(sender);
        errorChildHandler(notify, CLIENT_NAME);

        /* Imprime notificação do programa a correr */
        printf("Running PID %d\n", pid);

        /* Executa e verifica a execução do programa */
        int res = execvp(exec[0], exec);
        destroyDArray(exec);
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

    /* Imprime o tempo que o programa demorou */
    printf("Ended in %ld ms\n", (end - start));

    /* Define o tipo de mensagem a ser enviada para o servidor */
    char type = TYPE_PROCESS_END;

    /* Verifica se o filho terminou em sucesso */
    if (!WIFEXITED(status) || WEXITSTATUS(status))
        type = TYPE_PROCESS_FAIL;

    /* Notificação do final do processo ao servidor */
    int notify = messageSend(cpid, type, exec[0], end, sender);

    /* Fecha o fifo de escrita */
    close(sender);

    /* Destroí o array auxiliar de argumentos */
    destroyDArray(exec);
    
    /* Verificação do sucesso da notificação */
    if (notify < 0)
        return -1;

    /* Verifica o estado de conclusão do processo */
    if (WIFEXITED(status)) {

        /* Verifica se algum processo dentro do filho falhou*/
        if(WEXITSTATUS(status))
            exit(1);
    }
    else
        return -1;

    /* Termina o processo em sucesso */
    return 0;
}

/**
 * A função executePipeProcess executa um processo de um pipe de forma a usar o descritores necessários
 * para o processo seguinte.
 * 
 * @param exe O processo que se pretende executar nesta fase.
 * @param write O descritor onde vai ser escrito para o próximo processo (NULL no último processo).
 * @param read O descritor onde vai ser lido o resultado do processo anterior (NULL no primeiro processo).
 * 
 * @author Guilherme Oliveira
 * @date 12/05/2023
*/
void executePipeProcess(char ** exe, int * write, int * read)
{
    /* Cria o filho que irá executar o processo */
    if (fork() == 0)
    {
        /* Altera o stdout para o escritor (no caso de este ser necessário para um próximo processo) */
        if (write)
        {
            close(write[0]);
            dup2(write[1], 1);
            close(write[1]);
        }

        /* Altera o stdin para o leitor (no caso de este ser necessário de um processo anterior) */
        if (read)
        {
            dup2(read[0], 0);
            close(read[0]);
        }
        
        /* Execute e verifica a execução do programa */
        int res = execvp(exe[0], exe);
        destroyDArray(exe);
        errorChildHandler(res, CLIENT_NAME);

        /* Termina o processo em sucesso */
        _exit(0);
    }   
}

/**
 * @brief 
 * 
 * @param cmd 
 * @return int 
 */
int executePipeline(char ** cmd[], int N)
{
    /* Variável que irá armazenar os vários pipes */
    int pi[N-1][2];

    /* Execução da pipeline */
    for (int i = 0; i < N; i++)
    {
        /* Criação do pipes de comunicação entre execs */
        if (i < N - 1) {

            /* Verifica se a criação do pipe foi bem sucedida */
            if (pipe(pi[i]) < 0)
                return -1;
        }

        /* Execução do primeiro processo */
        if (i == 0)
            executePipeProcess(cmd[i], pi[i], NULL);

        /* Execução do último processo */
        else if (i == N - 1)
            executePipeProcess(cmd[i], NULL, pi[i-1]);

        /* Execução de qualquer outro processo */
        else
            executePipeProcess(cmd[i], pi[i], pi[i-1]);

        /* Fecha a escrita de um processo já terminado */
        if (i < N - 1) close(pi[i][1]);
        
        /* Fecha a leitura de um processo já lido */
        if (i > 0) close(pi[i-1][0]);
    }

    /* Verifica se a pipe falha */
    int fail = 0;

    /* Status dos processos terminado */
    int status;
    
    /* Espera pelo termino dos vários pipes */
    for (int i = 0; i < N; i++)
    {   
        /* Espera pelos vários processos */
        if (fail == 0)
            wait(&status);
        else
            wait(NULL);

        /* Verifica se o filho terminou em sucesso */
        if (!WIFEXITED(status) || WEXITSTATUS(status))
            fail = -1;
    }

    /* Devolve o resultado do pipeline */
    return fail;
}

int executePipe(char cmd[])
{
    /* Abertura do fifo para escrita */
    int sender = open(FIFO_PATH, O_WRONLY);

    /* Verificação de abertura do fifo */
    if (sender < 0)
        return -1;

    /* Descobre quantos execs vai precisar */
    int N = countStrOccr(cmd, '|') + 1;

    /* Variável que irá armazenar os argumentos do programa */
    char ** exec[N];

    /* Cria o array múltiplo e verifica a sua criação */
    if (stringToMultipleDArray(exec, N, cmd, '|', ' ') < 0)
        return -1;

    /* Descobre o timestamp de início do processo */
    long start = getTimeMilliseconds();

    /* Cria o processo-filho que executa o programa */
    if (fork() == 0)
    {
        /* Descobre o pid do processo-filho */
        pid_t pid = getpid();

        /* Cria array de mensagem */
        char msg[MSG_SIZE];

        /* Converte o array múltiplo em string */
        

        /* Notificação de um processo ao servidor e verificação do sucesso da notificação, tal como fecho do descritor de leitura (dentro do filho) */
        int notify = messageSend(pid, TYPE_PROCESS_START, exec[0], start, sender);
        close(sender);
        errorChildHandler(notify, CLIENT_NAME);

        /* Imprime notificação do programa a correr */
        printf("Running PID %d\n", pid);

        /* Executa e verifica a execução do programa */
        int res = execvp(exec[0], exec);
        destroyDArray(exec);
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

    /* Imprime o tempo que o programa demorou */
    printf("Ended in %ld ms\n", (end - start));

    /* Define o tipo de mensagem a ser enviada para o servidor */
    char type = TYPE_PROCESS_END;

    /* Verifica se o filho terminou em sucesso */
    if (!WIFEXITED(status) || WEXITSTATUS(status))
        type = TYPE_PROCESS_FAIL;

    /* Notificação do final do processo ao servidor */
    int notify = messageSend(cpid, type, exec[0], end, sender);

    /* Fecha o fifo de escrita */
    close(sender);

    /* Destroí o array auxiliar de argumentos */
    destroyDArray(exec);
    
    /* Verificação do sucesso da notificação */
    if (notify < 0)
        return -1;

    /* Verifica o estado de conclusão do processo */
    if (WIFEXITED(status)) {

        /* Verifica se algum processo dentro do filho falhou*/
        if(WEXITSTATUS(status))
            exit(1);
    }
    else
        return -1;

    /* Termina o processo em sucesso */
    return 0;
}