// ===================================================
// MODULO 4 - SESSOES E RELATORIOS
// Autor: Lucas Henrique Santos Pedrette
// ===================================================

#ifndef SESSOES_H
#define SESSOES_H

#include <stdio.h>
#include <string.h>
#include "clientes.h"
#include "computadores.h"
#include "produtos.h"

// STRUCT E VETOR GLOBAL
struct Sessao {
    int id;
    int idCliente;
    int idComputador;
    char data[11];
    float horas;
    float valorTotal;
    int ativo;

    float precoHoraAplicado;
    int idProduto; // 0 = sem produto
    int quantidadeProduto;
    float precoProdutoAplicado;
    int encerrada; // 0 = em andamento, 1 = encerrada
};
struct Sessao sessoes[MAX_REGISTROS];
int totalSessoes = 0;

// FUNCOES
int buscarIndiceSessao(int id) {
    int i;
    for (i = 0; i < totalSessoes; i++) {
        if (sessoes[i].id == id) return i;
    }
    return -1;
}

int clienteTemSessaoAberta(int id) {
    int i;
    for (i = 0; i < totalSessoes; i++) {
        if (sessoes[i].ativo && !sessoes[i].encerrada && sessoes[i].idCliente == id) return 1;
    }
    return 0;
}

int computadorTemSessaoAberta(int id) {
    int i;
    for (i = 0; i < totalSessoes; i++) {
        if (sessoes[i].ativo && !sessoes[i].encerrada && sessoes[i].idComputador == id) return 1;
    }
    return 0;
}

void atualizarDisponibilidade(void) {
    int i;
    for (i = 0; i < totalComputadores; i++) {
        computadores[i].disponivel = !computadorTemSessaoAberta(computadores[i].id);
    }
}

float calcularValorSessao(struct Sessao item) {
    float valor = item.horas * item.precoHoraAplicado +
                  item.quantidadeProduto * item.precoProdutoAplicado;

    return arredondarValor(valor);
}

// Regrava os registros do vetor no arquivo CSV.
int salvarSessoesCSV(void) {
    int i;
    FILE *arq = fopen("sessoes.csv", "w");
    int ok = 1;
    if (arq == NULL) { printf("[ERRO] Nao foi possivel salvar sessoes.csv.\n"); return 0; }
    for (i = 0; i < totalSessoes; i++) {
        if (fprintf(arq, "%d;%d;%d;%s;%.2f;%.2f;%d;%.2f;%d;%d;%.2f;%d\n",
                    sessoes[i].id, sessoes[i].idCliente, sessoes[i].idComputador,
                    sessoes[i].data, sessoes[i].horas, sessoes[i].valorTotal,
                    sessoes[i].ativo, sessoes[i].precoHoraAplicado, sessoes[i].idProduto,
                    sessoes[i].quantidadeProduto, sessoes[i].precoProdutoAplicado,
                    sessoes[i].encerrada) < 0) ok = 0;
    }
    if (fclose(arq) != 0) ok = 0;
    if (!ok) printf("[ERRO] Falha na gravacao de sessoes.csv.\n");
    return ok;
}

// Carrega os registros do CSV para o vetor.
int carregarSessoesCSV(void) {
    FILE *arq = fopen("sessoes.csv", "r");
    struct Sessao item;
    int lidos, numeroLinha = 0;
    totalSessoes = 0;
    if (arq == NULL) {

        return 1;
    }
    while ((lidos = fscanf(arq, "%d;%d;%d;%10[^;\r\n];%f;%f;%d;%f;%d;%d;%f;%d\n",
                       &item.id, &item.idCliente, &item.idComputador, item.data,
                       &item.horas, &item.valorTotal, &item.ativo, &item.precoHoraAplicado,
                       &item.idProduto, &item.quantidadeProduto, &item.precoProdutoAplicado,
                       &item.encerrada)) == 12) {
        numeroLinha++;
        if (totalSessoes >= MAX_REGISTROS || item.id <= 0 ||
            buscarIndiceSessao(item.id) != -1 || !dataValida(item.data) ||
            !(item.horas >= 0.01f && item.horas <= 24) ||
            !(item.precoHoraAplicado >= 0.01f && item.precoHoraAplicado <= 10000) ||
            !(item.precoProdutoAplicado >= 0 && item.precoProdutoAplicado <= 10000) ||
            !(item.valorTotal >= 0 && item.valorTotal <= 10240000) ||
            (item.ativo != 0 && item.ativo != 1) || (item.encerrada != 0 && item.encerrada != 1) ||
            item.quantidadeProduto < 0 || item.quantidadeProduto > 1000 || item.idProduto < 0 ||
            (item.idProduto == 0 && (item.quantidadeProduto != 0 || item.precoProdutoAplicado != 0)) ||
            (item.idProduto > 0 && (item.quantidadeProduto == 0 || item.precoProdutoAplicado < 0.01f)) ||
            (item.valorTotal < calcularValorSessao(item) - 0.02f ||
             item.valorTotal > calcularValorSessao(item) + 0.02f)) {
            printf("[ERRO] sessoes.csv: registro %d invalido.\n", numeroLinha);
            fclose(arq); return 0;
        }
        sessoes[totalSessoes] = item;
        totalSessoes++;
    }

    if (lidos != EOF || ferror(arq)) {
        printf("[ERRO] sessoes.csv: formato incorreto.\n");
        fclose(arq); return 0;
    }
    return fclose(arq) == 0;
}

