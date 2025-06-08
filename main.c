#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expressao.h"

int main() {
    // Casos de teste
    char *testes_infixa[] = {
        "3 + 4",
        "2 * (3 + 5)",
        "log(10)",
        "sen(0)",
        "3 +",
        "5 * * 2",
        "raiz(9) + 1",
        "2 ^ 3 ^ 2",
        "-4 + 3.5",
        "tg(45)",
        NULL
    };

    for (int i = 0; testes_infixa[i] != NULL; i++) {
        printf("=== Teste %d ===\n", i + 1);
        char *infixa = testes_infixa[i];
        printf("Infixa: %s\n", infixa);

        char *posfixa = getFormaPosFixa(infixa);
        printf("Posfixa: %s\n", posfixa);

        if (strncmp(posfixa, "ERRO", 4) != 0) {
            char *reconstruida = getFormaInFixa(posfixa);
            printf("Infixa Reconstruída: %s\n", reconstruida);

            float resultado = getValorPosFixa(posfixa);
            printf("Resultado: %.6f\n", resultado);
        } else {
            printf("Erro detectado na conversão para pós-fixa.\n");
        }
        printf("\n");
    }

    return 0;
}
