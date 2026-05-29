/*
 * Projeto de Programacao - Parte 2 - Panelinhas em Redes Sociais
 * Teoria dos Grafos - 2026/1
 *
 * Alunos:
 *   Joao Guilherme Messias de Oliveira Santos RA: 10426110
 *   Andre Ihsan Ward                          RA: 10425684
 *   Enzo Ponte Gamberi                        RA: 10389931
 *   Thiago Ruiz Fernandes Silva               RA: 10426057
 *   Luis Felipe Cunha                         RA: 10419514
 *
 * Descricao:
 *   Modelagem de uma rede social como um grafo nao-orientado.
 *   Cada vertice representa um aluno de uma turma de faculdade,
 *   com nome, curso e semestre. As arestas representam amizade.
 *
 * Algoritmos implementados:
 *   - exibeRede         : exibe todos os detalhes da rede social
 *   - encontraPanelinhas: encontra todos os cliques maximais (>= 3)
 *                         usando Busca em Profundidade (DFS)
 *   - consultaPanelinhas: lista as panelinhas de um participante
 *   - consultaInfluentes: lista os elementos influentes da rede
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
   Constantes
   ============================================================ */

#define MAX_NOME       50
#define MAX_CURSO      40
#define MAX_PANELINHAS 500
#define MAX_MEMBROS    35

/* ============================================================
   Estruturas de dados
   ============================================================ */

/* Celula da lista de adjacencia */
typedef struct a {
    int      vizinho;
    struct a *prox;
} Aresta;

/* Vertice: participante da rede social */
typedef struct v {
    int    id;
    char   nome[MAX_NOME];
    char   curso[MAX_CURSO];
    int    semestre;
    Aresta *prim;
} Vertice;

/* Conjunto que representa uma panelinha (clique maximal) */
typedef struct {
    int membros[MAX_MEMBROS];
    int tamanho;
} Panelinha;

/* Colecao de todas as panelinhas encontradas */
typedef struct {
    Panelinha lista[MAX_PANELINHAS];
    int       quantidade;
} ColecaoPanelinhas;

/* ============================================================
   Prototipos
   ============================================================ */

void criaGrafo          (Vertice **G, int ordem);
void destroiGrafo       (Vertice **G, int ordem);
int  acrescentaAresta   (Vertice G[], int ordem, int v1, int v2);
void exibeRede          (Vertice G[], int ordem);

void encontraPanelinhas (Vertice G[], int ordem, ColecaoPanelinhas *col);
void consultaPanelinhas (Vertice G[], int ordem,
                         ColecaoPanelinhas *col, int idParticipante);
void consultaInfluentes (Vertice G[], int ordem, ColecaoPanelinhas *col);

static void defineParticipante (Vertice G[], int id, const char *nome,
                                 const char *curso, int semestre);
static int  saoVizinhos        (Vertice G[], int v1, int v2);
static int  contem             (int V[], int tam, int x);
static int  todosVizinhos      (Vertice G[], int v, int grupo[], int tam);
static int  panelinhasIguais   (Panelinha *a, Panelinha *b);
static void dfsCliques         (Vertice G[], int ordem,
                                 int grupo[], int tamGrupo,
                                 int inicio, ColecaoPanelinhas *col);
static void intersecao         (int A[], int tamA, int B[], int tamB,
                                 int C[], int *tamC);

/* ============================================================
   Criacao e destruicao do grafo
   ============================================================ */

void criaGrafo(Vertice **G, int ordem) {
    int i;
    *G = (Vertice *) malloc(sizeof(Vertice) * ordem);
    for (i = 0; i < ordem; i++) {
        (*G)[i].id       = i;
        (*G)[i].nome[0]  = '\0';
        (*G)[i].curso[0] = '\0';
        (*G)[i].semestre = 0;
        (*G)[i].prim     = NULL;
    }
}

void destroiGrafo(Vertice **G, int ordem) {
    int i;
    Aresta *a, *n;
    for (i = 0; i < ordem; i++) {
        a = (*G)[i].prim;
        while (a != NULL) {
            n = a->prox;
            free(a);
            a = n;
        }
    }
    free(*G);
}

/* ============================================================
   Manipulacao de arestas e atributos
   ============================================================ */