int validarRelacionamentos(void) {
    int i, j;
    for (i = 0; i < totalSessoes; i++) {
        int cli = buscarIndiceCliente(sessoes[i].idCliente);
        int pc = buscarIndiceComputador(sessoes[i].idComputador);
        int prod = buscarIndiceProduto(sessoes[i].idProduto);
        if (cli < 0 || pc < 0 || (sessoes[i].idProduto > 0 && prod < 0)) {
            printf("[ERRO] Sessao %d referencia cadastro inexistente.\n", sessoes[i].id); return 0;
        }
        if (sessoes[i].ativo && !sessoes[i].encerrada) {
            if (!clientes[cli].ativo || !computadores[pc].ativo) {
                printf("[ERRO] Sessao aberta %d usa cadastro inativo.\n", sessoes[i].id); return 0;
            }
            for (j = 0; j < i; j++) {
                if (sessoes[j].ativo && !sessoes[j].encerrada &&
                    (sessoes[j].idComputador == sessoes[i].idComputador ||
                     sessoes[j].idCliente == sessoes[i].idCliente)) {
                    printf("[ERRO] Duas sessoes abertas para o mesmo cliente ou computador.\n"); return 0;
                }
            }
        }
    }
    atualizarDisponibilidade();
    return 1;
}

int salvarMovimentacao(void) {
    int a = salvarSessoesCSV();
    int b = salvarComputadoresCSV();
    int c = salvarProdutosCSV();
    if (!a || !b || !c) {
        printf("[ERRO] Falha de persistencia. Pare e confira os CSVs antes de continuar.\n");
        return 0;
    }
    return 1;
}

void mostrarSessao(int pos) {
    int cli = buscarIndiceCliente(sessoes[pos].idCliente);
    int pc = buscarIndiceComputador(sessoes[pos].idComputador);
    int prod = buscarIndiceProduto(sessoes[pos].idProduto);
    printf("ID: %d | Cliente: %s | PC: %s | %s | %.2f h | R$ %.2f | %s\n",
           sessoes[pos].id, clientes[cli].nome, computadores[pc].nome,
           sessoes[pos].data, sessoes[pos].horas, sessoes[pos].valorTotal,
           sessoes[pos].encerrada ? "Encerrada" : "Em andamento");
    if (prod >= 0) printf("  Produto: %s | Quantidade: %d | Preco aplicado: R$ %.2f\n",
                          produtos[prod].nome, sessoes[pos].quantidadeProduto,
                          sessoes[pos].precoProdutoAplicado);
}

