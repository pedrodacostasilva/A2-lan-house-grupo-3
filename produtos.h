// ===================================================
// MODULO 3 - PRODUTOS E ESTOQUE
// Autor: Pedro Henrique da Costa Silva
// ===================================================

#ifndef PRODUTOS_H
#define PRODUTOS_H

#include <stdio.h>
#include <string.h>

// STRUCT E VETOR GLOBAL
struct Produto {
    int id;
    char nome[50];
    float preco;
    int quantidade;
    int ativo;
};
struct Produto produtos[MAX_REGISTROS];
int totalProdutos = 0;

// FUNCOES
int buscarIndiceProduto(int id) {
    int i;
    for (i = 0; i < totalProdutos; i++) {
        if (produtos[i].id == id) return i;
    }
    return -1;
}

// Regrava os registros do vetor no arquivo CSV.
int salvarProdutosCSV(void) {
    int i;
    FILE *arq = fopen("produtos.csv", "w");
    int ok = 1;
    if (arq == NULL) { printf("[ERRO] Nao foi possivel abrir produtos.csv para salvar.\n"); return 0; }
    for (i = 0; i < totalProdutos; i++) {
        if (fprintf(arq, "%d;%s;%.2f;%d;%d\n", produtos[i].id, produtos[i].nome, produtos[i].preco, produtos[i].quantidade, produtos[i].ativo) < 0) ok = 0;
    }
    if (fclose(arq) != 0) ok = 0;
    if (!ok) printf("[ERRO] Falha ao gravar produtos.csv. Verifique o disco.\n");
    return ok;
}

// Carrega os registros do CSV para o vetor.
int carregarProdutosCSV(void) {
    FILE *arq = fopen("produtos.csv", "r");
    struct Produto item;
    int lidos, numeroLinha = 0;
    totalProdutos = 0;
    if (arq == NULL) {

        return 1;
    }
    while ((lidos = fscanf(arq, "%d;%49[^;\r\n];%f;%d;%d\n", &item.id, item.nome, &item.preco, &item.quantidade, &item.ativo)) == 5) {
        numeroLinha++;
        if (totalProdutos >= MAX_REGISTROS || item.id <= 0 ||
            (item.ativo != 0 && item.ativo != 1) ||
            !(item.preco >= 0.01f && item.preco <= 10000) || item.quantidade < 0 || item.quantidade > 1000000 || buscarIndiceProduto(item.id) != -1) {
            printf("[ERRO] produtos.csv: registro %d invalido ou limite excedido.\n", numeroLinha);
            fclose(arq); return 0;
        }
        produtos[totalProdutos] = item;
        totalProdutos++;
    }

    if (lidos != EOF || ferror(arq)) {
        printf("[ERRO] produtos.csv: formato incorreto.\n");
        fclose(arq); return 0;
    }
    return fclose(arq) == 0;
}

void mostrarProduto(int pos) {
    printf("ID: %d | %-25s | R$ %.2f | Estoque: %d\n",
           produtos[pos].id, produtos[pos].nome, produtos[pos].preco, produtos[pos].quantidade);
}

void cadastrarProduto(void) {
    struct Produto novo;
    if (totalProdutos >= MAX_REGISTROS) { printf("Limite de 50 registros atingido.\n"); return; }
    novo.id = lerInteiro("ID: ", 1, 999999);
    if (buscarIndiceProduto(novo.id) != -1) { printf("ID ja cadastrado (mesmo se inativo).\n"); return; }
    lerTexto("Nome: ", novo.nome, sizeof(novo.nome));
    novo.preco = lerReal("Preco (R$): ", 0.01f, 10000);
    novo.quantidade = lerInteiro("Quantidade em estoque: ", 0, 1000000);

    novo.ativo = 1;
    produtos[totalProdutos] = novo;
    totalProdutos++;
    if (!salvarProdutosCSV()) { totalProdutos--; printf("Cadastro nao confirmado. Confira o CSV antes de continuar.\n"); exit(1); }
    printf("Cadastro salvo.\n");
}

void listarProdutos(void) {
    int i;
    int encontrados = 0;
    for (i = 0; i < totalProdutos; i++) {
        if (produtos[i].ativo) { mostrarProduto(i); encontrados++; }
    }
    if (!encontrados) printf("Nenhum registro ativo.\n");
}

void buscarProdutos(void) {
    int i;
    int modo = lerInteiro("1 - Por ID | 2 - Por parte do nome: ", 1, 2);
    int pos, encontrados = 0;
    char termo[50];
    if (modo == 1) {
        pos = buscarIndiceProduto(lerInteiro("ID: ", 1, 999999));
        if (pos >= 0 && produtos[pos].ativo) { mostrarProduto(pos); return; }
    } else {
        lerTexto("Trecho do nome (respeite maiusculas): ", termo, sizeof(termo));
        for (i = 0; i < totalProdutos; i++) {
            if (produtos[i].ativo && strstr(produtos[i].nome, termo) != NULL) {
                mostrarProduto(i); encontrados++;
            }
        }
    }
    if (!encontrados) printf("Nenhum registro encontrado.\n");
}

void editarProduto(void) {
    int pos = buscarIndiceProduto(lerInteiro("ID para editar: ", 1, 999999));
    struct Produto anterior, novo;
    if (pos < 0 || !produtos[pos].ativo) { printf("Registro nao encontrado.\n"); return; }
    anterior = produtos[pos];
    novo = anterior;
    lerTexto("Novo nome: ", novo.nome, sizeof(novo.nome));
    novo.preco = lerReal("Preco (R$): ", 0.01f, 10000);
    novo.quantidade = lerInteiro("Quantidade em estoque: ", 0, 1000000);
    produtos[pos] = novo;
    if (!salvarProdutosCSV()) { produtos[pos] = anterior; printf("Edicao nao confirmada. Confira o CSV.\n"); exit(1); }
    printf("Edicao salva.\n");
}

void removerProduto(void) {
    int id = lerInteiro("ID para remover: ", 1, 999999);
    int pos = buscarIndiceProduto(id);
    if (pos < 0 || !produtos[pos].ativo) { printf("Registro nao encontrado.\n"); return; }
    if (lerInteiro("Confirma remocao? 1 - Sim | 0 - Nao: ", 0, 1) == 0) return;
    produtos[pos].ativo = 0;
    if (!salvarProdutosCSV()) { produtos[pos].ativo = 1; printf("Remocao nao confirmada. Confira o CSV.\n"); exit(1); }
    printf("Registro removido.\n");
}

// SUBMENU
void moduloProdutos(void) {
    int opcao;
    do {
        limparTela();
        printf("\n===== PRODUTOS =====\n");
        printf("1 - Cadastrar\n2 - Listar\n3 - Editar\n4 - Remover\n5 - Buscar\n0 - Voltar\n");
        opcao = lerInteiro("Escolha: ", 0, 5);

        switch (opcao) {
            case 1: cadastrarProduto(); break;
            case 2: listarProdutos(); break;
            case 3: editarProduto(); break;
            case 4: removerProduto(); break;

            case 5: buscarProdutos();
        }
        if (opcao != 0) pausar();
    } while (opcao != 0);
}

#endif
