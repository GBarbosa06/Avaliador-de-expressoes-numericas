#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "expressao.h"

#define MAX 512

//funções auxiliares

// Pilha de strings para operadores e funções
typedef struct {
    char items[MAX][32];
    int topo;
} PilhaStr;

// Adiciona um elemento ao topo da pilha de strings.
void push(PilhaStr *p, const char *str) {
    if (p->topo < MAX - 1) {
        strcpy(p->items[++(p->topo)], str);
    }
}

// Remove e retorna o elemento do topo da pilha de strings.
char *pop(PilhaStr *p) {
    if (p->topo >= 0) {
        return p->items[(p->topo)--];
    }
    return NULL;
}

// Retorna o elemento do topo da pilha sem removê-lo.
char *peek(PilhaStr *p) {
    if (p->topo >= 0) {
        return p->items[p->topo];
    }
    return NULL;
}

// Verifica se o token é um operador matemático (+, -, *, /, %, ^).
int ehOperador(const char *s) {
    return strcmp(s, "+") == 0 || strcmp(s, "-") == 0 ||
           strcmp(s, "*") == 0 || strcmp(s, "/") == 0 ||
           strcmp(s, "%") == 0 || strcmp(s, "^") == 0;
}

// Verifica se o token é uma função matemática (sen, cos, tg, log, raiz).
int ehFuncao(const char *s) {
    return strcmp(s, "sen") == 0 || strcmp(s, "cos") == 0 ||
           strcmp(s, "tg") == 0 || strcmp(s, "log") == 0 ||
           strcmp(s, "raiz") == 0;
}

// Retorna a precedência de um operador ou função.
int precedencia(const char *op) {
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) return 2;
    if (strcmp(op, "^") == 0) return 3;
    if (ehFuncao(op)) return 4; // maior precedência
    return 0;
}

// Verifica se o operador é associativo à esquerda.
int associativaEsquerda(const char *op) {
    return strcmp(op, "^") != 0;
}


//funções principais

char *getFormaInFixa(char *posfixa) {
    static char infixa[MAX];
    infixa[0] = '\0';

    char posfixa_copia[MAX];
    strncpy(posfixa_copia, posfixa, MAX);
    posfixa_copia[MAX-1] = '\0';

    typedef struct {
        char items[MAX][MAX];
        int topo;
    } PilhaStr;

    PilhaStr pilha;
    pilha.topo = -1;

    char *token = strtok(posfixa, " ");
    while (token != NULL) { 
        // converte vírgula para ponto no token, se houver
        for (int j = 0; token[j] != '\0'; j++) {
            if (token[j] == ',') token[j] = '.';
        }
        if (isdigit(token[0]) || (token[0] == '.' && isdigit(token[1])) || 
            (token[0] == '-' && (isdigit(token[1]) || token[1] == '.'))) {
            strcpy(pilha.items[++(pilha.topo)], token);
        }
        else if (strcmp(token, "+") == 0 || strcmp(token, "-") == 0 ||
                 strcmp(token, "*") == 0 || strcmp(token, "/") == 0 ||
                 strcmp(token, "%") == 0 || strcmp(token, "^") == 0) {
            // Operador binário
            if (pilha.topo < 1) {
                strcpy(infixa, "ERRO: expressao mal formada");
                return infixa;
            }

            char op2[MAX], op1[MAX], expr[MAX];
            strcpy(op2, pilha.items[pilha.topo--]);
            strcpy(op1, pilha.items[pilha.topo--]);
            snprintf(expr, MAX, "(%s %s %s)", op1, token, op2);
            strcpy(pilha.items[++(pilha.topo)], expr);
        }
        else if (strcmp(token, "sen") == 0 || strcmp(token, "cos") == 0 ||
                 strcmp(token, "tg") == 0 || strcmp(token, "log") == 0 ||
                 strcmp(token, "raiz") == 0) {
            // Função unária
            if (pilha.topo < 0) {
                strcpy(infixa, "ERRO: expressao mal formada");
                return infixa;
            }

            char op[MAX], expr[MAX];
            strcpy(op, pilha.items[pilha.topo--]);
            snprintf(expr, MAX, "%s(%s)", token, op);
            strcpy(pilha.items[++(pilha.topo)], expr);
        }
        else {
            // Token inválido
            strcpy(infixa, "ERRO: token invalido");
            return infixa;
        }

        token = strtok(NULL, " ");
    }

    if (pilha.topo == 0) {
        strcpy(infixa, pilha.items[0]);
    } else {
        strcpy(infixa, "ERRO: expressao mal formada");
    }

    return infixa;
}

