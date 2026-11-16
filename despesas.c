/*
 * Sistema de Cadastro de Despesas Pessoais
 * Arvore Binaria de Busca | Compilar: gcc -Wall -Wextra -o despesas despesas.c
 */

#include <stdio.h>
#include <stdlib.h>

#define ARQUIVO "despesas.csv"

typedef struct {
    int codigo;
    char descricao[50];
    char categoria[30];
    double valor;
} Despesa;

typedef struct No {
    Despesa dado;
    struct No *esq, *dir;
} No;

No *criar_no(Despesa d) {
    No *n = (No *) malloc(sizeof(No));
    if (n == NULL) { printf("Erro: sem memoria.\n"); exit(1); }
    n->dado = d;
    n->esq = n->dir = NULL;
    return n;
}

int inserir(No **r, Despesa d) {
    if (*r == NULL) { *r = criar_no(d); return 1; }
    if (d.codigo < (*r)->dado.codigo) return inserir(&(*r)->esq, d);
    if (d.codigo > (*r)->dado.codigo) return inserir(&(*r)->dir, d);
    return 0;
}

No *buscar(No *r, int cod) {
    if (r == NULL || r->dado.codigo == cod) return r;
    return cod < r->dado.codigo ? buscar(r->esq, cod) : buscar(r->dir, cod);
}

void listar(No *r) {
    if (r == NULL) return;
    listar(r->esq);
    printf("  %4d | %-30s | %-20s | R$ %8.2f\n",
           r->dado.codigo, r->dado.descricao, r->dado.categoria, r->dado.valor);
    listar(r->dir);
}

void exibir_arvore(No *r, int nivel, char lado) {
    int i;
    if (r == NULL) return;
    for (i = 0; i < nivel; i++) printf("    ");
    if (lado == 'R') printf("Raiz: [%d]\n", r->dado.codigo);
    else printf("%c: [%d]\n", lado, r->dado.codigo);
    exibir_arvore(r->esq, nivel + 1, 'E');
    exibir_arvore(r->dir, nivel + 1, 'D');
}

/* Operacao adicional: conta os registros na arvore */
int contar(No *r) {
    if (r == NULL) return 0;
    return 1 + contar(r->esq) + contar(r->dir);
}

/* Grava em pre-ordem: ao reler, a arvore mantem a mesma estrutura */
void gravar(No *r, FILE *f) {
    if (r == NULL) return;
    fprintf(f, "%d;%s;%s;%.2f\n", r->dado.codigo, r->dado.descricao,
            r->dado.categoria, r->dado.valor);
    gravar(r->esq, f);
    gravar(r->dir, f);
}

void destruir(No *r) {
    if (r == NULL) return;
    destruir(r->esq);
    destruir(r->dir);
    free(r);
}

void carregar(No **r) {
    Despesa d;
    FILE *f = fopen(ARQUIVO, "r");
    if (f == NULL) { printf("Arquivo nao encontrado. Cadastro vazio.\n\n"); return; }
    while (fscanf(f, "%d;%49[^;];%29[^;];%lf\n", &d.codigo, d.descricao,
                  d.categoria, &d.valor) == 4) {
        inserir(r, d);
    }
    fclose(f);
    printf("Dados carregados de '%s'.\n\n", ARQUIVO);
}

void salvar_e_sair(No **r) {
    FILE *f = fopen(ARQUIVO, "w");
    if (f == NULL) { printf("Erro ao gravar arquivo.\n"); exit(1); }
    gravar(*r, f);
    fclose(f);
    destruir(*r);
    *r = NULL;
    printf("Dados gravados. Memoria liberada. Ate logo!\n");
}

int main(void) {
    No *raiz = NULL;
    int op, cod;
    Despesa d;

    carregar(&raiz);

    do {
        printf("=== MENU ===\n");
        printf("1. Cadastrar\n");
        printf("2. Buscar\n");
        printf("3. Listar\n");
        printf("4. Exibir arvore\n");
        printf("5. Contar registros\n");
        printf("0. Sair\n");
        printf("Opcao: ");
        if (scanf("%d", &op) != 1) { while (getchar() != '\n'); op = -1; }

        switch (op) {
        case 1:
            printf("Codigo: "); scanf("%d", &d.codigo);
            printf("Descricao: "); scanf(" %49[^\n]", d.descricao);
            printf("Categoria: "); scanf(" %29[^\n]", d.categoria);
            printf("Valor: "); scanf("%lf", &d.valor);
            printf(inserir(&raiz, d) ? "Cadastrado com sucesso!\n\n" : "Erro: codigo repetido.\n\n");
            break;
        case 2:
            printf("Codigo: "); scanf("%d", &cod);
            {
                No *n = buscar(raiz, cod);
                if (n) printf("Encontrado: %d | %s | %s | R$ %.2f\n\n",
                              n->dado.codigo, n->dado.descricao, n->dado.categoria, n->dado.valor);
                else printf("Registro nao encontrado.\n\n");
            }
            break;
        case 3:
            if (!raiz) printf("Nenhum registro cadastrado.\n\n");
            else {
                printf("\n  Cod  | Descricao                      | Categoria            | Valor\n");
                printf("  -----+--------------------------------+----------------------+------------\n");
                listar(raiz);
                printf("\n");
            }
            break;
        case 4:
            if (!raiz) printf("Arvore vazia.\n\n");
            else {
                printf("\n");
                exibir_arvore(raiz, 0, 'R');
                printf("\n");
            }
            break;
        case 5:
            printf("Total de registros cadastrados: %d\n\n", contar(raiz));
            break;
        case 0:
            salvar_e_sair(&raiz);
            break;
        default:
            printf("Opcao invalida!\n\n");
            break;
        }
    } while (op != 0);

    return 0;
}
