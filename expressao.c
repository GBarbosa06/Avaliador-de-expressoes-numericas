#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "expressao.h"

#define MAX 512
#define M_PI 3.14159265358979323846 // Definindo PI, pois o compilador não reconheceu do math.h

//funções auxiliares

//Pilhas
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

    // Pilha de floats para o getValorPosFixa.
        typedef struct {
            float items[MAX];
            int topo;
        } PilhaFloat;
        void pushFloat(PilhaFloat *p, float valor) {
            if (p->topo < MAX - 1) {
                p->items[++(p->topo)] = valor;
            }
        }
        float popFloat(PilhaFloat *p) {
            if (p->topo >= 0) {
                return p->items[(p->topo)--];
            }
            return NAN;
        }


// Funções de verificação
    // Verifica se o token é um número válido (inteiro, negativo ou decimal).
    int ehNumeroValido(const char *s) {
        int i = 0;
        int temDigito = 0;
        int temPonto = 0;

        // sinal opcional no começo
        if (s[i] == '-' || s[i] == '+') i++;

        for (; s[i] != '\0'; i++) {
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

    // Validação da expressão pós-fixada
    int validarPosfixa(const char *posfixa) {
        int count = 0;
        char copia[MAX];
        strncpy(copia, posfixa, MAX);

        char *token = strtok(copia, " ");
        while (token != NULL) {
            if (ehNumeroValido(token)) {
                count++;
            }
            else if (ehOperador(token)) {
                if (count < 2) return 0;
                count--; // consome 2, produz 1
            }
            else if (ehFuncao(token)) {
                if (count < 1) return 0;
                // consome 1, produz 1 (mantém count)
            }
            else {
                return 0; // token inválido
            }

            token = strtok(NULL, " ");
        }

        return count == 1;
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
    memset(posfixa, 0, sizeof(posfixa));
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

            // Verifica se o número é válido
            if (!ehNumeroValido(numero)) {
                strcpy(posfixa, "ERRO: numero invalido");
                return posfixa;
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
    if (!validarPosfixa(posfixa)) {
        strcpy(posfixa, "ERRO: expressao mal formada");
    }
    return posfixa;
}

float getValorPosFixa(char *StrPosFixa) {
    PilhaFloat pilha;
    pilha.topo = -1;

    char copia_posfixa[MAX];
    strncpy(copia_posfixa, StrPosFixa, MAX);
    copia_posfixa[MAX - 1] = '\0';

    char *token = strtok(copia_posfixa, " ");
    while (token != NULL) {
        for (int i = 0; token[i] != '\0'; i++) {
            if (token[i] == ',') token[i] = '.';
        }

        // Número
        if (ehNumeroValido(token)) {
            pushFloat(&pilha, strtof(token, NULL));
        }

        // Operador binário
        else if (ehOperador(token)) {
            float b = popFloat(&pilha);
            float a = popFloat(&pilha);
            if (isnan(a) || isnan(b)) {
                printf("ERRO: expressao mal formada\n");
                return NAN;
            }

            float res = 0;
            if (strcmp(token, "+") == 0) res = a + b;
            else if (strcmp(token, "-") == 0) res = a - b;
            else if (strcmp(token, "*") == 0) res = a * b;
            else if (strcmp(token, "/") == 0) {
                if (b == 0) {
                    printf("ERRO: divisao por zero\n");
                    return NAN;
                }
                res = a / b;
            }
            else if (strcmp(token, "%") == 0) {
                if ((int)b == 0) {
                    printf("ERRO: modulo por zero\n");
                    return NAN;
                }
                res = (int)a % (int)b;
            }
            else if (strcmp(token, "^") == 0) res = powf(a, b);
            else {
                printf("ERRO: operador desconhecido\n");
                return NAN;
            }

            pushFloat(&pilha, res);
        }

        // Função unária
        else if (ehFuncao(token)) {
            float x = popFloat(&pilha);
            if (isnan(x)) {
                printf("ERRO: expressao mal formada\n");
                return NAN;
            }

            float res = 0;
            if (strcmp(token, "sen") == 0) res = sinf(x * M_PI / 180.0f);
            else if (strcmp(token, "cos") == 0) res = cosf(x * M_PI / 180.0f);
            else if (strcmp(token, "tg") == 0) res = tanf(x * M_PI / 180.0f);
            else if (strcmp(token, "log") == 0) {
                if (x <= 0) {
                    printf("ERRO: log invalido\n");
                    return NAN;
                }
                res = log10f(x);
            }
            else if (strcmp(token, "raiz") == 0) {
                if (x < 0) {
                    printf("ERRO: raiz de negativo\n");
                    return NAN;
                }
                res = sqrtf(x);
            }
            else {
                printf("ERRO: funcao desconhecida\n");
                return NAN;
            }

            pushFloat(&pilha, res);
        }

        // Token inválido
        else {
            printf("ERRO: token invalido: %s\n", token);
            return NAN;
        }

        token = strtok(NULL, " ");
    }

    if (pilha.topo == 0) {
        return popFloat(&pilha);
    } else {
        printf("ERRO: expressao mal formada\n");
        return NAN;
    }
}

float getValorInFixa(char *StrInFixa) {
    // Primeiro converte a expressão infixa para posfixa
    
    char posfixa[MAX];
    strncpy(posfixa, getFormaPosFixa(StrInFixa), MAX);
    posfixa[MAX - 1] = '\0';

    return getValorPosFixa(posfixa);
}

int main() {
    char posfixa[MAX] = "0.5 45 sen 2 ^ +";
    char posfixaCopia[MAX];
    strcpy(posfixaCopia, posfixa);
    char infixa[MAX] = "sen(45) ^2 + 0,5";
    char infixaCopia[MAX];
    strcpy(infixaCopia, infixa);

    printf("Pos-fixa: %s\n", posfixa);
    printf("Infixa getFormaInFixa: %s\n\n", getFormaInFixa(posfixaCopia));


    // ! Quando getFormaPosFixa é chamado antes de getValorPosFixa, dá erro em getValorInFixa
    /* printf("Infixa: %s\n", infixa);
    printf("Pos-fixa: %s\n\n", getFormaPosFixa(infixaCopia)); */

    printf("Valor da pos-fixa: %.2f\n", getValorPosFixa(posfixa));
    printf("Valor da infixa: %.2f\n", getValorInFixa(infixa));

    return 0;
}