char *getFormaPosFixa(char *infixa) {
    static char posfixa[MAX];
    posfixa[0] = '\0';

    char infixa_copia[MAX];
    strncpy(infixa_copia, infixa, MAX);
    infixa_copia[MAX-1] = '\0';

    // converte vírgulas em pontos, para melhor organização e compatibilidade com os cálculos feitos no código
    /* 
    for (int i = 0; infixa_copia[i] != '\0'; i++) {
        if (infixa_copia[i] == ',') infixa_copia[i] = '.';
    }
     */

    PilhaStr pilha;
    pilha.topo = -1;

    int i = 0;
    while (infixa_copia[i] != '\0') {
        if (isspace(infixa_copia[i])) {
            i++;
            continue;
        }

        // reconhece número normal, decimal e negativo, depois copia e adiciona à saída com espaço
        if (isdigit(infixa_copia[i]) || (infixa_copia[i] == '.' || (infixa_copia[i] == ',' && isdigit(infixa_copia[i + 1]))) ||
            (infixa_copia[i] == '-' && (isdigit(infixa_copia[i + 1]) || infixa_copia[i + 1] == '.' || infixa_copia[i + 1] == ','))) {
            char numero[32];
            int k = 0;
            if (infixa_copia[i] == '-') {
                numero[k++] = infixa_copia[i++];
            }
            while (isdigit(infixa_copia[i]) || infixa_copia[i] == '.' || infixa_copia[i] == ',') {
                numero[k++] = infixa_copia[i++];
            }
            numero[k] = '\0';
            strcat(posfixa, numero);
            strcat(posfixa, " ");
            continue;
        }

        // reconhece as funções
        if (strncmp(&infixa_copia[i], "sen", 3) == 0) {
            push(&pilha, "sen");
            i += 3;
            continue;
        }
        if (strncmp(&infixa_copia[i], "cos", 3) == 0) {
            push(&pilha, "cos");
            i += 3;
            continue;
        }
        if (strncmp(&infixa_copia[i], "tg", 2) == 0) {
            push(&pilha, "tg");
            i += 2;
            continue;
        }
        if (strncmp(&infixa_copia[i], "log", 3) == 0) {
            push(&pilha, "log");
            i += 3;
            continue;
        }
        if (strncmp(&infixa_copia[i], "raiz", 4) == 0) {
            push(&pilha, "raiz");
            i += 4;
            continue;
        }

        // Parênteses
        if (infixa_copia[i] == '(') {
            push(&pilha, "(");
            i++;
            continue;
        }
        if (infixa_copia[i] == ')') {
            while (pilha.topo >= 0 && strcmp(peek(&pilha), "(") != 0) {
                strcat(posfixa, pop(&pilha));
                strcat(posfixa, " ");
            }
            if (pilha.topo >= 0 && strcmp(peek(&pilha), "(") == 0) {
                pop(&pilha); // remove "("
            }
            // se após ')' houver uma função, ela deve ir para saída
            if (pilha.topo >= 0 && ehFuncao(peek(&pilha))) {
                strcat(posfixa, pop(&pilha));
                strcat(posfixa, " ");
            }
            i++;
            continue;
        }

        // Operadores
        if (strchr("+-*/%^", infixa_copia[i])) {
            char op[2] = {infixa_copia[i], '\0'};
            while (pilha.topo >= 0 && ehOperador(peek(&pilha)) &&
                   (precedencia(peek(&pilha)) > precedencia(op) ||
                    (precedencia(peek(&pilha)) == precedencia(op) &&
                     associativaEsquerda(op)))) {
                strcat(posfixa, pop(&pilha));
                strcat(posfixa, " ");
            }
            push(&pilha, op);
            i++;
            continue;
        }

        i++; // se não reconheceu nada, avança
    }

    // Desempilha o restante
    while (pilha.topo >= 0) {
        strcat(posfixa, pop(&pilha));
        strcat(posfixa, " ");
    }

    return posfixa;
}


/* 
    ! em ambas as funções do bloco 2 é importante  rodar uma função mais ou menos assim para converter de , para . quando necessário:
   for (int i = 0; expressao[i] != '\0'; i++) {
       if (expressao[i] == ',') expressao[i] = '.';
   } */

/* 


float getValorPosFixa(char *StrPosFixa){
    ver nota acima
}

float getValorInFixa(char *StrInFixa){
    ver nota acima
}
*/

int main() {
    char infixa[MAX] = "sen(45) ^ 2 + 0,5";
    printf("Infixa: %s\n", infixa);
    printf("Pos-fixa: %s\n\n", getFormaPosFixa(infixa));

    char posfixa1[MAX] = "0.5 45 sen 2 ^ +";
    printf("Pos-fixa: %s\n", posfixa1);
    printf("Infixa: %s\n\n", getFormaInFixa(posfixa1));

    return 0;
}