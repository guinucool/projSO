#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../../includes/tools/dynarray.h"
#include "../../includes/tools/register.h"

/**
 * @struct __REGISTER__
 * @brief Estrutura que define uma variável de registo.
 * 
 * Esta estrutura define um registo de um processo que já tenha terminado, podendo armazenar nela os conteúdos necessários
 * a esse armazenamento.
*/
typedef struct __REGISTER__ {
    char exec[RES_MAX];
    long time;
}*Register, NPRegister;

/**
 * A função createRegister cria e aloca o espaço necessário a uma variável do tipo Register.
 * 
 * @param exec O conteúdo do registo.
 * @param time O tempo de execução do registo.
 * 
 * @return O registo criado.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023 
*/
Register createRegister(char exec[], long time)
{
    /* Cria e aloca o espaço necessário a uma variável do tipo Register */
    Register new = malloc(sizeof(NPRegister));

    /* Verifica se a variável foi efetivamente criada */
    if (new)
    {
        /* Define as propriedades da variável */
        strncpy(new->exec, exec, RES_MAX);
        new->time = time;
    }

    /* Devolve a nova variável */
    return new;
}

/**
 * A função destroyRegister destroí um registo que exista.
 * 
 * @param reg O registo a ser destruído.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
void destroyRegister(Register reg)
{
    /* Destroí uma variável de registo, caso ela exista */
    if (reg)
        free(reg);
}

/**
 * A função getRegisterPath descobre o caminho para um registo de um processo.
 * 
 * @param pid O pid do processo do qual se pretende o registo.
 * @param path O caminho para a pasta de pids.
 * 
 * @return O caminho para o registo do processo desejado.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
char * getRegisterPath(pid_t pid, char path[])
{
    /* Descobre o tamanho do caminho para a pasta dos PIDs */
    int len = strlen(path) + 16;

    /* Cria o string que irá armazenar o caminho do registo */
    char * filepath = malloc(sizeof(char) * len);

    /* Caso haja criação do string, coloca no mesmo o caminho para o registo */
    if (filepath)
        snprintf(filepath, len, "%s/%d", path, pid);

    /* Retorna o path criado */
    return filepath;
}

/**
 * A função writeRegister escreve um registo de um processo no sistema.
 * 
 * @param pid O pid do processo a ser registado.
 * @param reg O registo do processo a ser escrito.
 * @param path O caminho para a pasta de pids.
 * 
 * @return O resultado a operação (sucesso ou erro).
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
int writeRegister(pid_t pid, Register reg, char path[])
{
    /* Cria o path para o novo registo */
    char * filepath = getRegisterPath(pid, path);

    /* Verifica o sucesso na criação do filepath */
    if (filepath == NULL)
        return -1;

    /* Abre o descritor de escrita no novo ficheiro */
    int fd = open(filepath, O_CREAT | O_TRUNC | O_WRONLY, 0640);

    /* Liberta o string auxiliar de path */
    free(filepath);

    /* Verifica a abertura do descritor */
    if (fd < 0)
        return -1;

    /* Escreve o registo no ficheiro */
    size_t bytes_written = write(fd, reg, sizeof(NPRegister));

    /* Verifica se a escrita foi bem sucedida */
    if (bytes_written <= 0)
        return -1;

    /* Termina o processo em sucesso */
    return 0;
}

/**
 * A função readRegister lê um registo de um processo guardado no sistema.
 * 
 * @param pid O pid do processo do qual se pretende o registo.
 * @param path O caminho para a pasta de pids.
 * 
 * @return O registo lido do sistema.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
Register readRegister(pid_t pid, char path[])
{
    /* Cria o path para o novo registo */
    char * filepath = getRegisterPath(pid, path);

    /* Verifica o sucesso na criação do filepath */
    if (filepath == NULL)
        return NULL;

    /* Abre o descritor de leitura no novo ficheiro */
    int fd = open(filepath, O_CREAT | O_RDONLY, 0640);

    /* Liberta o string auxiliar de path */
    free(filepath);

    /* Verifica a abertura do descritor */
    if (fd < 0)
        return NULL;

    /* Cria uma variável de registo auxiliar para registar o lido */
    Register reg = malloc(sizeof(NPRegister));

    /* Verifica se o processo de criação foi bem sucedido */
    if (reg)
    {
        /* Lê o registo do ficheiro */
        size_t bytes_read = read(fd, reg, sizeof(NPRegister));

        /* Fecha o descritor de leitura do ficheiro */
        close(fd);

        /* Verifica o sucesso na leitura do ficheiro */
        if (bytes_read <= 0) {

            /* Liberta a variável criada */
            destroyRegister(reg);

            /* Devolve um erro */
            return NULL;
        }
    }

    /* Fecha o descritor de leitura do ficheiro em caso de erro */
    else
        close(fd);

    /* Devolve o registo criado */
    return reg;
}

/**
 * A função registerProcess regista um processo no sistema.
 * 
 * @param pid O pid do processo a registar.
 * @param exec O executado pelo processo a registar.
 * @param time O tempo de execução a registar.
 * @param path O caminho da pasta de pids.
 *  
 * @return O resultado do processo (sucesso ou erro).
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
int registerProcess(pid_t pid, char exec[], long time, char path[])
{
    /* Cria o registo a escrever */
    Register reg = createRegister(exec, time);

    /* Verifica a criação do registo */
    if (reg == NULL)
        return -1;

    /* Escreve o ficheiro no sistema */
    int res = writeRegister(pid, reg, path);

    /* Liberta o registo auxiliar criado */
    destroyRegister(reg);

    /* Verifica a escrita do registo */
    if (res < 0)
        return res;

    /* Termina o processo em sucesso */
    return 0;
}

/**
 * A função getRegisterTime devolve o tempo de execução associado a um processo num registo.
 * 
 * @param reg O registo de onde se pretende obter o tempo de execução.
 * 
 * @return O tempo de execução do registo.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023 
*/
long getRegisterTime(Register reg)
{
    /* Devolve o tempo de execução de um registo */
    return reg->time;
}

/**
 * A função countExecInRegister conta o número de vezes que um processo aparece num registo.
 * 
 * @param reg O registo a ser considerado na contagem.
 * @param exec O processo a ser contado.
 * 
 * @return O número de vezes que o processo aparece.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023 
*/
int countExecInRegister(Register reg, char exec[])
{
    /* Variável que armazena a quantidade de vezes que aparece o processo */
    int count = 0;

    /* Percorre a lista de processos no registo (no caso de ser um pipeline) */
    for (char * token = strtok(reg->exec, " | "); token; token = strtok(NULL, " | "))
        if (strcmp(token, exec) == 0)
            count++;

    /* Devolve a quantidade de vezes que o processo aparece no registo */
    return count;
}

/**
 * A função addExecRegisterUnique adiciona todos os processos de um registo que ainda não estejam no array dinâmico.
 * 
 * @param reg O registo de onde devem ser adicionados os processos.
 * @param arr O array dinâmico onde vão ser guardados os processos únicos.
 * 
 * @return O array dinâmico atualizado com os novos processos.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023 
*/
char ** addExecRegisterUnique(Register reg, char ** arr)
{
    /* Percorre a lista de processos no registo (no caso de ser um pipeline) e adiciona os inexistentes ao array */
    for (char * token = strtok(reg->exec, " | "); token && arr; token = strtok(NULL, " | "))
        if (checkIfInDArray(arr, token))
            arr = insertDArray(arr, token);

    /* Devolve o array atualizado de acordo com as regras */
    return arr;
}