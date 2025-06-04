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
    char items[MAX][MAX];
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

// Essa struct é usada para o getFormaInFixa, para armazenar a expressão e sua precedência
// Importante para a formatação correta da expressão infixa final sem excesso de parênteses.
typedef struct {
    char str[MAX];
    int precedencia;
} Expr;

// Pilha de expressões para a conversão de posfixa para infixa.
    typedef struct {
        Expr items[MAX];
        int topo;
    } PilhaExpr;

    void pushExpr(PilhaExpr *p, Expr expr) {
        if (p->topo < MAX - 1) {
            p->items[++(p->topo)] = expr;
        }
    }
    Expr popExpr(PilhaExpr *p) {
        if (p->topo >= 0) {
            return p->items[(p->topo)--];
        }
        Expr vazio = {"", 0};
        return vazio;
    }
    Expr peekExpr(PilhaExpr *p) {
        if (p->topo >= 0) {
            return p->items[p->topo];
        }
        Expr vazio = {"", 0};
        return vazio;
    }

// Verifica se o token é um número válido (inteiro, negativo ou decimal).
int ehNumeroValido(const char *s) {
    int i = 0;
    int temDigito = 0;
    int temPonto = 0;

    // sinal opcional no começo
    if (s[i] == '-' || s[i] == '+') i++;

    for (i ; s[i] != '\0'; i++) {
        if (isdigit(s[i])) {
            temDigito = 1;
        }
        else if (s[i] == '.') {
                if (temPonto) return 0; // só um ponto permitido
            temPonto = 1;
        }
        else {
            return 0; // caractere inválido
        }
    }

    return temDigito; // válido só se tiver pelo menos um dígito
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
    posfixa_copia[MAX - 1] = '\0';

    PilhaExpr pilha;
    pilha.topo = -1;

    char *token = strtok(posfixa_copia, " ");
    while (token != NULL) {
        for (int j = 0; token[j] != '\0'; j++) {
            if (token[j] == ',') token[j] = '.';
        }

        if (ehNumeroValido(token)) {
            Expr e;
            strcpy(e.str, token);
            e.precedencia = 100; // máxima
            pushExpr(&pilha, e);
        }

        // Operador binário
        else if (ehOperador(token)) {
            Expr op2 = popExpr(&pilha);
            Expr op1 = popExpr(&pilha);
            if (strlen(op1.str) == 0 || strlen(op2.str) == 0) {
                strcpy(infixa, "ERRO: expressao mal formada");
                return infixa;
            }

            int prec = precedencia(token);
            char s1[MAX], s2[MAX], expr[MAX];

            // Aqui é onde ocorre a formatação dos parênteses
            // É considerada a precedência dos operadores para evitar redundâncias 
            if (op1.precedencia < prec)
                snprintf(s1, MAX, "(%s)", op1.str);
            else
                strcpy(s1, op1.str);

            if (op2.precedencia < prec ||
                (op2.precedencia == prec && !associativaEsquerda(token)))
                snprintf(s2, MAX, "(%s)", op2.str);
            else
                strcpy(s2, op2.str);

            snprintf(expr, MAX, "%s %s %s", s1, token, s2);

            Expr novo;
            strcpy(novo.str, expr);
            novo.precedencia = prec;
            pushExpr(&pilha, novo);
        }

        // Função unária
        else if (ehFuncao(token)) {
            Expr op = popExpr(&pilha);

            if (strlen(op.str) == 0) {
                strcpy(infixa, "ERRO: expressao mal formada");
                return infixa;
            }

            char expr[MAX];
            snprintf(expr, MAX, "%s(%s)", token, op.str);

            Expr novo;
            strcpy(novo.str, expr);
            novo.precedencia = precedencia(token); // geralmente 4
            pushExpr(&pilha, novo);
            token = strtok(NULL, " ");
            continue;
        }

        // Se o token não for reconhecido, retorna erro
        else {
            strcpy(infixa, "ERRO: token invalido");
            return infixa;
        }

        token = strtok(NULL, " ");
    }

    if (pilha.topo == 0) {
        strcpy(infixa, popExpr(&pilha).str);
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
    for (int i = 0; infixa_copia[i] != '\0'; i++) {
        if (infixa_copia[i] == ',') infixa_copia[i] = '.';
    }
    
    // Cria uma pilha vazia
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

        i++; // se não reconheceu nada, avança para o próximo caractere
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
   }
   
   * Em ambas as getFormas tem essa conversão, mas caso a função seja rodada direto na entrada do usuário, vai dar erro sem essa conversão.

/* 

float getValorPosFixa(char *StrPosFixa){
    * ver nota acima
    * Lembrar de considerar a associatividade ("^" é a única que não é associativa à esquerda)
}

float getValorInFixa(char *StrInFixa){
    * ver nota acima
    * Lembrar de considerar a associatividade ("^" é a única que não é associativa à esquerda)
}
*/


// esse main é apenas para testes rápidos, não faz parte do código final
int main() {
    
    char posfixa[MAX] = "3 4 + 5 * ";
    printf("Pos-fixa: %s\n", posfixa);
    printf("Infixa getFormaInFixa: %s\n\n", getFormaInFixa(posfixa));
    
    char infixa[MAX] = "(25 + 21) / 4 * (3 + 1)";
    printf("Infixa: %s\n", infixa);
    printf("Pos-fixa: %s\n\n", getFormaPosFixa(infixa));

    return 0;
}