#include <stdio.h>

// ===================================================
// GRUPO 3 - LAN HOUSE / CYBER CAFE ANOS 2000
// ===================================================


// ===================================================
// PARTE 1 - STRUCTS E VETORES GLOBAIS
// ===================================================


// ---------------------------------------------------
// STRUCT 1 - CLIENTES
// Autor: Miguel Otavio Parreira Medeiros De Oliveira
// ---------------------------------------------------

// Adicionar aqui a struct
struct Cliente {
    int id;
    char nome[50];
    char telefone[20];
    float saldo;
    int ativo;
};
struct Cliente clientes[50];
int totalClientes = 0;

// ---------------------------------------------------
// STRUCT 2 - COMPUTADORES
// Autor: Gabriel Bandeira Leopoldo
// ---------------------------------------------------

// Adicionar aqui a struct
struct Computador{
    int id;
    char nome[50];
    float precoHora;
    int disponivel;
    int ativo;
};
struct Computador computadores[50];
int totalComputadores = 0;


// ---------------------------------------------------
// STRUCT 3 - PRODUTOS E ESTOQUE
// Autor: Pedro Henrique da Costa Silva
// ---------------------------------------------------

// Adicionar aqui a struct


// ---------------------------------------------------
// STRUCT 4 - SESSOES E RELATORIOS
// Autor: Lucas Henrique Santos Pedrette
// ---------------------------------------------------

// Adicionar aqui a struct



// ===================================================
// PARTE 2 - FUNCOES
// ===================================================


// ===================================================
// MODULO 1 - CLIENTES
// Autor: Miguel Otavio Parreira Medeiros De Oliveira
// ===================================================

void moduloClientes() {
    int opcao;
    do {
        printf("\n===== CLIENTES =====\n");
        printf("1 - Cadastrar cliente\n");
        printf("2 - Listar / Buscar clientes\n");
        printf("3 - Editar cliente\n");
        printf("4 - Remover cliente\n");
        printf("0 - Voltar\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        switch (opcao) {
            case 1:
                printf("Cadastro em desenvolvimento.\n");
                break;
            case 2:
                printf("Consulta em desenvolvimento.\n");
                break;
            case 3:
                printf("Edicao em desenvolvimento.\n");
                break;
            case 4:
                printf("Remocao em desenvolvimento.\n");
                break;
            case 0:
                printf("Voltando ao menu principal.\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 0);
}


// ===================================================
// MODULO 2 - COMPUTADORES
// Autor: Gabriel Bandeira Leopoldo
// ===================================================

void moduloComputadores() {
    printf("\nModulo de Computadores - Em desenvolvimento\n");
     int opcao;
    do{
    printf("\n===== COMPUTADORES =====\n");
    printf("1 - Cadastrar computador\n");
    printf("2 - Listar / Buscar computadores\n");
    printf("3 - Editar computador\n");
    printf("4 - Remover computador\n");
    printf("0 - Voltar\n");
    printf("Escolha: ");
    scanf("%d", &opcao);
        switch (opcao) {
            case 1:
            printf("Cadastro em desenvolvimento.\n");
            break;
            case 2:
            printf("Consulta em desenvolvimento.\n");
            break;
            case 3:
            printf("Edicao em desenvolvimento.\n");
            break;
            case 4:
            printf("Remocao em desenvolvimento.\n");
            break;
            case 0:
            printf("Voltando ao menu principal.\n");
            break;
            default:
                printf("Opcao invalida!\n");
        }
    }while (opcao != 0);
}


// ===================================================
// MODULO 3 - PRODUTOS E ESTOQUE
// Autor: Pedro Henrique da Costa Silva
// ===================================================

void moduloProdutos() {
    printf("\nModulo de Produtos - Em desenvolvimento\n");
}


// ===================================================
// MODULO 4 - SESSOES E RELATORIOS
// Autor: Lucas Henrique Santos Pedrette
// ===================================================

void moduloSessoes() {
    printf("\nModulo de Sessoes - Em desenvolvimento\n");
}



// ===================================================
// PARTE 3 - MENU PRINCIPAL
// Responsabilidade: Integracao do Grupo 3
// ===================================================

int main() {

    int opcao;

    do {
        printf("\n==================================\n");
        printf("        LAN HOUSE ANOS 2000       \n");
        printf("==================================\n");

        printf("1 - Clientes\n");
        printf("2 - Computadores\n");
        printf("3 - Produtos e Estoque\n");
        printf("4 - Sessoes e Relatorios\n");
        printf("0 - Sair\n");

        printf("\nEscolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {

            case 1:
                moduloClientes();
                break;

            case 2:
                moduloComputadores();
                break;

            case 3:
                moduloProdutos();
                break;

            case 4:
                moduloSessoes();
                break;

            case 0:
                printf("\nEncerrando o sistema...\n");
                break;

            default:
                printf("\nOpcao invalida!\n");
        }

    } while (opcao != 0);

    return 0;
}
