// ===================================================
// MODULO 2 - COMPUTADORES
// Autor: Gabriel Bandeira Leopoldo
// ===================================================

#ifndef COMPUTADORES_H
#define COMPUTADORES_H
#include <stdio.h>
#include <string.h>

// STRUCT E VETOR GLOBAL
struct Computador {
    int id;
    char nome[50];
    float precoHora;
    int disponivel;
    int ativo;
};
struct Computador computadores[MAX_REGISTROS];
int totalComputadores = 0;

// FUNCOES
int buscarIndiceComputador(int id) {
    for (int i = 0; i < totalComputadores; i++) {
        if (computadores[i].id == id) return i;
    }
    return -1;
}

// Regrava os registros do vetor no arquivo CSV.
int salvarComputadoresCSV(void) {
    FILE *arq = fopen("computadores.csv", "w");
    int ok = 1;
    if (arq == NULL) { printf("[ERRO] Nao foi possivel abrir computadores.csv para salvar.\n"); return 0; }
    for (int i = 0; i < totalComputadores; i++) {
        if (fprintf(arq, "%d;%s;%.2f;%d;%d\n", computadores[i].id, computadores[i].nome, computadores[i].precoHora, computadores[i].disponivel, computadores[i].ativo) < 0) ok = 0;
    }
    if (fclose(arq) != 0) ok = 0;
    if (!ok) printf("[ERRO] Falha ao gravar computadores.csv. Verifique o disco.\n");
    return ok;
}

// Carrega os registros do CSV para o vetor.
int carregarComputadoresCSV(void) {
    FILE *arq = fopen("computadores.csv", "r");
    struct Computador item;
    int lidos, numeroLinha = 0;
    totalComputadores = 0;
    if (arq == NULL) {

        return 1;
    }
    while ((lidos = fscanf(arq, "%d;%49[^;\r\n];%f;%d;%d\n", &item.id, item.nome, &item.precoHora, &item.disponivel, &item.ativo)) == 5) {
        numeroLinha++;
        if (totalComputadores >= MAX_REGISTROS || item.id <= 0 ||
            (item.ativo != 0 && item.ativo != 1) ||
            !(item.precoHora >= 0.01f && item.precoHora <= 10000) || (item.disponivel != 0 && item.disponivel != 1) || buscarIndiceComputador(item.id) != -1) {
            printf("[ERRO] computadores.csv: registro %d invalido ou limite excedido.\n", numeroLinha);
            fclose(arq); return 0;
        }
        computadores[totalComputadores] = item;
        totalComputadores++;
    }

    if (lidos != EOF || ferror(arq)) {
        printf("[ERRO] computadores.csv: formato incorreto.\n");
        fclose(arq); return 0;
    }
    return fclose(arq) == 0;
}

void mostrarComputador(int pos) {
    printf("ID: %d | %-25s | Hora: R$ %.2f | %s\n",
           computadores[pos].id, computadores[pos].nome, computadores[pos].precoHora,
           computadores[pos].disponivel ? "Livre" : "Ocupado");
}

void cadastrarComputador(void) {
    struct Computador novo;
    if (totalComputadores >= MAX_REGISTROS) { printf("Limite de 50 registros atingido.\n"); return; }
    novo.id = lerInteiro("ID: ", 1, 999999);
    if (buscarIndiceComputador(novo.id) != -1) { printf("ID ja cadastrado (mesmo se inativo).\n"); return; }
    lerTexto("Nome: ", novo.nome, sizeof(novo.nome));
    novo.precoHora = lerReal("Preco por hora (R$): ", 0.01f, 10000);
    novo.disponivel = 1;
    novo.ativo = 1;
    computadores[totalComputadores] = novo;
    totalComputadores++;
    if (!salvarComputadoresCSV()) { totalComputadores--; printf("Cadastro nao confirmado. Confira o CSV antes de continuar.\n"); exit(1); }
    printf("Cadastro salvo.\n");
}

void listarComputadores(void) {
    int encontrados = 0;
    for (int i = 0; i < totalComputadores; i++) {
        if (computadores[i].ativo) { mostrarComputador(i); encontrados++; }
    }
    if (!encontrados) printf("Nenhum registro ativo.\n");
}

void buscarComputadores(void) {
    int modo = lerInteiro("1 - Por ID | 2 - Por parte do nome: ", 1, 2);
    int pos, encontrados = 0;
    char termo[50];
    if (modo == 1) {
        pos = buscarIndiceComputador(lerInteiro("ID: ", 1, 999999));
        if (pos >= 0 && computadores[pos].ativo) { mostrarComputador(pos); return; }
    } else {
        lerTexto("Trecho do nome (respeite maiusculas): ", termo, sizeof(termo));
        for (int i = 0; i < totalComputadores; i++) {
            if (computadores[i].ativo && strstr(computadores[i].nome, termo) != NULL) {
                mostrarComputador(i); encontrados++;
            }
        }
    }
    if (!encontrados) printf("Nenhum registro encontrado.\n");
}

void editarComputador(void) {
    int pos = buscarIndiceComputador(lerInteiro("ID para editar: ", 1, 999999));
    struct Computador anterior, novo;
    if (pos < 0 || !computadores[pos].ativo) { printf("Registro nao encontrado.\n"); return; }
    anterior = computadores[pos];
    novo = anterior;
    lerTexto("Novo nome: ", novo.nome, sizeof(novo.nome));
    novo.precoHora = lerReal("Preco por hora (R$): ", 0.01f, 10000);
    computadores[pos] = novo;
    if (!salvarComputadoresCSV()) { computadores[pos] = anterior; printf("Edicao nao confirmada. Confira o CSV.\n"); exit(1); }
    printf("Edicao salva.\n");
}

void removerComputador(void) {
    int id = lerInteiro("ID para remover: ", 1, 999999);
    int pos = buscarIndiceComputador(id);
    if (pos < 0 || !computadores[pos].ativo) { printf("Registro nao encontrado.\n"); return; }
    if (computadorTemSessaoAberta(id)) {
        printf("Nao pode remover: existe uma sessao em andamento.\n"); return;
    }
    if (lerInteiro("Confirma remocao? 1 - Sim | 0 - Nao: ", 0, 1) == 0) return;
    computadores[pos].ativo = 0;
    if (!salvarComputadoresCSV()) { computadores[pos].ativo = 1; printf("Remocao nao confirmada. Confira o CSV.\n"); exit(1); }
    printf("Registro removido.\n");
}

// SUBMENU
void moduloComputadores(void) {
    int opcao;
    do {
        limparTela();
        printf("\n===== COMPUTADORES =====\n");
        printf("1 - Cadastrar\n2 - Listar\n3 - Editar\n4 - Remover\n5 - Buscar\n0 - Voltar\n");
        opcao = lerInteiro("Escolha: ", 0, 5);

        switch (opcao) {
            case 1: cadastrarComputador(); break;
            case 2: listarComputadores(); break;
            case 3: editarComputador(); break;
            case 4: removerComputador(); break;

            case 5: buscarComputadores();
        }
        if (opcao != 0) pausar();
    } while (opcao != 0);
}

#endif
