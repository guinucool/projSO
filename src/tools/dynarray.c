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
    arr = reallocDArray(arr, size + 1);

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