static void defineParticipante(Vertice G[], int id, const char *nome,
                                const char *curso, int semestre) {
    strncpy(G[id].nome,  nome,  MAX_NOME  - 1);
    strncpy(G[id].curso, curso, MAX_CURSO - 1);
    G[id].semestre = semestre;
}

/* Acrescenta aresta nao-orientada entre v1 e v2 */
int acrescentaAresta(Vertice G[], int ordem, int v1, int v2) {
    Aresta *A1, *A2;
    if (v1 < 0 || v1 >= ordem) return 0;
    if (v2 < 0 || v2 >= ordem) return 0;

    A1          = (Aresta *) malloc(sizeof(Aresta));
    A1->vizinho = v2;
    A1->prox    = G[v1].prim;
    G[v1].prim  = A1;

    if (v1 == v2) return 1;

    A2          = (Aresta *) malloc(sizeof(Aresta));
    A2->vizinho = v1;
    A2->prox    = G[v2].prim;
    G[v2].prim  = A2;

    return 1;
}

/* ============================================================
   Exibicao da rede social
   ============================================================ */

void exibeRede(Vertice G[], int ordem) {
    int i, grau, totalArestas;
    Aresta *aux;

    /* Conta o total de arestas (cada aresta aparece duas vezes na lista) */
    totalArestas = 0;
    for (i = 0; i < ordem; i++) {
        aux = G[i].prim;
        while (aux != NULL) { totalArestas++; aux = aux->prox; }
    }
    totalArestas = totalArestas / 2;

    printf("============================================================\n");
    printf("         REDE SOCIAL - TURMA DE COMPUTACAO                  \n");
    printf("============================================================\n\n");

    printf("  Participantes : %d alunos\n", ordem);
    printf("  Amizades      : %d conexoes\n\n", totalArestas);

    printf("  PARTICIPANTES\n");
    printf("  %-4s %-22s %-28s %-8s %s\n",
           "ID", "Nome", "Curso", "Semestre", "Amigos");
    printf("  ------------------------------------------------------------------------\n");
    for (i = 0; i < ordem; i++) {
        grau = 0;
        aux  = G[i].prim;
        while (aux != NULL) { grau++; aux = aux->prox; }
        printf("  v%-3d %-22s %-28s %do       %d\n",
               i, G[i].nome, G[i].curso, G[i].semestre, grau);
    }

    printf("\n  AMIZADES (lista de adjacencia)\n");
    printf("  ------------------------------------------------------------------------\n");
    for (i = 0; i < ordem; i++) {
        printf("  %-22s ->", G[i].nome);
        aux = G[i].prim;
        if (aux == NULL) printf("  (nenhuma amizade)");
        while (aux != NULL) {
            printf("  %s", G[aux->vizinho].nome);
            aux = aux->prox;
        }
        printf("\n");
    }
    printf("\n");
}

/* ============================================================
   Funcoes auxiliares
   ============================================================ */

/* Verifica se v1 e v2 sao vizinhos */
static int saoVizinhos(Vertice G[], int v1, int v2) {
    Aresta *aux = G[v1].prim;
    while (aux != NULL) {
        if (aux->vizinho == v2) return 1;
        aux = aux->prox;
    }
    return 0;
}

/* Verifica se x esta no vetor V */
static int contem(int V[], int tam, int x) {
    int i;
    for (i = 0; i < tam; i++)
        if (V[i] == x) return 1;
    return 0;
}

/* Verifica se v eh vizinho de todos os membros do grupo */
static int todosVizinhos(Vertice G[], int v, int grupo[], int tam) {
    int i;
    for (i = 0; i < tam; i++)
        if (!saoVizinhos(G, v, grupo[i])) return 0;
    return 1;
}

/* Verifica se duas panelinhas tem os mesmos membros */
static int panelinhasIguais(Panelinha *a, Panelinha *b) {
    int i;
    if (a->tamanho != b->tamanho) return 0;
    for (i = 0; i < a->tamanho; i++)
        if (!contem(b->membros, b->tamanho, a->membros[i])) return 0;
    return 1;
}

/* Calcula a intersecao entre dois vetores */
static void intersecao(int A[], int tamA, int B[], int tamB,
                        int C[], int *tamC) {
    int i;
    *tamC = 0;
    for (i = 0; i < tamA; i++)
        if (contem(B, tamB, A[i]))
            C[(*tamC)++] = A[i];
}