void cadastrarSessao(void) {
    struct Sessao nova = {0};
    int cli, pc, prod = -1;
    if (totalSessoes >= MAX_REGISTROS) { printf("Limite de 50 sessoes atingido.\n"); return; }
    nova.id = lerInteiro("ID da sessao: ", 1, 999999);
    if (buscarIndiceSessao(nova.id) != -1) { printf("ID ja cadastrado.\n"); return; }
    nova.idCliente = lerInteiro("ID do cliente: ", 1, 999999);
    cli = buscarIndiceCliente(nova.idCliente);
    if (cli < 0 || !clientes[cli].ativo) { printf("Cliente inexistente ou inativo.\n"); return; }
    if (clienteTemSessaoAberta(nova.idCliente)) { printf("Cliente ja tem sessao em andamento.\n"); return; }
    nova.idComputador = lerInteiro("ID do computador: ", 1, 999999);
    pc = buscarIndiceComputador(nova.idComputador);
    if (pc < 0 || !computadores[pc].ativo || !computadores[pc].disponivel) {
        printf("Computador inexistente, inativo ou ocupado.\n"); return;
    }
    lerData(nova.data);
    nova.horas = lerReal("Horas contratadas (ex.: 1.50): ", 0.01f, 24);
    nova.horas = arredondarValor(nova.horas);
    nova.precoHoraAplicado = computadores[pc].precoHora;
    nova.idProduto = lerInteiro("ID do produto (0 = sem produto): ", 0, 999999);
    if (nova.idProduto > 0) {
        prod = buscarIndiceProduto(nova.idProduto);
        if (prod < 0 || !produtos[prod].ativo) { printf("Produto inexistente ou inativo.\n"); return; }
        nova.quantidadeProduto = lerInteiro("Quantidade comprada (maximo 1000): ", 1, 1000);
        if (produtos[prod].quantidade < nova.quantidadeProduto) { printf("Estoque insuficiente.\n"); return; }
        nova.precoProdutoAplicado = produtos[prod].preco;
    }
    nova.ativo = 1;
    nova.encerrada = 0;
    nova.valorTotal = calcularValorSessao(nova);
    printf("Total calculado: R$ %.2f\n", nova.valorTotal);
    if (!lerInteiro("Confirmar? 1 - Sim | 0 - Nao: ", 0, 1)) return;
    sessoes[totalSessoes++] = nova;
    if (prod >= 0) produtos[prod].quantidade -= nova.quantidadeProduto;
    atualizarDisponibilidade();
    if (!salvarMovimentacao()) exit(1);
    printf("Sessao salva. Computador ocupado e estoque atualizado.\n");
}

void listarSessoes(void) {
    int i;
    int encontrados = 0;
    for (i = 0; i < totalSessoes; i++) {
        if (sessoes[i].ativo) { mostrarSessao(i); encontrados++; }
    }
    if (!encontrados) printf("Nenhuma sessao ativa no historico.\n");
}

void buscarSessoes(void) {
    int i;
    int modo = lerInteiro("1 - ID da sessao | 2 - Nome do cliente: ", 1, 2);
    int pos, cli, encontrados = 0;
    char termo[50];
    if (modo == 1) {
        pos = buscarIndiceSessao(lerInteiro("ID: ", 1, 999999));
        if (pos >= 0 && sessoes[pos].ativo) { mostrarSessao(pos); return; }
    } else {
        lerTexto("Trecho do nome (respeite maiusculas): ", termo, sizeof(termo));
        for (i = 0; i < totalSessoes; i++) {
            cli = buscarIndiceCliente(sessoes[i].idCliente);
            if (sessoes[i].ativo && strstr(clientes[cli].nome, termo) != NULL) {
                mostrarSessao(i); encontrados++;
            }
        }
    }
    if (!encontrados) printf("Sessao nao encontrada.\n");
}

void editarSessao(void) {
    int pos = buscarIndiceSessao(lerInteiro("ID para editar: ", 1, 999999));
    struct Sessao anterior;
    if (pos < 0 || !sessoes[pos].ativo) { printf("Sessao nao encontrada.\n"); return; }
    anterior = sessoes[pos];
    lerData(sessoes[pos].data);
    sessoes[pos].horas = lerReal("Nova duracao (horas): ", 0.01f, 24);
    sessoes[pos].horas = arredondarValor(sessoes[pos].horas);
    sessoes[pos].valorTotal = calcularValorSessao(sessoes[pos]);
    if (!salvarSessoesCSV()) { sessoes[pos] = anterior; exit(1); }
    printf("Sessao editada. Total: R$ %.2f\n", sessoes[pos].valorTotal);
}

void removerSessao(void) {
    int pos = buscarIndiceSessao(lerInteiro("ID para cancelar: ", 1, 999999));
    int prod;
    if (pos < 0 || !sessoes[pos].ativo) { printf("Sessao nao encontrada.\n"); return; }
    prod = buscarIndiceProduto(sessoes[pos].idProduto);
    if (prod >= 0 && produtos[prod].quantidade > 1000000 - sessoes[pos].quantidadeProduto) {
        printf("Estorno excederia o limite do estoque. Ajuste o estoque antes.\n"); return;
    }
    if (!lerInteiro("Cancelar e devolver os produtos? 1 - Sim | 0 - Nao: ", 0, 1)) return;
    sessoes[pos].ativo = 0;
    if (prod >= 0) produtos[prod].quantidade += sessoes[pos].quantidadeProduto;
    atualizarDisponibilidade();
    if (!salvarMovimentacao()) exit(1);
    printf("Sessao cancelada. Produtos devolvidos e computador liberado se nao houver outra sessao.\n");
}

