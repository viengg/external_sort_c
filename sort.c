// =----------------------------------------------------------=
// =                           AVISO:                         =
// =                   MODIFIQUE ESTE ARQUIVO!                =
// = Aqui, você deve implementar, obrigatoriamente, essas     =
// = duas funções:                                            =
// =     - `a_menor_que_b`: deve retornar quando uma string   =
// =        é menor que a outra (ambos com o mesmo tamanho);  =
// =     - `external_sort`: aqui, você deverá implementar o   =
// =        seu trabalho. Ordene os registros em `input_file` =
// =        e escreva-os em `output_file`, usando somente     =
// =        `memory` KB de memória.                           =
// =                                                          =
// = Instruções:                                              =
// =     - use a função `mathias_malloc` ao invés de          =
// =       `malloc`;                                          =
// =     - use a função `mathias_free` ao invés de `free`;    =
// =     - lembre-se que você tem somente `memory` KB de      =
// =       memória;                                           =
// =     - note que a sua implementação de `a_menor_que_b`    =
// =       afetará o resultado da verificação da resposta.    =
// =                                                          =
// = Dicas:                                                   =
// =     - você pode usar a sua função `a_menor_que_b` em     =
// =       sua implementação da função `external_sort`;       =
// =                                                          =
// =                      BOM TRABALHO!                       =
// =----------------------------------------------------------=

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "mem.h"

// Parâmetros:
//     - a: uma cadeia de caracteres de tamanho `len`;
//     - b: uma cadeia de caracteres de tamanho `len`;
//     - len: o tamanho (quantidade de bytes) de `a` e `b`.
// Essa função deve retornar 1, se `a` é menor que `b` (`a` deve vir antes de `b` no
// arquivo), e 0 caso contrário.
// Ela também é usada para verificação da ordenação! Então, se você implementá-la
// erroneamente, a verificação também estará errada!
int a_menor_que_b(char* a, char* b, int len)
{
    int i;
    for(i=0; i<len; i++)
    {
        if(a[i] > b[i])
            return 0;
        else if(a[i] < b[i])
            return 1;
    }
    return 1;
}

int compare(const void* p1, const void *p2)
{
     char *const *a = p1;
     char *const *b = p2;
    return strcmp(*a, *b);

}

void CriaFitas(FILE *fita[], int numCaminhos)
{
    int i;
    char c[30];
    for(i=0; i<(numCaminhos*2); i++)
    {
        sprintf(c,"%d",i);
        char nome[] = "fita";
        strcat(nome,c);
        strcat(nome,".txt");
        fita[i] = fopen(nome,"w+");
    }
}


void FechaFitas(FILE *fita[], int numCaminhos){
    int i;
    for(i=0; i<(numCaminhos*2); i++)
    {
        fclose(fita[i]);
    }
    free(fita);
}

void GeraPeso(char *registro, int numChars)
{
    char *peso = malloc((sizeof(char)*numChars)+1);
    int i;
    for(i=0; i<numChars; i++)
    {
        peso[i] = '~';
    }
    peso[i]='\0';

    strcpy(registro, peso);
    free(peso);
}

void CriaBlocosOrdenados(char **registros, unsigned int memDisponivel, int numChars, int numCaminhos, int registrosTotais, FILE *inputFile, FILE *fita[])
{
    int i=0, j=0;
    char *aux = malloc((sizeof(char)*numChars)+1);

    fgets(aux, numChars+2, inputFile); //Pula um \n
    while(fgets(registros[j], numChars+2, inputFile) != NULL)  //Enquanto não terminar de ler o arquivo original
    {
        if(i >= numCaminhos)   //Verifica se escreveu em todas as fitas disponíveis para a criação inicial dos blocos
        {
            i=0;               //Se já, então começa a escrever de volta na primeira fita
        }

        for(j=1; j<memDisponivel; j++)   //Le os n-1 registros restantes, onde n é a quantidade de registros que cabe em RAM
        {
            if(fgets(aux, numChars+2, inputFile) != NULL)
            {
                strcpy(registros[j], aux);
            }
            else
            {
                GeraPeso(registros[j], numChars);
            }
        }

        qsort(registros, memDisponivel, sizeof(*registros), compare); //Ordena os registros em RAM

        GeraPeso(aux, numChars);
        for(j=0; j<memDisponivel; j++)
        {
            if(strcmp(registros[j], aux) != 0)
            {
                fprintf(fita[i], "%s", registros[j]);   //Escreve os registros ordenados em uma fita
            }
        }

        i++;   //Avança para a próxima fita
        j=0;   //Rebobina o vetor
    }
}