/* ============================================================
   Algoritmo de busca de panelinhas usando DFS
   ============================================================

   A ideia central e a seguinte:
   Partimos de um grupo inicial com um vertice e tentamos
   expandi-lo adicionando novos vertices via DFS.

   Um vertice pode entrar no grupo apenas se for vizinho de
   TODOS os membros ja presentes — assim garantimos que o
   grupo continua sendo um clique (panelinha em formacao).

   Para evitar duplicatas, so consideramos vertices com indice
   maior que o ultimo adicionado (ordem crescente).

   Quando nao ha mais ninguem para adicionar ao grupo, ele e
   maximal. Se tiver 3 ou mais membros, e uma panelinha valida.
   Verificamos tambem se ja nao foi registrada antes de salvar.
   ============================================================ */

static void dfsCliques(Vertice G[], int ordem,
                        int grupo[], int tamGrupo,
                        int inicio, ColecaoPanelinhas *col) {
    int v, expandiu, i;
    Panelinha nova;

    expandiu = 0;

    /* Tenta expandir o grupo com vertices de indice >= inicio */
    for (v = inicio; v < ordem; v++) {
        if (todosVizinhos(G, v, grupo, tamGrupo)) {
            grupo[tamGrupo] = v;
            dfsCliques(G, ordem, grupo, tamGrupo + 1, v + 1, col);
            expandiu = 1;
        }
    }

    /*
     * Maximalidade: verifica se existe algum vertice FORA do grupo
     * (com indice menor que inicio, portanto ainda nao tentado nesta
     * ramificacao) que seja vizinho de todos no grupo.
     * Se existir, este grupo nao e maximal — sera coberto por outro
     * ramo da DFS que partiu de um inicio menor.
     */
    if (!expandiu && tamGrupo >= 3) {
        int ehMaximal = 1;
        for (v = 0; v < inicio && ehMaximal; v++) {
            if (!contem(grupo, tamGrupo, v) &&
                todosVizinhos(G, v, grupo, tamGrupo))
                ehMaximal = 0;
        }

        if (ehMaximal && col->quantidade < MAX_PANELINHAS) {
            for (i = 0; i < tamGrupo; i++)
                nova.membros[i] = grupo[i];
            nova.tamanho = tamGrupo;

            /* Checa duplicata */
            for (i = 0; i < col->quantidade; i++)
                if (panelinhasIguais(&col->lista[i], &nova)) return;

            col->lista[col->quantidade] = nova;
            col->quantidade++;
        }
    }
}

/*
 * Percorre todos os vertices do grafo como ponto de partida
 * e usa DFS para encontrar todos os cliques maximais.
 */
void encontraPanelinhas(Vertice G[], int ordem, ColecaoPanelinhas *col) {
    int v;
    int grupo[MAX_MEMBROS];

    col->quantidade = 0;

    for (v = 0; v < ordem; v++) {
        grupo[0] = v;
        dfsCliques(G, ordem, grupo, 1, v + 1, col);
    }
}

/* ============================================================
   Consultas sobre panelinhas
   ============================================================ */

/*
 * Lista todas as panelinhas de um participante.
 */
void consultaPanelinhas(Vertice G[], int ordem,
                        ColecaoPanelinhas *col, int idParticipante) {
    int i, j, qtd;

    if (idParticipante < 0 || idParticipante >= ordem) {
        printf("  Participante invalido.\n");
        return;
    }

    printf("============================================================\n");
    printf("  PANELINHAS DE: %s\n", G[idParticipante].nome);
    printf("============================================================\n");

    qtd = 0;
    for (i = 0; i < col->quantidade; i++) {
        if (contem(col->lista[i].membros, col->lista[i].tamanho, idParticipante)) {
            qtd++;
            printf("  Panelinha %d (%d membros): ", qtd, col->lista[i].tamanho);
            for (j = 0; j < col->lista[i].tamanho; j++) {
                printf("%s", G[col->lista[i].membros[j]].nome);
                if (j < col->lista[i].tamanho - 1) printf(", ");
            }
            printf("\n");
        }
    }

    if (qtd == 0)
        printf("  %s nao faz parte de nenhuma panelinha.\n",
               G[idParticipante].nome);

    printf("\n");
}

