#include <stdio.h>
#include <stdlib.h>

#define ORDEM 3

typedef struct No {
    int total;
    int* chaves;
    struct No** filhos;
    struct No* pai;
} No;

No* criarNo() {
    No* novoNo = (No*)malloc(sizeof(No));
    novoNo->total = 0;
    novoNo->chaves = (int*)malloc(sizeof(int) * (2 * ORDEM - 1));
    novoNo->filhos = (No**)malloc(sizeof(No*) * (2 * ORDEM));
    novoNo->pai = NULL;

    for (int i = 0; i < 2 * ORDEM; i++) {
        novoNo->filhos[i] = NULL;
    }

    return novoNo;
}

No* buscar(No* no, int chave) {
    int i = 0;
    while (i < no->total && chave > no->chaves[i]) {
        i++;
    }

    if (i < no->total && chave == no->chaves[i]) {
        return no;
    } else if (no->filhos[i] != NULL) {
        return buscar(no->filhos[i], chave);
    } else {
        return NULL;
    }
}

void dividirNo(No* noPai, int i, No* noFilho) {
    No* novoNo = criarNo();
    int noMeio = noFilho->chaves[ORDEM - 1];

    for (int j = 0; j < ORDEM - 1; j++) {
        novoNo->chaves[j] = noFilho->chaves[j + ORDEM];
    }

    if (noFilho->filhos[0] != NULL) {
        for (int j = 0; j < ORDEM; j++) {
            novoNo->filhos[j] = noFilho->filhos[j + ORDEM];
            novoNo->filhos[j]->pai = novoNo;
        }
    }

    noFilho->total = ORDEM - 1;
    novoNo->total = ORDEM - 1;

    for (int j = noPai->total; j > i; j--) {
        noPai->filhos[j + 1] = noPai->filhos[j];
    }

    noPai->filhos[i + 1] = novoNo;
    novoNo->pai = noPai;

    for (int j = noPai->total - 1; j >= i; j--) {
        noPai->chaves[j + 1] = noPai->chaves[j];
    }
    noPai->chaves[i] = noMeio;
    noPai->total++;
}

void inserirNaoCheio(No* no, int chave) {
    int i = no->total - 1;

    if (no->filhos[0] == NULL) {
        while (i >= 0 && chave < no->chaves[i]) {
            no->chaves[i + 1] = no->chaves[i];
            i--;
        }
        no->chaves[i + 1] = chave;
        no->total++;
    } else {
        while (i >= 0 && chave < no->chaves[i]) {
            i--;
        }
        i++;

        if (no->filhos[i]->total == 2 * ORDEM - 1) {
            dividirNo(no, i, no->filhos[i]);

            if (chave > no->chaves[i]) {
                i++;
            }
        }

        inserirNaoCheio(no->filhos[i], chave);
    }
}

void inserir(No* raiz, int chave) {
    if (raiz->total == 2 * ORDEM - 1) {

        No* novoNo = criarNo();
        novoNo->filhos[0] = raiz;
        raiz->pai = novoNo;
        dividirNo(novoNo, 0, raiz);
        raiz = novoNo;
    }
    inserirNaoCheio(raiz, chave);
}

void fundirNo(No* pai, int indiceFilho) {
    No* noEsquerda = pai->filhos[indiceFilho];
    No* noDireita = pai->filhos[indiceFilho + 1];

    noEsquerda->chaves[ORDEM - 1] = pai->chaves[indiceFilho];
    noEsquerda->total++;

    for (int i = 0; i < noDireita->total; i++) {
        noEsquerda->chaves[ORDEM + i] = noDireita->chaves[i];
        noEsquerda->total++;
    }

    for (int i = 0; i <= noDireita->total; i++) {
        noEsquerda->filhos[ORDEM + i] = noDireita->filhos[i];
        if (noEsquerda->filhos[ORDEM + i] != NULL) {
            noEsquerda->filhos[ORDEM + i]->pai = noEsquerda;
        }
    }

    for (int i = indiceFilho; i < pai->total - 1; i++) {
        pai->chaves[i] = pai->chaves[i + 1];
        pai->filhos[i + 1] = pai->filhos[i + 2];
    }
    pai->total--;

    free(noDireita);
}