int PosicaoMenorRegistro(char **registros, int numChars, int tamanho){
    int i, posicao = 0;
    char *menor = registros[0];

    for(i=1; i<tamanho;i++)
    {
        if(!a_menor_que_b(menor, registros[i], numChars))
        {
            menor = registros[i];
            posicao = i;
        }
    }

    return posicao;

}

int AlgumaFitaAtiva(int *atividade, int inicio, int fim)
{
    int i;
    for(i=inicio; i<fim; i++)
    {
        if(atividade[i])
        {
            return 1;
        }
    }
    return 0;
}

int Intercalacao(char **registros, unsigned int memDisponivel, int numCaminhos, int numChars, int registrosTotais, FILE *fita[])
{
    int i, j, fitaComMenor, inicioLeitura, fimLeitura, posEscrita, inicioEscrita, fimEscrita, tamanhoBloco, tamanhoVetor;
    int *atividade = malloc(sizeof(int)*numCaminhos*2);
    int *posicaoAtual = malloc(sizeof(int)*numCaminhos*2);
    int qtdRegistros;
    char *aux = malloc((sizeof(char)*numChars)+1);

    inicioLeitura = 0;
    fimLeitura = numCaminhos;
    posEscrita = numCaminhos;
    inicioEscrita = posEscrita;
    fimEscrita = numCaminhos*2;
    tamanhoBloco = memDisponivel;
    qtdRegistros = 0;
    tamanhoVetor = memDisponivel;

    for(i = inicioLeitura; i < fimLeitura; i++)
        {
            rewind(fita[i]);
        }

    for(i=0; i< memDisponivel; i++)
    {
        GeraPeso(registros[i], numChars);
    }

    while(qtdRegistros < registrosTotais)    //Enquanto não for criado um bloco do tamanho do arquivo original
    {
        qtdRegistros = 0;

        //Le o primeiro registro de cada fita

        j = inicioLeitura;
        for(i = inicioLeitura; i < fimLeitura; i++, j--)
        {
            if(fgets(registros[inicioLeitura-j], numChars+2, fita[i]) != NULL) //Se conseguir ler um registro de uma fita
            {
                posicaoAtual[i] = 1;              //Posicao atual da fita lida é setado como 1
                atividade[i] = 1;                 //Seta a fita atual como ativa;
            }
            else
            {
                //Gera um peso "infinito" para a posição da fita no registro e seta a fita como inativa
                GeraPeso(registros[inicioLeitura-j], numChars);
                atividade[i] = 0;
            }
        }



        //Intercala os primeiros n registros de n fitas
        while(AlgumaFitaAtiva(atividade, inicioLeitura, fimLeitura))
        {
            //Obtem de onde vem o menor registro
            fitaComMenor = PosicaoMenorRegistro(registros, numChars, tamanhoVetor);

            //Armazena o menor registro em uma fita de saida
            fprintf(fita[posEscrita], "%s", registros[fitaComMenor]);
            qtdRegistros++;   //Calcula o tamanho do bloco que está sendo escrito

            //Tenta ler um novo registro de onde o menor foi retirado
            posicaoAtual[fitaComMenor+inicioLeitura]++;

            if(posicaoAtual[fitaComMenor+inicioLeitura] > tamanhoBloco)  //Verifica se já leu um bloco inteiro
            {
                atividade[fitaComMenor+inicioLeitura] = 0;   // A fita é setada como inativa

                //Gera um peso "infinito" para que o registro proveniente da fita inativa nunca seja retornado na comparação dos registros
                GeraPeso(registros[fitaComMenor], numChars);
            }
            else
            {
                if(fgets(aux, numChars+2, fita[fitaComMenor+inicioLeitura])!=NULL) //Verifica se a fita já acabou
                {
                    strcpy(registros[fitaComMenor], aux);       //Se não, armazena o registro lido
                }
                else
                {
                    atividade[fitaComMenor+inicioLeitura] = 0;       //Se sim, seta a fita como inativa e copia o mesmo peso "infinito" para a sua posição no registro
                    GeraPeso(registros[fitaComMenor], numChars);
                }
            }
        }
        posEscrita++;   //Muda a fica de escrita

        //Verifica se já ordenou o arquivo inteiro
        if(qtdRegistros == registrosTotais)
        {
            break;
        }

        if(posEscrita == fimEscrita)   //Se já escreveu em todas as fitas reservadas para a escrita
        {
            //Passa a ler das fitas de escrita e escrever nas de leitura
            inicioLeitura = inicioEscrita;
            fimLeitura = fimEscrita;
            posEscrita = 0;
            inicioEscrita = 0;
            fimEscrita = numCaminhos;
            tamanhoBloco *= memDisponivel;

            for(i = 0; i < (numCaminhos*2); i++)
            {
                rewind(fita[i]);
            }
        }

    }

    free(atividade);
    free(posicaoAtual);
    free(aux);

    return posEscrita-1;

}