/*
 * Lista os elementos influentes da rede social.
 *
 * Um elemento Psi eh influente se existem panelinhas alfa e beta
 * tais que alfa inter beta = {Psi}.
 *
 * Para cada influente encontrado, exibe tambem as duas panelinhas
 * que comprovam sua influencia.
 */
void consultaInfluentes(Vertice G[], int ordem, ColecaoPanelinhas *col) {
    int i, j, k, m, influente, qtdInf;
    int inter[MAX_MEMBROS], tamInter;
    int pA, pB; /* indices das panelinhas que comprovam a influencia */

    printf("============================================================\n");
    printf("  ELEMENTOS INFLUENTES DA REDE SOCIAL                       \n");
    printf("============================================================\n");
    printf("  (Psi eh influente se existem panelinhas alfa e beta\n");
    printf("   com alfa inter beta = {Psi})\n\n");

    qtdInf = 0;

    for (k = 0; k < ordem; k++) {
        influente = 0;
        pA = -1;
        pB = -1;

        for (i = 0; i < col->quantidade && !influente; i++) {
            if (!contem(col->lista[i].membros, col->lista[i].tamanho, k))
                continue;

            for (j = i + 1; j < col->quantidade && !influente; j++) {
                if (!contem(col->lista[j].membros, col->lista[j].tamanho, k))
                    continue;

                intersecao(col->lista[i].membros, col->lista[i].tamanho,
                           col->lista[j].membros, col->lista[j].tamanho,
                           inter, &tamInter);

                if (tamInter == 1 && inter[0] == k) {
                    influente = 1;
                    pA = i;
                    pB = j;
                }
            }
        }

        if (influente) {
            qtdInf++;
            printf("  -> %s (%s, %do semestre)\n",
                   G[k].nome, G[k].curso, G[k].semestre);

            /* Exibe a panelinha alfa */
            printf("     Panelinha alfa: {");
            for (m = 0; m < col->lista[pA].tamanho; m++) {
                printf("%s", G[col->lista[pA].membros[m]].nome);
                if (m < col->lista[pA].tamanho - 1) printf(", ");
            }
            printf("}\n");

            /* Exibe a panelinha beta */
            printf("     Panelinha beta: {");
            for (m = 0; m < col->lista[pB].tamanho; m++) {
                printf("%s", G[col->lista[pB].membros[m]].nome);
                if (m < col->lista[pB].tamanho - 1) printf(", ");
            }
            printf("}\n");

            printf("     Intersecao alfa e beta: {%s}\n\n", G[k].nome);
        }
    }

    if (qtdInf == 0)
        printf("  Nao ha elementos influentes na rede.\n");

    printf("  Total de influentes: %d\n\n", qtdInf);
}

/* ============================================================
   Funcao principal
   ============================================================ */

