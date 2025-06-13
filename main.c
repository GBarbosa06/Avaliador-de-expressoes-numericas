#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "expressao.h"

// Estrutura para armazenar casos de teste
typedef struct {
    char *id;                // Identificador do teste
    char *expressao;         // Expressão a ser avaliada (infixa)
    char *descricao;         // Descrição ou observações sobre o teste
    char *resultado_esperado;// Valor esperado como resultado (informativo)
} Teste;

int main() {
    // Conjunto de testes automáticos para validação do sistema
    Teste testes[] = {
        {"1", "3 4 + 5 * (3 + 4) * 5", "1 3 4 + 5 * (3 + 4) * 5", "35"},
        {"2", "7 2 * 4 + 7 * 2 + 4", "2 7 2 * 4 + 7 * 2 + 4", "18"},
        {"3", "8 5 2 4 + * + 8 +", "3 8 5 2 4 + * + 8 + (5 * (2 + 4))", "38"},
        {"4", "6 2 / 3 + 4 * (6 / 2 + 3) * 4", "4 6 2 / 3 + 4 * (6 / 2 + 3) * 4", "24"},
        {"5", "9 5 2 8 * 4 + * + 9 +", "5 9 5 2 8 * 4 + * + 9 + (5 * (2 + 8 * 4))", "109"},
        {"6", "2 3 + log 5 / log(2 + 3) / 5", "6 2 3 + log 5 / log(2 + 3) / 5", "Aprox. 0.14"},
        {"7", "10 log 3 ^ 2 +", "7 10 log 3 ^ 2 + (log10)^3 + 2", "3"},
        {"8", "45 60 + 30 cos *", "8 45 60 + 30 cos * (45 + 60) * cos(30)", "Aprox. 90,93"},
        {"9", "0.5 45 sen 2 ^ +", "9 0.5 45 sen 2 ^ + sen(45) ^2 + 0,5", "1"},
        {NULL, NULL, NULL, NULL} // Fim da lista
    };

    int modo;
    printf("Escolha o modo:\n");
    printf("1 - Rodar testes automaticos\n");
    printf("2 - Entrada manual\n");
    printf("Opcao: ");
    scanf("%d", &modo);
    getchar(); // Limpa o '\n' do buffer do teclado

    if (modo == 1) {
        for (int i = 0; testes[i].expressao != NULL; i++) {
            printf("=== Teste %s ===\n", testes[i].id);
            printf("Expressao: %s\n", testes[i].expressao);

            char *posfixa = getFormaPosFixa(testes[i].expressao);
            if (posfixa == NULL) {
                printf("Erro na conversao para posfixa.\n\n");
                continue;
            }
            printf("Posfixa: %s\n", posfixa);

            if (strncmp(posfixa, "ERRO", 4) != 0) {
                char *reconstruida = getFormaInFixa(posfixa);
                if (reconstruida != NULL) {
                    printf("Infixa Reconstruida: %s\n", reconstruida);
                    // Se getFormaInFixa alocasse dinamicamente, deveríamos usar free() aqui
                }

                float resultado = getValorPosFixa(posfixa);
                if (isnan(resultado)) {
                    printf("Resultado: ERRO na avaliacao\n");
                } else {
                    printf("Resultado: %.6f\n", resultado);
                }
            } else {
                printf("Erro detectado na conversao para posfixa.\n");
            }

            printf("Resultado esperado: %s\n\n", testes[i].resultado_esperado);
        }

    } else if (modo == 2) {
        // Entrada manual de expressão pelo usuário
        char entrada[256];
        printf("Digite uma expressao infixa:\n");
        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            printf("Erro ao ler a expressao.\n");
            return 1;
        }

        entrada[strcspn(entrada, "\n")] = 0; // Remove o '\n'

        char *posfixa = getFormaPosFixa(entrada);
        if (posfixa == NULL) {
            printf("Erro na conversao para posfixa.\n");
            return 1;
        }

        printf("Posfixa: %s\n", posfixa);

        if (strncmp(posfixa, "ERRO", 4) != 0) {
            char *reconstruida = getFormaInFixa(posfixa);
            if (reconstruida != NULL) {
                printf("Infixa Reconstruida: %s\n", reconstruida);
                // Se getFormaInFixa alocasse, deveria liberar aqui
            }

            float resultado = getValorPosFixa(posfixa);
            if (isnan(resultado)) {
                printf("Resultado: ERRO na avaliacao\n");
            } else {
                printf("Resultado: %.6f\n", resultado);
            }
        } else {
            printf("Erro detectado na conversao para posfixa.\n");
        }

    } else {
        printf("Opcao invalida.\n");
    }

    return 0;
}