void LiberaRegistros(char **registros, int tamanho)
{
    int i;

    for(i=0; i<tamanho; i++)
    {
        mathias_free(registros[i]);
    }
    mathias_free(registros);

}

void copiaArquivo(const char *nomeArquivo, int numChars, FILE *fita[], int registrosTotais, int pos)
{
    int i;
    FILE *arquivo = fopen(nomeArquivo, "w");
    char *aux = malloc((sizeof(char)*numChars)+1);

    fprintf(arquivo, "%d\n", numChars);
    rewind(fita[pos]);

    for(i=0; i < registrosTotais; i++)
    {
        fgets(aux, numChars+2, fita[pos]);
        fprintf(arquivo, "%s", aux);
    }

    fclose(arquivo);
    free(aux);
}

int tamanhoArquivo(FILE *arquivo, int numChars)
{
    int i=0, num;
    char *aux = malloc((sizeof(char)*numChars)+1);
    fgets(aux, numChars+2, arquivo);

    while(fgets(aux, numChars+2, arquivo)!=NULL)
    {
        i++;
    }
    rewind(arquivo);
    fscanf(arquivo, "%d", &num);
    return i;
}
// Parâmetros:
//     - input_file: o nome do arquivo com os registros de entrada;
//     - output_file: o nome do arquivo com os registros de entrada ordenados;
//     - memory: o limite de memória a ser usado.
// Essa função deverá ler todos registros do arquivo `input_file`, ordená-los e
// escrevê-los em `output_file`, usando memória menor ou igual a `memory`. Lembre-se
// de fechar todos os arquivos que você usar por aqui!!

void external_sort(const char* input_file, const char* output_file, unsigned int memory) {
    FILE *inputFile = fopen(input_file, "r");
    int i, fitaOrdenada, numCaminhos;

    if(inputFile == NULL)
    {
        printf("Erro na abertura do arquivo %s\n",input_file);
    }
    else
    {
        int numChars;
        fscanf(inputFile,"%d",&numChars);
        //A memória disponível será a quantidade de registros que eu consigo armazenar em RAM
        unsigned int memDisponivel = floor((double)(memory*1024)/(numChars+2+sizeof(char*)));

        //Obtem o numero total de registros no arquivo
        int registrosTotais = tamanhoArquivo(inputFile, numChars);

        //Obtem o numero de caminhos
        numCaminhos = ceil(sqrt(ceil((double)registrosTotais/memDisponivel)));

        //Cria um array de arquivos que serão utilizados como fitas magnéticas
        FILE **fita = malloc(sizeof(FILE*)*numCaminhos*2);
        CriaFitas(fita, numCaminhos);

        //Cria o vetor que irá armazenar os registros em RAM
        char **registros;

        //Verifica se a memória disponível é maior do que a quantidade de registros
        if(memDisponivel > registrosTotais)
        {
            memDisponivel = registrosTotais;  //Se for, é necessário alocar um vetor somente com posições iguais a quantidade de registros totais
        }

        //Aloca o vetor responsável por trazer os registros para a memória primária
        registros = mathias_malloc(memDisponivel*sizeof(char*));
        for(i=0; i<memDisponivel; i++)
        {
            registros[i] = mathias_malloc(numChars+2);
            GeraPeso(registros[i],numChars);
        }

        //Fase inicial da intercalação de f-caminhos, onde se cria blocos ordenados
        CriaBlocosOrdenados(registros, memDisponivel, numChars, numCaminhos, registrosTotais, inputFile, fita);

        //Obtem a fita que contem o arquivo ordenado
        fitaOrdenada = Intercalacao(registros, memDisponivel, numCaminhos, numChars, registrosTotais, fita);


        //Copia o arquivo ordenado para o output_file
        copiaArquivo(output_file, numChars, fita, registrosTotais, fitaOrdenada);

        fclose(inputFile);
        FechaFitas(fita, numCaminhos);
        LiberaRegistros(registros, memDisponivel);

    }
}
