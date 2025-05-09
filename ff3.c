#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

typedef struct
{
    int x;
    int y;
    int inf;
} PecaC;

typedef struct
{
    int IL, IC;
    char *result;
    int **visitados;
} Frame;

void calcularMovimentos(PecaC peca, int *dx, int *dy)
{
    int dx1 = peca.x;
    int dy1 = peca.y;

    dx[0] = -dy1;
    dy[0] = -dx1;
    dx[1] = dx1;
    dy[1] = dy1;
    dx[2] = dx1;
    dy[2] = -dy1;
    dx[3] = -dx1;
    dy[3] = dy1;
    dx[4] = -dx1;
    dy[4] = -dy1;
    dx[5] = dy1;
    dy[5] = dx1;
    dx[6] = dy1;
    dy[6] = -dx1;
    dx[7] = -dy1;
    dy[7] = dx1;
}

void atualizarResultado(char *result, const char *palavra)
{
    int len = strlen(result);
    if (len > 0 && result[len - 1] == palavra[0])
    {
        result[len - 1] = '\0';
        char temp[100];
        sprintf(temp, "[%c]%s", palavra[0], palavra + 1);
        strcat(result, temp);
    }
    else
    {
        strcat(result, palavra);
    }
}

int dfs(int L, int C, int nPecaCs, PecaC peca[], char mapa[L][C], int x, int y,
        int *lastL, int *lastC, char palavra[], int indexP, int visitados[L][C], int *finais[], int *count)
{
    //printf("letra = %c em %d %d\n", mapa[x][y], x, y);
    if (palavra[indexP] == '\0')
    {
        //printf("FIM de palavra %s\n", palavra);
        finais[0][*count] = x;
        finais[1][*count] = y;
        (*count)++;
        return 1;
    }

    int end = 0;
    for (int i = 0; i < nPecaCs; i++)
    {
        int dx[8], dy[8];
        calcularMovimentos(peca[i], dx, dy);
        for (int d = 0; d < 8; d++)
        {
            int passo = 1, flag = 1;
            while (flag)
            {
                int proxX = x + passo * dx[d];
                int proxY = y + passo * dy[d];

                if (proxX >= 0 && proxX < L && proxY >= 0 && proxY < C)
                {
                    if (mapa[proxX][proxY] == palavra[indexP] && !visitados[proxX][proxY])
                    {
                        *lastL = proxX;
                        *lastC = proxY;
                        end = dfs(L, C, nPecaCs, peca, mapa, proxX, proxY, lastL, lastC, palavra, indexP + 1, visitados, finais, count);
                        if (end)
                            visitados[x][y] = 1;
                    }
                    if (peca[i].inf)
                        passo++;
                    else
                        flag = 0;
                }
                else
                {
                    flag = 0;
                }
            }
        }
    }
    return end;
}

int Fim(int L, int C, int IL, int IC, int FL, int FC, char mapa[L][C], int d, char **dict, char result[10000], int *bestLength, char bestResult[10000])
{
    if (IC == FC && IL == FL)
    {
        int len = strlen(result);
        if (len == 0)
        {
            char stringF[2] = {mapa[FL][FC], '\0'};
            for (int i = 0; i < d; i++)
            {
                if (!strcmp(stringF, dict[i]))
                {
                    strcpy(result, stringF);
                    break;
                }
            }
        }
        if (len < *bestLength)
        {
            *bestLength = len;
            strcpy(bestResult, result);
        }
        return 1;
    }
    return 0;
}

void casoEspecial(char *result, const char *palavra, int IL, int IC, int FL, int FC, char bestResult[10000], int *bestLength)
{
    if (IL == FL && IC == FC)
    {
        char temp[100000];
        strcpy(temp, result);
        atualizarResultado(temp, palavra);
        int len = strlen(temp);
        if (len < *bestLength)
        {
            *bestLength = len;
            strcpy(bestResult, temp);
        }
    }
}