void encerrarSessao(void) {
    int pos = buscarIndiceSessao(lerInteiro("ID para encerrar: ", 1, 999999));
    if (pos < 0 || !sessoes[pos].ativo || sessoes[pos].encerrada) {
        printf("Sessao nao esta em andamento.\n"); return;
    }
    sessoes[pos].encerrada = 1;
    atualizarDisponibilidade();
    if (!salvarMovimentacao()) exit(1);
    printf("Sessao encerrada. Computador liberado; valor mantido no historico.\n");
}

// MATRIZ CLIENTES X COMPUTADORES
void montarMatrizUso(int matriz[MAX_REGISTROS][MAX_REGISTROS]) {
    int i, j;
    for (i = 0; i < MAX_REGISTROS; i++) {
        for (j = 0; j < MAX_REGISTROS; j++) matriz[i][j] = 0;
    }
    for (i = 0; i < totalSessoes; i++) {
        if (sessoes[i].ativo) {
            int cli = buscarIndiceCliente(sessoes[i].idCliente);
            int pc = buscarIndiceComputador(sessoes[i].idComputador);
            matriz[cli][pc]++;
        }
    }
}

// ORDENACAO DOS RANKINGS - BUBBLE SORT
void ordenarRanking(int indices[], int totais[], int quantidade) {
    int i, j, auxiliar;
    for (i = 0; i < quantidade - 1; i++) {
        for (j = 0; j < quantidade - 1 - i; j++) {
            if (totais[j] < totais[j + 1]) {
                auxiliar = totais[j];
                totais[j] = totais[j + 1];
                totais[j + 1] = auxiliar;
                auxiliar = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = auxiliar;
            }
        }
    }
}

void relatorioComputadores(void) {
    int matriz[MAX_REGISTROS][MAX_REGISTROS];
    int totais[MAX_REGISTROS] = {0}, indices[MAX_REGISTROS];
    int i, j;
    montarMatrizUso(matriz);
    for (j = 0; j < totalComputadores; j++) {
        indices[j] = j;
        for (i = 0; i < totalClientes; i++) totais[j] += matriz[i][j];
    }
    ordenarRanking(indices, totais, totalComputadores);
    printf("RANKING DE COMPUTADORES (BUBBLE SORT)\n");
    for (j = 0; j < totalComputadores; j++) {
        printf("%d. %s - %d sessoes\n", j + 1, computadores[indices[j]].nome, totais[j]);
    }
    if (totalComputadores == 0 || totais[0] == 0) {
        printf("Sem movimentacao valida.\n"); return;
    }
    printf("Mais utilizado(s), incluindo empates:\n");
    for (j = 0; j < totalComputadores; j++) {
        if (totais[j] == totais[0]) printf("%s - %d sessoes\n", computadores[indices[j]].nome, totais[j]);
    }
}

void relatorioClientes(void) {
    int matriz[MAX_REGISTROS][MAX_REGISTROS];
    int totais[MAX_REGISTROS] = {0}, indices[MAX_REGISTROS];
    int i, j;
    montarMatrizUso(matriz);
    for (i = 0; i < totalClientes; i++) {
        indices[i] = i;
        for (j = 0; j < totalComputadores; j++) totais[i] += matriz[i][j];
    }
    ordenarRanking(indices, totais, totalClientes);
    printf("RANKING DE CLIENTES (BUBBLE SORT)\n");
    for (i = 0; i < totalClientes; i++) {
        printf("%d. %s - %d sessoes\n", i + 1, clientes[indices[i]].nome, totais[i]);
    }
    if (totalClientes == 0 || totais[0] == 0) {
        printf("Sem movimentacao valida.\n"); return;
    }
    printf("Cliente(s) VIP, incluindo empates:\n");
    for (i = 0; i < totalClientes; i++) {
        if (totais[i] == totais[0]) printf("%s - %d sessoes\n", clientes[indices[i]].nome, totais[i]);
    }
}

