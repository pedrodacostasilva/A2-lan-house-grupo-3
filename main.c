// ===================================================
// GRUPO 3 - LAN HOUSE / CYBER CAFE ANOS 2000
// ===================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define MAX_REGISTROS 50

// FUNCOES DE APOIO
int limparEntrada(void) {
    int caractere, sobrou = 0;
    do {
        caractere = getchar();
        if (caractere != '\n' && caractere != EOF &&
            caractere != ' ' && caractere != '\t' && caractere != '\r') sobrou = 1;
    } while (caractere != '\n' && caractere != EOF);
    return sobrou;
}

void limparTela(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausar(void) {
    printf("\nPressione ENTER para continuar...");
    getchar();
}

float arredondarValor(float valor) {
    int centavos = (int)(valor * 100.0f + 0.5f);
    return centavos / 100.0f;
}

int lerInteiro(const char mensagem[], int minimo, int maximo) {
    int numero, lidos, sobrou;
    do {
        printf("%s", mensagem);
        lidos = scanf("%9d", &numero);
        if (lidos == EOF) exit(0);
        sobrou = limparEntrada();
        if (lidos == 1 && !sobrou && numero >= minimo && numero <= maximo) return numero;
        printf("Digite um inteiro entre %d e %d.\n", minimo, maximo);
    } while (1);
}

float lerReal(const char mensagem[], float minimo, float maximo) {
    float numero;
    int lidos, sobrou;
    do {
        printf("%s", mensagem);
        lidos = scanf("%f", &numero);
        if (lidos == EOF) exit(0);
        sobrou = limparEntrada();
        if (lidos == 1 && !sobrou && numero >= minimo && numero <= maximo)
            return arredondarValor(numero);
        printf("Digite um numero entre %.2f e %.2f. Use ponto decimal.\n", minimo, maximo);
    } while (1);
}

void lerTexto(const char mensagem[], char destino[], int tamanho) {
    char texto[50];
    int lidos, sobrou, temSeparador, i;
    do {
        printf("%s", mensagem);

        lidos = scanf(" %49[^\n]", texto);
        if (lidos == EOF) exit(0);
        sobrou = limparEntrada();
        if (lidos != 1) continue;
        if (sobrou || (int)strlen(texto) >= tamanho) {
            printf("Texto muito longo. Tente novamente.\n");
        } else {
            temSeparador = 0;
            for (i = 0; texto[i] != '\0'; i++) {
                if (texto[i] == ';') temSeparador = 1;
            }
            if (temSeparador) printf("Preencha o texto sem ponto e virgula (;).\n");
            else { strcpy(destino, texto); return; }
        }
    } while (1);
}

int dataValida(const char data[]) {
    int i, dia, mes, ano;
    int diasMes[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (strlen(data) != 10 || data[2] != '/' || data[5] != '/') return 0;
    for (i = 0; i < 10; i++) {
        if (i != 2 && i != 5 && (data[i] < '0' || data[i] > '9')) return 0;
    }

    dia = (data[0] - '0') * 10 + data[1] - '0';
    mes = (data[3] - '0') * 10 + data[4] - '0';
    ano = (data[6] - '0') * 1000 + (data[7] - '0') * 100 +
          (data[8] - '0') * 10 + data[9] - '0';
    if (ano < 1900 || ano > 2100 || mes < 1 || mes > 12) return 0;
    if ((ano % 4 == 0 && ano % 100 != 0) || ano % 400 == 0) diasMes[1] = 29;
    return dia >= 1 && dia <= diasMes[mes - 1];
}

void lerData(char data[]) {
    do {
        lerTexto("Data (DD/MM/AAAA): ", data, 11);
        if (dataValida(data)) return;
        printf("Data invalida.\n");
    } while (1);
}

// PROTOTIPOS
int clienteTemSessaoAberta(int id);
int computadorTemSessaoAberta(int id);

// MODULOS
#include "clientes.h"
#include "computadores.h"
#include "produtos.h"
#include "sessoes.h"

// MENU PRINCIPAL
int main(void) {
    int opcao;

    setlocale(LC_CTYPE, "Portuguese");
    if (!carregarClientesCSV() || !carregarComputadoresCSV() ||
        !carregarProdutosCSV() || !carregarSessoesCSV() || !validarRelacionamentos()) {
        printf("Inicializacao interrompida. Corrija os CSVs; os dados nao serao regravados.\n");
        return 1;
    }
    do {
        limparTela();
        printf("\n==================================\n");
        printf("        LAN HOUSE ANOS 2000\n");
        printf("==================================\n");
        printf("1 - Clientes\n2 - Computadores\n3 - Produtos e Estoque\n4 - Sessoes e Relatorios\n0 - Salvar e sair\n");
        opcao = lerInteiro("Escolha: ", 0, 4);

        switch (opcao) {
            case 1: moduloClientes(); break;
            case 2: moduloComputadores(); break;
            case 3: moduloProdutos(); break;

            case 4: moduloSessoes();
        }
    } while (opcao != 0);

    if (!salvarClientesCSV() || !salvarComputadoresCSV() ||
        !salvarProdutosCSV() || !salvarSessoesCSV()) return 1;
    printf("Dados salvos. Ate logo!\n");
    return 0;
}