void pecorrerDictIterativo(int L, int C, int nPecaCs, PecaC peca[], char mapa[L][C], int IL, int IC, int FL, int FC, char **dict, int d, char bestResult[10000], int *bestLength)
{

    Frame stack[10000];
    int top = 0;

    Frame inicio;
    inicio.IL = IL;
    inicio.IC = IC;
    inicio.result = malloc(10000);
    strcpy(inicio.result, "");
    inicio.visitados = malloc(L * sizeof(int *));
    for (int i = 0; i < L; i++)
    {
        inicio.visitados[i] = calloc(C, sizeof(int));
    }
    stack[top++] = inicio;

    while (top > 0)
    {
        Frame atual = stack[--top];

        if (Fim(L, C, atual.IL, atual.IC, FL, FC, mapa, d, dict, atual.result, bestLength, bestResult))
        {
            for (int i = 0; i < L; i++)
                free(atual.visitados[i]);
            free(atual.visitados);
            free(atual.result);
            continue;
        }

        for (int i = 0; i < d; i++)
        {
            if (dict[i][0] == mapa[atual.IL][atual.IC])
            {
                int lenPalavra = strlen(dict[i]);
                if (lenPalavra == 1)
                {
                    casoEspecial(atual.result, dict[i], atual.IL, atual.IC, FL, FC, bestResult, bestLength);
                }
                else
                {
                    int proxL = atual.IL, proxC = atual.IC;
                    int tempVisitados[L][C];
                    for (int a = 0; a < L; a++)
                        memcpy(tempVisitados[a], atual.visitados[a], C * sizeof(int));

                    int *finais[2];
                    finais[0] = malloc(10000 * sizeof(int));
                    finais[1] = malloc(10000 * sizeof(int));
                    int count = 0;
                    //printf("palvra = %s\n", dict[i]);
                    //printf("result atual = %s\n", atual.result);
                    dfs(L, C, nPecaCs, peca, mapa, atual.IL, atual.IC, &proxL, &proxC, dict[i], 1, tempVisitados, finais, &count);

                    for (int k = 0; k < count; k++)
                    {
                        Frame novo;
                        novo.IL = finais[0][k];
                        novo.IC = finais[1][k];
                        novo.result = malloc(10000);

                        char temp[10000];
                        strcpy(temp, atual.result);
                        atualizarResultado(temp, dict[i]);
                        strcpy(novo.result, temp);

                        novo.visitados = malloc(L * sizeof(int *));
                        for (int a = 0; a < L; a++)
                        {
                            novo.visitados[a] = malloc(C * sizeof(int));
                            memcpy(novo.visitados[a], tempVisitados[a], C * sizeof(int));
                        }
                        stack[top++] = novo;
                    }
                    free(finais[0]);
                    free(finais[1]);
                }
            }
        }
        for (int i = 0; i < L; i++)
            free(atual.visitados[i]);
        free(atual.visitados);
        free(atual.result);
    }
}

int solve(int L, int C, char mapa[L][C], int nPecaCs, PecaC peca[], int IL, int IC, int FL, int FC, int d, char **dict)
{
    char bestResult[10000] = "";
    int bestLength = 100000;

    pecorrerDictIterativo(L, C, nPecaCs, peca, mapa, IL, IC, FL, FC, dict, d, bestResult, &bestLength);

    if (bestLength < 10000)
    {
        printf("%s\n", bestResult);
        return 1;
    }
    return 0;
}

void processarCasos(int x, int L, int C, char mapa[L][C], int d, char **dict)
{
    for (int i = 0; i < x; i++)
    {
        int nPecaCs;
        assert(scanf(" %d", &nPecaCs) == 1);
        PecaC peca[nPecaCs];

        for (int j = 0; j < nPecaCs; j++)
            assert(scanf(" %d %d %d", &peca[j].x, &peca[j].y, &peca[j].inf) == 3);

        int IL, IC, FL, FC;
        assert(scanf(" %d %d", &IL, &IC) == 2);
        assert(scanf(" %d %d", &FL, &FC) == 2);

        printf("Caso %d\n", i + 1);
        int r = solve(L, C, mapa, nPecaCs, peca, IL - 1, IC - 1, FL - 1, FC - 1, d, dict);
        if (r)
            printf("SIM\n");
        else
            printf("NAO\n");
    }
}

int main()
{
    int d;
    assert(scanf(" %d", &d) == 1);
    char *dict[d];
    for (int i = 0; i < d; i++)
    {
        dict[i] = malloc(500 * sizeof(char));
        assert(scanf(" %s", dict[i]) == 1);
    }

    int L, C;
    assert(scanf(" %d %d", &L, &C) == 2);
    char mapa[L][C + 1];
    for (int i = 0; i < L; i++)
    {
        assert(scanf("%s", mapa[i]) == 1);
    }

    int x;
    assert(scanf(" %d", &x) == 1);
    processarCasos(x, L, C + 1, mapa, d, dict);

    for (int i = 0; i < d; i++)
    {
        free(dict[i]);
    }

    return 0;
}
