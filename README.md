# Projeto de Programação – Parte 2
## Panelinhas em Redes Sociais
**Teoria dos Grafos – 2026/1**

---

## Grupo

| Nome | RA |
|---|---|
| João Guilherme Messias de Oliveira Santos | 10426110 |
| André Ihsan Ward | 10425684 |
| Enzo Ponte Gamberi | 10389931 |
| Thiago Ruiz Fernandes Silva | 10426057 |
| Luís Felipe Cunha | 10419514 |

---

## Descrição

Modelagem de uma **rede social** como um grafo não orientado, onde:

- Cada **vértice** representa um participante da rede (aluno de faculdade)
- Cada **aresta** representa uma amizade entre dois participantes

O programa identifica as **panelinhas** da rede e os **elementos influentes**.

### O que é uma panelinha?
Um grupo de pessoas onde **todo mundo se conhece com todo mundo** — tecnicamente, um clique maximal com pelo menos 3 membros. Maximal significa que não dá para adicionar mais ninguém ao grupo mantendo essa propriedade.

### O que é um elemento influente?
Uma pessoa **Psi** é influente se ela faz parte de duas panelinhas **alfa** e **beta** cuja única conexão é ela mesma, ou seja: **alfa ∩ beta = {Psi}**.

---

## Arquivos do projeto

```
Grafos-Projeto2/
├── Grafo_2026-1_P2.c       <- código-fonte completo em C (ANSI C)
├── documentacao.pdf         <- documentação do projeto
└── README.md                <- este arquivo
```

---

## Como compilar e executar

### DEV C++ (Windows)
1. Abra o DEV C++
2. `File -> Open -> Grafo_2026-1_P2.c`
3. `Execute -> Compile & Run` (ou F11)

### GCC (linha de comando)
```bash
gcc -ansi -o projeto2 Grafo_2026-1_P2.c
./projeto2
```

---

## O grafo utilizado

- **28 participantes** (vértices) — alunos fictícios de uma turma de faculdade
- **50 amizades** (arestas)
- Cada participante tem: **nome**, **curso** e **semestre**

| Cor no yEd | Curso |
|---|---|
| Azul | Ciência da Computação |
| Verde | Engenharia de Software |
| Laranja | Sistemas de Informação |

---

## Funcionalidades implementadas

| Função | Descrição |
|---|---|
| `exibeRede` | Exibe todos os detalhes da rede: participantes, grau de cada um e lista de amizades |
| `encontraPanelinhas` | Encontra todos os cliques maximais >= 3 usando Busca em Profundidade (DFS) |
| `consultaPanelinhas` | Lista todas as panelinhas de cada participante da rede |
| `consultaInfluentes` | Identifica e exibe os elementos influentes com prova (alfa, beta e alfa ∩ beta) |

---

## Resultados

### Panelinhas encontradas (11 no total)

| # | Membros | Tamanho |
|---|---|---|
| 1 | Ana Beatriz, Bruno Carvalho, Carla Mendes, Diego Ramos | 4 |
| 2 | Ana Beatriz, Alice Monteiro, Bernardo Paiva | 3 |
| 3 | Bruno Carvalho, Carla Mendes, Elena Souza | 3 |
| 4 | Felipe Nunes, Gabriela Lima, Heitor Alves, Isabela Costa | 4 |
| 5 | Heitor Alves, Isabela Costa, Jorge Melo | 3 |
| 6 | Karen Pinto, Lucas Ferreira, Marina Torres, Nicolas Rocha | 4 |
| 7 | Marina Torres, Nicolas Rocha, Olivia Martins | 3 |
| 8 | Pedro Azevedo, Quezia Barros, Rafael Dias | 3 |
| 9 | Rafael Dias, Sabrina Lopes, Tiago Moreira | 3 |
| 10 | Ursula Faria, Vinicius Gomes, Wanessa Silva, Xavier Cunha, Yasmin Ribeiro | 5 |
| 11 | Xavier Cunha, Yasmin Ribeiro, Zeca Oliveira | 3 |

### Elementos influentes encontrados (2)

**Ana Beatriz**
- Panelinha alfa: {Ana Beatriz, Bruno Carvalho, Carla Mendes, Diego Ramos}
- Panelinha beta: {Ana Beatriz, Alice Monteiro, Bernardo Paiva}
- alfa ∩ beta = {Ana Beatriz}

**Rafael Dias**
- Panelinha alfa: {Pedro Azevedo, Quezia Barros, Rafael Dias}
- Panelinha beta: {Rafael Dias, Sabrina Lopes, Tiago Moreira}
- alfa ∩ beta = {Rafael Dias}

---

## Algoritmo para encontrar panelinhas

Usamos **Busca em Profundidade (DFS)** para encontrar todos os cliques maximais:

1. Para cada vértice do grafo, iniciamos uma DFS tentando formar um grupo
2. Um vértice só entra no grupo se for **amigo de todos** que já estão nele
3. Para evitar repetições, só tentamos vértices com **índice maior** que o último adicionado
4. Quando não é possível adicionar mais ninguém, verificamos se o grupo é **maximal** — ou seja, se nenhum vértice de fora poderia entrar
5. Se for maximal e tiver **3 ou mais membros**, é uma panelinha válida

---

## Disciplina

**Teoria dos Grafos – 2026/1**
Universidade — Ciência da Computação
