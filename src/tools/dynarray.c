#include <stdlib.h>
#include <string.h>
#include "../../includes/tools/dynarray.h"

/**
 * A função createDArray cria um array dinâmico.
 * 
 * @return O array dinâmico criado.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
char ** createDArray()
{
    /* Cria o array e armazena o espaço necessário ao mesmo */
    char ** new = malloc(sizeof(char*));

    /* Caso o array tenha sido criado com sucesso, define a primeira posição como o fim do array */
    if (new)
        new[0] = NULL;

    /* Devolve o array criado */
    return new;
}

/**
 * A função destroyDArray destroí um array dinâmico e os seus elementos.
 * 
 * @param arr O array dinâmico a ser destruído.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
void destroyDArray(char ** arr)
{
    /* Verifica se o array que se pretende destruir existe */
    if (arr)
    {
        /* Liberta o vários elementos do array, caso existam */
        for (int i = 0; arr[i]; i++)
            free(arr[i]);

        /* Liberta o array em si */
        free(arr);
    }
}

/**
 * A função getArraySize devolve o tamanho de um array dinâmico.
 * 
 * @param arr O array dinâmico do qual se pretende saber o tamanho.
 * 
 * @return O tamanho do array dinâmico.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023 
*/
int getArraySize(char ** arr)
{
    /* Variável que irá armazenar o tamanho do array */
    int size = 0;

    /* Percorre o array até encontrar a posição final */
    for (int i = 0; arr[i]; i++)
        size++;

    /* Devolve o tamanho do array */
    return size;
}

/**
 * A função reallocDArray realoca um array dinâmico.
 * 
 * @param arr O array dinâmico a ser realocado.
 * @param size O tamanho para o qual o array dinâmico vai ser realocado.
 * 
 * @return O array dinâmico realocado.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
char ** reallocDArray(char ** arr, int size)
{
    /* Cria uma localização temporária para guardar o realloc */
    char ** tmp = realloc(arr, sizeof(char*) * size);

    /* Verifica se o realloc foi bem sucedido */
    if (tmp == NULL)
        destroyDArray(arr);

    /* Devolve o realloc */
    return tmp;
}

/**
 * A função insertDArray insere um elemento no array dinâmico.
 * 
 * @param arr O array dinâmico onde vai ser inserido o elemento.
 * @param str O string que vai ser inserido no array.
 * 
 * @return O array dinâmico após a inserção.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
char ** insertDArray(char ** arr, char * str)
{
    /* Descobre o tamanho do array e do string */
    int size = getArraySize(arr);
    int len = (strlen(str) + 1);

    /* Reserva a última posição do array */
    arr[size] = malloc(sizeof(char) * len);

    /* Verifica se a reserva de espaço foi bem sucedida */
    if (arr[size] == NULL) {

        /* Destroí o array existente até agora */
        destroyDArray(arr);

        /* Devolve em erro */
        return NULL;
    }

    /* Atualiza a última posição do array */
    strncpy(arr[size], str, len);

    /* Aumenta o array */
    arr = reallocDArray(arr, size + 2);

    /* Atualiza a última posição do array para o limitador caso a realocação tenha sido um sucesso */
    if (arr)
        arr[size+1] = NULL;

    /* Devolve o array aumentado */
    return arr;
}

/**
 * A função stringToDArray converte um string num array dinâmico separando o string em vários elementos
 * do array através de um delimitador.
 * 
 * @param str O string a ser convertido.
 * @param delim O delimitador que separa o string.
 * 
 * @return O array dinâmico criado.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
char ** stringToDArray(char * str, char * delim)
{
    /* Cria o array dinâmico */
    char ** new = createDArray();

    /* Converte o string através do delimitador */
    for (char * token = strtok(str, delim); token && new; token = strtok(NULL, delim))
        new = insertDArray(new, token);
    
    /* Devolve o array criado */
    return new;
}