void relatorioFaturamento(void) {
    int i;
    double total = 0, uso = 0, vendas = 0;
    int quantidade = 0;
    for (i = 0; i < totalSessoes; i++) {
        if (sessoes[i].ativo) {
            total += sessoes[i].valorTotal;
            vendas += sessoes[i].quantidadeProduto * sessoes[i].precoProdutoAplicado;
            quantidade++;
        }
    }
    uso = total - vendas;
    printf("Sessoes validas: %d\nUso dos PCs: R$ %.2f\nProdutos: R$ %.2f\nTotal contratado: R$ %.2f\n",
           quantidade, uso, vendas, total);
    printf("Inclui sessoes em andamento e encerradas; exclui canceladas.\n");
}

void exibirMatrizUso(void) {
    int j, i;
    int matriz[MAX_REGISTROS][MAX_REGISTROS], totalLinha, totalColuna;
    montarMatrizUso(matriz);
    printf("\nMATRIZ CLIENTE x COMPUTADOR - NUMERO DE SESSOES VALIDAS\n");
    printf("Cliente/PC  ");
    for (j = 0; j < totalComputadores; j++) printf(" %5d", computadores[j].id);
    printf(" | Total\n");
    for (i = 0; i < totalClientes; i++) {
        totalLinha = 0;
        printf("%10d  ", clientes[i].id);
        for (j = 0; j < totalComputadores; j++) {
            printf(" %5d", matriz[i][j]);
            totalLinha += matriz[i][j];
        }
        printf(" | %d\n", totalLinha);
    }
    printf("Totais      ");
    for (j = 0; j < totalComputadores; j++) {
        totalColuna = 0;
        for (i = 0; i < totalClientes; i++) totalColuna += matriz[i][j];
        printf(" %5d", totalColuna);
    }
    printf("\nLegenda dos IDs:\n");
    for (i = 0; i < totalClientes; i++) printf("Cliente %d: %s\n", clientes[i].id, clientes[i].nome);
    for (j = 0; j < totalComputadores; j++) printf("PC %d: %s\n", computadores[j].id, computadores[j].nome);
}

void relatorioProdutos(void) {
    int totais[MAX_REGISTROS] = {0}, indices[MAX_REGISTROS];
    int i, prod;
    for (i = 0; i < totalProdutos; i++) indices[i] = i;
    for (i = 0; i < totalSessoes; i++) {
        if (sessoes[i].ativo && sessoes[i].idProduto > 0) {
            prod = buscarIndiceProduto(sessoes[i].idProduto);
            totais[prod] += sessoes[i].quantidadeProduto;
        }
    }
    ordenarRanking(indices, totais, totalProdutos);
    printf("RANKING DE PRODUTOS (BUBBLE SORT)\n");
    for (i = 0; i < totalProdutos; i++) {
        printf("%d. %s - %d unidades\n", i + 1, produtos[indices[i]].nome, totais[i]);
    }
    if (totalProdutos == 0 || totais[0] == 0) {
        printf("Sem venda valida de produtos.\n"); return;
    }
    printf("Produto(s) de maior demanda:\n");
    for (i = 0; i < totalProdutos; i++) {
        if (totais[i] == totais[0]) printf("%s - %d unidades\n", produtos[indices[i]].nome, totais[i]);
    }
}

// SUBMENU
void moduloSessoes(void) {
    int opcao;
    do {
        limparTela();
        printf("\n===== SESSOES E RELATORIOS =====\n");
        printf("1 - Registrar sessao\n2 - Listar sessoes\n3 - Editar sessao\n4 - Cancelar sessao\n");
        printf("5 - Computadores mais usados\n6 - Clientes mais frequentes\n7 - Faturamento\n");
        printf("8 - Matriz cliente x computador\n9 - Encerrar sessao\n10 - Buscar sessao\n");
        printf("11 - Produtos mais vendidos\n0 - Voltar\n");
        opcao = lerInteiro("Escolha: ", 0, 11);

        switch (opcao) {
            case 1: cadastrarSessao(); break;
            case 2: listarSessoes(); break;
            case 3: editarSessao(); break;
            case 4: removerSessao(); break;
            case 5: relatorioComputadores(); break;
            case 6: relatorioClientes(); break;
            case 7: relatorioFaturamento(); break;
            case 8: exibirMatrizUso(); break;
            case 9: encerrarSessao(); break;
            case 10: buscarSessoes(); break;

            case 11: relatorioProdutos();
        }
        if (opcao != 0) pausar();
    } while (opcao != 0);
}

#endif