void redistribuirChaves(No* pai, int indiceFilho, int indiceVizinho, No* noAtual, No* noVizinho) {
    if (indiceFilho < indiceVizinho) {
        noAtual->chaves[ORDEM - 1] = pai->chaves[indiceFilho];
        noAtual->total++;
        pai->chaves[indiceFilho] = noVizinho->chaves[0];

        for (int i = 0; i < noVizinho->total - 1; i++) {
            noVizinho->chaves[i] = noVizinho->chaves[i + 1];
        }
        noVizinho->total--;
    } else {
        for (int i = noAtual->total; i > 0; i--) {
            noAtual->chaves[i] = noAtual->chaves[i - 1];
        }
        noAtual->total++;
        noAtual->chaves[0] = pai->chaves[indiceFilho - 1];
        pai->chaves[indiceFilho - 1] = noVizinho->chaves[noVizinho->total - 1];
        noVizinho->total--;
    }
}

void remover(No* no, int chave) {
    int i = 0;
    while (i < no->total && chave > no->chaves[i]) {
        i++;
    }

    if (i < no->total && chave == no->chaves[i]) {
        for (int j = i; j < no->total - 1; j++) {
            no->chaves[j] = no->chaves[j + 1];
        }
        no->total--;
    } else if (no->filhos[i] != NULL) {
        remover(no->filhos[i], chave);

        if (no->filhos[i]->total < ORDEM - 1) {
            No* pai = no;
            int indiceFilho = i;

            while (indiceFilho <= pai->total && pai->filhos[indiceFilho] != no->filhos[i]) {
                indiceFilho++;
            }

            if (indiceFilho > 0 && pai->filhos[indiceFilho - 1]->total > ORDEM - 1) {
                redistribuirChaves(pai, indiceFilho, indiceFilho - 1, no->filhos[i], pai->filhos[indiceFilho - 1]);
            } else if (indiceFilho < pai->total && pai->filhos[indiceFilho + 1]->total > ORDEM - 1) {
                redistribuirChaves(pai, indiceFilho, indiceFilho + 1, no->filhos[i], pai->filhos[indiceFilho + 1]);
            } else if (indiceFilho > 0) {
                fundirNo(pai, indiceFilho - 1);
            } else {
                fundirNo(pai, indiceFilho);
            }
        }
    }
}

int main() {
    No* raiz = criarNo();
	inserir(raiz, 23);
	inserir(raiz, 12);
	remover(raiz, 23);
	inserir(raiz, 31);
	inserir(raiz, 54);
	inserir(raiz, 78);
	inserir(raiz, 18);
	inserir(raiz, 9);
	inserir(raiz, 44);
	remover(raiz, 12);
	inserir(raiz, 23);

    int chaveDeBusca = 12;
    No* resultado = buscar(raiz, chaveDeBusca);

    if (resultado != NULL) {
        printf("Chave %d encontrada!\n", chaveDeBusca);
    } else {
        printf("Chave %d nao encontrada.\n", chaveDeBusca);
    }
    
    chaveDeBusca = 9;
    resultado = buscar(raiz, chaveDeBusca);

    if (resultado != NULL) {
        printf("Chave %d encontrada!\n", chaveDeBusca);
    } else {
        printf("Chave %d nao encontrada.\n", chaveDeBusca);
    }
    
    chaveDeBusca = 23;
    resultado = buscar(raiz, chaveDeBusca);

    if (resultado != NULL) {
        printf("Chave %d encontrada!\n", chaveDeBusca);
    } else {
        printf("Chave %d nao encontrada.\n", chaveDeBusca);
    }
    
    return 0;
}