/**
 * A função darrayToString converte um array dinâmico para um string.
 * 
 * @param arr O array dinâmico a ser convertido.
 * @param dest O string de destino da conversão.
 * @param delim O delimitador dentro do string dos vários elementos.
 * @param size O tamanho disponível no string.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023
*/
void darrayToString(char ** arr, char * dest, char * delim, int size)
{
    /* Reserva o espaço final para o caracter nulo */
    size--;

    /* Percorre o array adicionando os seus elementos ao string separados por um delimitador */
    for (int i = 0; arr[i] && size > 0; i++)
    {
        /* Adiciona o elemento ao string e remove o espaço disponível de acordo */
        strncat(dest, arr[i], size);
        size -= strlen(arr[i]);

        /* Adiciona o delimitador ao string e remove o espaço disponível de acordo */
        strncat(dest, delim, size);
        size -= strlen(delim);
    }
}

/**
 * A função checkIfInDArray verifica se um elemento está num array dinâmico.
 * 
 * @param arr O array dinâmico onde a verificação vai ser feita.
 * @param str O elemento a ser verificado dentro do array.
 * 
 * @return A conclusão a que chega o processo.
 * 
 * @author Guilherme Oliveira
 * @date 26/04/2023 
*/
int checkIfInDArray(char ** arr, char * str)
{
    /* Variável que irá armazenar o resultado da verificação */
    int res = 1;

    /* Percorre o array à procura do string e para caso encontre */
    for (int i = 0; arr[i] && res; i++)
        res = strcmp(str, arr[i]);
    
    /* Devolve o resultado a que chegou */
    return !res;
}

/**
 * A função stringToMultipleDArray converte um string num DArray múltiplo.
 * 
 * @param arr O array para onde vão ser armazenados os vários DArrays.
 * @param N O número de elementos que o array possui disponíveis.
 * @param str O string a ser convertido.
 * @param delim1 O delimitador para o array múltiplo.
 * @param delim2 O delimitador para o DArray.
 * 
 * @return O resultado da operação (sucesso ou erro).
 * 
 * @author Guilherme Oliveira
 * @date 12/05/2023
*/
int stringToMultipleDArray(char ** arr[], int N, char * str, char delim1, char delim2)
{
    /* Variável auxiliar de contagem do array */
    int i = 0;

    /* Converte o string através do delimitador */
    for (char * token = strtok(str, delim1); token && i < N; token = strtok(NULL, delim1))
    {

        /* Constroi um DArray em cada posição do array múltiplo */
        arr[i] = stringToDArray(token, delim2);

        /* Verifica o sucesso da construção */
        if (arr[i] == NULL)
            return 1;

        /* Passa à próxima posição */
        i++;
    }
    
    /* Devolve um sucesso */
    return 0;
}

/**
 * A função destroyMultipleDArray destroí um DArray múltiplo. 
 * 
 * @param arr O array múltiplo a ser destruído.
 * @param N O tamanho do array múltiplo que vai ser destruído.
 * 
 * @author Guilherme Oliveira
 * @date 12/05/2023
*/
void destroyMultipleDArray(char ** arr[], int N)
{
    for (int i = 0; i < N; i++)
        destroyDArray(arr[i]);
}

/**
 * A função multipleToString converte um array múltiplo num string.
 * 
 * @param arr O array a ser convertido.
 * @param N O tamanho do array a ser convertido.
 * @param dest O string de destino.
 * @param delim O delimitador do string de destino.
 * @param size O tamanho restante para o string.
 * 
 * @author Guilherme Oliveira
 * @date 12/05/2023
*/
void multipleToString(char ** arr[], int N, char * dest, char * delim, int size)
{
    /* Reserva espaço para o caracter nulo */
    size--;

    /* Percorre o array adicionando os seus elementos ao string separados por um delimitador */
    for (int i = 0; i < N && size > 0; i++)
    {
        /* Adiciona o elemento ao string e remove o espaço disponível de acordo */
        strncat(dest, arr[i][0], size);
        size -= strlen(arr[i][0]);

        /* Adiciona o delimitador ao string e remove o espaço disponível de acordo */
        strncat(dest, delim, size);
        size -= strlen(delim);
    }
}