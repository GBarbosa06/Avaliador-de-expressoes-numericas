#include <stdio.h>
#include <string.h>
#include "expressao.h"

#define MAX 512

void testarConversao(const char *infixaExpr) {
    char posfixa[MAX];
    char infixaReconstruida[MAX];
    char infixaCopia[MAX];

    printf("====================================\n");
    printf("Expressão infixada original: %s\n", infixaExpr);

    strncpy(infixaCopia, infixaExpr, MAX);
    infixaCopia[MAX - 1] = '\0';

    strcpy(posfixa, getFormaPosFixa(infixaCopia));
    printf("Convertida para pós-fixada: %s\n", posfixa);

    strcpy(infixaReconstruida, getFormaInFixa(posfixa));
    printf("Reconstruída da pós-fixada: %s\n", infixaReconstruida);
    printf("====================================\n\n");
}

int main() {
    // Casos de teste
    testarConversao("3 + 4 * 2");
    testarConversao("3 + (4 * 2)");
    testarConversao("(3 + 4) * 2");
    testarConversao("sen(45) ^ 2 + 0,5");
    testarConversao("log(2 + 3) / 5");
    testarConversao("9 + (5 * (2 + 8 * 4))");
    testarConversao("cos(30) * (45 + 60)");

    // Casos malformados
    testarConversao("3 + * 2");
    testarConversao("sen( )");
    testarConversao("3 + (4 * 2");  // Parêntese aberto sem fechar

    return 0;
}