int main(void) {
    Vertice *G;
    int ordemG;
    ColecaoPanelinhas col;
    int i, j;

    /*
     * Grafo com 28 participantes (ordem entre 25 e 35).
     * Rede social de uma turma de Ciencia da Computacao.
     *
     * Panelinhas planejadas:
     *   A: {0,1,2,3}        Ana, Bruno, Carla, Diego
     *   B: {1,2,4}          Bruno, Carla, Elena
     *   C: {5,6,7,8}        Felipe, Gabriela, Heitor, Isabela
     *   D: {7,8,9}          Heitor, Isabela, Jorge
     *   E: {10,11,12,13}    Karen, Lucas, Marina, Nicolas
     *   F: {12,13,14}       Marina, Nicolas, Olivia
     *   G: {15,16,17}       Pedro, Quezia, Rafael
     *   H: {17,18,19}       Rafael, Sabrina, Tiago
     *   I: {20,21,22,23,24} Ursula, Vinicius, Wanessa, Xavier, Yasmin
     *   J: {23,24,25}       Xavier, Yasmin, Zeca
     *   K: {26,27,0}        Alice, Bernardo, Ana
     *
     * Influentes esperados:
     *   Rafael (v17): em G e H, G inter H = {Rafael}
     *   Ana    (v0) : em A e K, A inter K = {Ana}
     */
    ordemG = 28;
    criaGrafo(&G, ordemG);

    /* --- Participantes --- */
    defineParticipante(G,  0, "Ana Beatriz",    "Ciencia da Computacao",  3);
    defineParticipante(G,  1, "Bruno Carvalho", "Ciencia da Computacao",  3);
    defineParticipante(G,  2, "Carla Mendes",   "Engenharia de Software", 3);
    defineParticipante(G,  3, "Diego Ramos",    "Ciencia da Computacao",  3);
    defineParticipante(G,  4, "Elena Souza",    "Sistemas de Informacao", 3);
    defineParticipante(G,  5, "Felipe Nunes",   "Ciencia da Computacao",  5);
    defineParticipante(G,  6, "Gabriela Lima",  "Engenharia de Software", 5);
    defineParticipante(G,  7, "Heitor Alves",   "Ciencia da Computacao",  5);
    defineParticipante(G,  8, "Isabela Costa",  "Sistemas de Informacao", 5);
    defineParticipante(G,  9, "Jorge Melo",     "Ciencia da Computacao",  5);
    defineParticipante(G, 10, "Karen Pinto",    "Engenharia de Software", 7);
    defineParticipante(G, 11, "Lucas Ferreira", "Ciencia da Computacao",  7);
    defineParticipante(G, 12, "Marina Torres",  "Sistemas de Informacao", 7);
    defineParticipante(G, 13, "Nicolas Rocha",  "Ciencia da Computacao",  7);
    defineParticipante(G, 14, "Olivia Martins", "Engenharia de Software", 7);
    defineParticipante(G, 15, "Pedro Azevedo",  "Ciencia da Computacao",  1);
    defineParticipante(G, 16, "Quezia Barros",  "Sistemas de Informacao", 1);
    defineParticipante(G, 17, "Rafael Dias",    "Ciencia da Computacao",  1);
    defineParticipante(G, 18, "Sabrina Lopes",  "Engenharia de Software", 1);
    defineParticipante(G, 19, "Tiago Moreira",  "Ciencia da Computacao",  1);
    defineParticipante(G, 20, "Ursula Faria",   "Sistemas de Informacao", 9);
    defineParticipante(G, 21, "Vinicius Gomes", "Ciencia da Computacao",  9);
    defineParticipante(G, 22, "Wanessa Silva",  "Engenharia de Software", 9);
    defineParticipante(G, 23, "Xavier Cunha",   "Ciencia da Computacao",  9);
    defineParticipante(G, 24, "Yasmin Ribeiro", "Sistemas de Informacao", 9);
    defineParticipante(G, 25, "Zeca Oliveira",  "Ciencia da Computacao",  9);
    defineParticipante(G, 26, "Alice Monteiro", "Engenharia de Software", 2);
    defineParticipante(G, 27, "Bernardo Paiva", "Ciencia da Computacao",  2);

    /* --- Panelinha A: Ana(0), Bruno(1), Carla(2), Diego(3) --- */
    acrescentaAresta(G, ordemG,  0,  1);
    acrescentaAresta(G, ordemG,  0,  2);
    acrescentaAresta(G, ordemG,  0,  3);
    acrescentaAresta(G, ordemG,  1,  2);
    acrescentaAresta(G, ordemG,  1,  3);
    acrescentaAresta(G, ordemG,  2,  3);

    /* --- Panelinha B: Bruno(1), Carla(2), Elena(4) --- */
    acrescentaAresta(G, ordemG,  1,  4);
    acrescentaAresta(G, ordemG,  2,  4);

    /* --- Panelinha C: Felipe(5), Gabriela(6), Heitor(7), Isabela(8) --- */
    acrescentaAresta(G, ordemG,  5,  6);
    acrescentaAresta(G, ordemG,  5,  7);
    acrescentaAresta(G, ordemG,  5,  8);
    acrescentaAresta(G, ordemG,  6,  7);
    acrescentaAresta(G, ordemG,  6,  8);
    acrescentaAresta(G, ordemG,  7,  8);

    /* --- Panelinha D: Heitor(7), Isabela(8), Jorge(9) --- */
    acrescentaAresta(G, ordemG,  7,  9);
    acrescentaAresta(G, ordemG,  8,  9);

    /* --- Panelinha E: Karen(10), Lucas(11), Marina(12), Nicolas(13) --- */
    acrescentaAresta(G, ordemG, 10, 11);
    acrescentaAresta(G, ordemG, 10, 12);
    acrescentaAresta(G, ordemG, 10, 13);
    acrescentaAresta(G, ordemG, 11, 12);
    acrescentaAresta(G, ordemG, 11, 13);
    acrescentaAresta(G, ordemG, 12, 13);

    /* --- Panelinha F: Marina(12), Nicolas(13), Olivia(14) --- */
    acrescentaAresta(G, ordemG, 12, 14);
    acrescentaAresta(G, ordemG, 13, 14);

    /* --- Panelinha G: Pedro(15), Quezia(16), Rafael(17) --- */
    acrescentaAresta(G, ordemG, 15, 16);
    acrescentaAresta(G, ordemG, 15, 17);
    acrescentaAresta(G, ordemG, 16, 17);

    /* --- Panelinha H: Rafael(17), Sabrina(18), Tiago(19) --- */
    acrescentaAresta(G, ordemG, 17, 18);
    acrescentaAresta(G, ordemG, 17, 19);
    acrescentaAresta(G, ordemG, 18, 19);

    /* --- Panelinha I: Ursula(20), Vinicius(21), Wanessa(22), Xavier(23), Yasmin(24) --- */
    acrescentaAresta(G, ordemG, 20, 21);
    acrescentaAresta(G, ordemG, 20, 22);
    acrescentaAresta(G, ordemG, 20, 23);
    acrescentaAresta(G, ordemG, 20, 24);
    acrescentaAresta(G, ordemG, 21, 22);
    acrescentaAresta(G, ordemG, 21, 23);
    acrescentaAresta(G, ordemG, 21, 24);
    acrescentaAresta(G, ordemG, 22, 23);
    acrescentaAresta(G, ordemG, 22, 24);
    acrescentaAresta(G, ordemG, 23, 24);

    /* --- Panelinha J: Xavier(23), Yasmin(24), Zeca(25) --- */
    acrescentaAresta(G, ordemG, 23, 25);
    acrescentaAresta(G, ordemG, 24, 25);

    /* --- Panelinha K: Alice(26), Bernardo(27), Ana(0) --- */
    acrescentaAresta(G, ordemG, 26, 27);
    acrescentaAresta(G, ordemG, 26,  0);
    acrescentaAresta(G, ordemG, 27,  0);

    /* --- Amizades extras (nao formam novas panelinhas) --- */
    acrescentaAresta(G, ordemG,  3,  5);  /* Diego   - Felipe   */
    acrescentaAresta(G, ordemG,  9, 11);  /* Jorge   - Lucas    */
    acrescentaAresta(G, ordemG, 14, 20);  /* Olivia  - Ursula   */
    acrescentaAresta(G, ordemG, 19, 26);  /* Tiago   - Alice    */
    acrescentaAresta(G, ordemG, 25, 27);  /* Zeca    - Bernardo */

    /* --- Exibe a rede social completa --- */
    exibeRede(G, ordemG);

    /* --- Encontra todas as panelinhas --- */
    encontraPanelinhas(G, ordemG, &col);

    printf("============================================================\n");
    printf("  TODAS AS PANELINHAS DA REDE (%d encontradas)              \n",
           col.quantidade);
    printf("============================================================\n\n");

    for (i = 0; i < col.quantidade; i++) {
        printf("  Panelinha %2d (%d membros): ", i + 1, col.lista[i].tamanho);
        for (j = 0; j < col.lista[i].tamanho; j++) {
            printf("%s", G[col.lista[i].membros[j]].nome);
            if (j < col.lista[i].tamanho - 1) printf(", ");
        }
        printf("\n");
    }
    printf("\n");

    /* --- Consulta panelinhas de todos os participantes --- */
    printf("============================================================\n");
    printf("  CONSULTA DE PANELINHAS POR PARTICIPANTE                   \n");
    printf("============================================================\n\n");
    for (i = 0; i < ordemG; i++)
        consultaPanelinhas(G, ordemG, &col, i);

    /* --- Consulta os influentes --- */
    consultaInfluentes(G, ordemG, &col);

    /* --- Libera memoria --- */
    destroiGrafo(&G, ordemG);

    printf("Pressione ENTER para terminar\n");
    getchar();
    return 0;
}