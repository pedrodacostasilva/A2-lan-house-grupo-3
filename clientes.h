// ===================================================
// MODULO 1 - CLIENTES
// Autor: Miguel Otavio Parreira Medeiros De Oliveira
// ===================================================

#ifndef CLIENTES_H
#define CLIENTES_H

#include <stdio.h>
#include <string.h>

// STRUCT E VETOR GLOBAL
struct Cliente {
    int id;
    char nome[50];
    char telefone[20];
    float saldo;
    int ativo;
};
struct Cliente clientes[MAX_REGISTROS];
int totalClientes = 0;

// FUNCOES
int buscarIndiceCliente(int id) {
    int i;
    for (i = 0; i < totalClientes; i++) {
        if (clientes[i].id == id) return i;
    }
    return -1;
}

// Regrava os registros do vetor no arquivo CSV.
int salvarClientesCSV(void) {
    int i;
    FILE *arq = fopen("clientes.csv", "w");
    int ok = 1;
    if (arq == NULL) { printf("[ERRO] Nao foi possivel abrir clientes.csv para salvar.\n"); return 0; }
    for (i = 0; i < totalClientes; i++) {
        if (fprintf(arq, "%d;%s;%s;%.2f;%d\n", clientes[i].id, clientes[i].nome, clientes[i].telefone, clientes[i].saldo, clientes[i].ativo) < 0) ok = 0;
    }
    if (fclose(arq) != 0) ok = 0;
    if (!ok) printf("[ERRO] Falha ao gravar clientes.csv. Verifique o disco.\n");
    return ok;
}

// Carrega os registros do CSV para o vetor.
int carregarClientesCSV(void) {
    FILE *arq = fopen("clientes.csv", "r");
    struct Cliente item;
    int lidos, numeroLinha = 0;
    totalClientes = 0;
    if (arq == NULL) {

        return 1;
    }
    while ((lidos = fscanf(arq, "%d;%49[^;\r\n];%19[^;\r\n];%f;%d\n", &item.id, item.nome, item.telefone, &item.saldo, &item.ativo)) == 5) {
        numeroLinha++;
        if (totalClientes >= MAX_REGISTROS || item.id <= 0 ||
            (item.ativo != 0 && item.ativo != 1) ||
            !(item.saldo >= 0 && item.saldo <= 1000000) || buscarIndiceCliente(item.id) != -1) {
            printf("[ERRO] clientes.csv: registro %d invalido ou limite excedido.\n", numeroLinha);
            fclose(arq); return 0;
        }
        clientes[totalClientes] = item;
        totalClientes++;
    }

    if (lidos != EOF || ferror(arq)) {
        printf("[ERRO] clientes.csv: formato incorreto.\n");
        fclose(arq); return 0;
    }
    return fclose(arq) == 0;
}

void mostrarCliente(int pos) {
    printf("ID: %d | %-25s | Tel: %s | Saldo: R$ %.2f\n",
           clientes[pos].id, clientes[pos].nome, clientes[pos].telefone, clientes[pos].saldo);
}

void cadastrarCliente(void) {
    struct Cliente novo;
    if (totalClientes >= MAX_REGISTROS) { printf("Limite de 50 registros atingido.\n"); return; }
    novo.id = lerInteiro("ID: ", 1, 999999);
    if (buscarIndiceCliente(novo.id) != -1) { printf("ID ja cadastrado (mesmo se inativo).\n"); return; }
    lerTexto("Nome: ", novo.nome, sizeof(novo.nome));
    lerTexto("Telefone: ", novo.telefone, sizeof(novo.telefone));
    novo.saldo = lerReal("Saldo (R$): ", 0, 1000000);

    novo.ativo = 1;
    clientes[totalClientes] = novo;
    totalClientes++;
    if (!salvarClientesCSV()) { totalClientes--; printf("Cadastro nao confirmado. Confira o CSV antes de continuar.\n"); exit(1); }
    printf("Cadastro salvo.\n");
}

void listarClientes(void) {
    int i;
    int encontrados = 0;
    for (i = 0; i < totalClientes; i++) {
        if (clientes[i].ativo) { mostrarCliente(i); encontrados++; }
    }
    if (!encontrados) printf("Nenhum registro ativo.\n");
}

void buscarClientes(void) {
    int i;
    int modo = lerInteiro("1 - Por ID | 2 - Por parte do nome: ", 1, 2);
    int pos, encontrados = 0;
    char termo[50];
    if (modo == 1) {
        pos = buscarIndiceCliente(lerInteiro("ID: ", 1, 999999));
        if (pos >= 0 && clientes[pos].ativo) { mostrarCliente(pos); return; }
    } else {
        lerTexto("Trecho do nome (respeite maiusculas): ", termo, sizeof(termo));
        for (i = 0; i < totalClientes; i++) {
            if (clientes[i].ativo && strstr(clientes[i].nome, termo) != NULL) {
                mostrarCliente(i); encontrados++;
            }
        }
    }
    if (!encontrados) printf("Nenhum registro encontrado.\n");
}

void editarCliente(void) {
    int pos = buscarIndiceCliente(lerInteiro("ID para editar: ", 1, 999999));
    struct Cliente anterior, novo;
    if (pos < 0 || !clientes[pos].ativo) { printf("Registro nao encontrado.\n"); return; }
    anterior = clientes[pos];
    novo = anterior;
    lerTexto("Novo nome: ", novo.nome, sizeof(novo.nome));
    lerTexto("Telefone: ", novo.telefone, sizeof(novo.telefone));
    novo.saldo = lerReal("Saldo (R$): ", 0, 1000000);
    clientes[pos] = novo;
    if (!salvarClientesCSV()) { clientes[pos] = anterior; printf("Edicao nao confirmada. Confira o CSV.\n"); exit(1); }
    printf("Edicao salva.\n");
}

void removerCliente(void) {
    int id = lerInteiro("ID para remover: ", 1, 999999);
    int pos = buscarIndiceCliente(id);
    if (pos < 0 || !clientes[pos].ativo) { printf("Registro nao encontrado.\n"); return; }
    if (clienteTemSessaoAberta(id)) {
        printf("Nao pode remover: existe uma sessao em andamento.\n"); return;
    }
    if (lerInteiro("Confirma remocao? 1 - Sim | 0 - Nao: ", 0, 1) == 0) return;
    clientes[pos].ativo = 0;
    if (!salvarClientesCSV()) { clientes[pos].ativo = 1; printf("Remocao nao confirmada. Confira o CSV.\n"); exit(1); }
    printf("Registro removido.\n");
}

// SUBMENU
void moduloClientes(void) {
    int opcao;
    do {
        limparTela();
        printf("\n===== CLIENTES =====\n");
        printf("1 - Cadastrar\n2 - Listar\n3 - Editar\n4 - Remover\n5 - Buscar\n0 - Voltar\n");
        opcao = lerInteiro("Escolha: ", 0, 5);

        switch (opcao) {
            case 1: cadastrarCliente(); break;
            case 2: listarClientes(); break;
            case 3: editarCliente(); break;
            case 4: removerCliente(); break;

            case 5: buscarClientes();
        }
        if (opcao != 0) pausar();
    } while (opcao != 0);
}

#endif
