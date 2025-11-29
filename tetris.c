#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- Constantes ---
#define CAPACIDADE 5
// ID sequencial para cada nova peça gerada.
int proximo_id = 1001; 

// --- 1. Estrutura da Peça ---
typedef struct {
    char nome; // Tipo da peça: 'I', 'O', 'T', 'L', 'J', 'S', 'Z'
    int id;    // Identificador único da peça
} Peca;

// --- 2. Estrutura da Fila Circular ---
typedef struct {
    Peca pecas[CAPACIDADE]; // Array de Peças
    int frente;             // Índice da próxima peça a ser removida (dequeue)
    int tras;               // Índice onde a próxima peça será inserida (enqueue)
} FilaPecas;

// -----------------------------------------------------------------------------
// --- Funções Auxiliares de Peças ---
// -----------------------------------------------------------------------------

// Documentação: Cria e retorna uma nova peça com tipo e ID únicos.
Peca gerarPeca() {
    // Tipos de peças Tetris: I, O, T, L, J, S, Z
    char tipos[] = {'I', 'O', 'T', 'L', 'J', 'S', 'Z'};
    int num_tipos = sizeof(tipos) / sizeof(tipos[0]);
    
    // Escolhe um tipo de forma aleatória
    char tipo_sorteado = tipos[rand() % num_tipos];
    
    Peca nova_peca = {tipo_sorteado, proximo_id++};
    return nova_peca;
}

// -----------------------------------------------------------------------------
// --- Funções da Fila Circular ---
// -----------------------------------------------------------------------------

// Documentação: Inicializa a Fila, preenchendo-a com 5 peças iniciais.
void inicializarFila(FilaPecas *f) {
    f->frente = 0;
    f->tras = 0; // O 'tras' indica o local VAZIO onde a próxima peça será inserida

    // Preencher a fila com CAPACIDADE peças.
    for (int i = 0; i < CAPACIDADE; i++) {
        f->pecas[f->tras] = gerarPeca();
        // Move o 'tras' para a próxima posição (circular)
        f->tras = (f->tras + 1) % CAPACIDADE; 
    }
    printf("✅ Fila inicializada com %d peças!\n", CAPACIDADE);
}

// Documentação: Verifica se a Fila está CHEIA.
// Numa fila circular de tamanho N, ela está cheia quando (tras + 1) % N == frente.
// Para este desafio, simplificamos: a fila deve estar SEMPRE cheia, então esta função é mais para a lógica futura.
int estaCheia(FilaPecas *f) {
    return (f->tras + 1) % CAPACIDADE == f->frente; 
}

// Documentação: Insere uma peça no final da Fila (Enqueue).
void enqueue(FilaPecas *f, Peca p) {
    // Como a fila deve estar sempre cheia neste nível, não verificamos se está cheia.
    // Simplesmente inserimos no índice 'tras'.
    f->pecas[f->tras] = p;
    
    // Move o 'tras' para a próxima posição (circular).
    f->tras = (f->tras + 1) % CAPACIDADE; 
    
    printf("➕ PEÇA INSERIDA: [%c - ID:%d] no final da fila.\n", p.nome, p.id);
}

// Documentação: Remove uma peça da frente da Fila (Dequeue).
Peca dequeue(FilaPecas *f) {
    // A peça a ser removida está no índice 'frente'.
    Peca peca_removida = f->pecas[f->frente];
    
    // Move o 'frente' para a próxima posição (circular).
    f->frente = (f->frente + 1) % CAPACIDADE;
    
    printf("➖ PEÇA JOGADA (REMOVIDA): [%c - ID:%d]\n", peca_removida.nome, peca_removida.id);
    return peca_removida;
}

// Documentação: Exibe o estado atual da Fila, marcando a 'Frente' e 'Trás'.
void visualizarFila(FilaPecas *f) {
    printf("\n*** FILA DE PEÇAS FUTURAS (Capacidade: %d) ***\n", CAPACIDADE);
    printf("| Índice | Tipo | ID | Posição na Fila |\n");
    printf("|--------|------|----|-----------------|\n");
    
    // Percorre todos os índices do array
    for (int i = 0; i < CAPACIDADE; i++) {
        char *posicao = "";
        
        // Identifica onde estão os ponteiros 'frente' e 'tras'
        if (i == f->frente) {
            posicao = " <--- PRÓXIMA (Frente)";
        } else if (i == f->tras) {
            // Se 'tras' é igual a 'frente', a fila estaria vazia.
            // Aqui, 'tras' aponta para a posição VAZIA onde a próxima peça entrará.
            posicao = " <--- ENTRADA (Trás)";
        }

        // Exibe a peça na posição 'i'
        printf("| %6d |  %c   | %d |%s|\n", 
               i, 
               f->pecas[i].nome, 
               f->pecas[i].id, 
               posicao);
    }
    printf("--------------------------------------------\n");
}

// -----------------------------------------------------------------------------
// --- Função Principal e Menu ---
// -----------------------------------------------------------------------------

int main() {
    // Inicialização para gerar peças aleatórias
    srand(time(NULL)); 
    
    FilaPecas fila;
    
    // 1. Inicializa a Fila com 5 peças.
    inicializarFila(&fila);
    visualizarFila(&fila);

    int opcao;
    
    do {
        printf("\n=== Menu Tetris Stack - Nível Novato ===\n");
        printf("1 - Jogar próxima peça (Dequeue)\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        
        // Garante que o usuário digite um número
        if (scanf("%d", &opcao) != 1) {
            // Limpa o buffer de entrada em caso de erro (ex: digitar letra)
            while (getchar() != '\n');
            opcao = -1; // Força a repetição
            printf("\nOpção inválida. Por favor, digite 1 ou 0.\n");
            continue;
        }

        switch (opcao) {
            case 1:
                // 2. Dequeue: Remove a peça da frente.
                Peca jogada = dequeue(&fila);
                
                // 3. Enqueue Automático: Insere uma nova peça para manter a fila cheia.
                Peca nova = gerarPeca();
                enqueue(&fila, nova);
                
                // 4. Visualiza o novo estado da fila.
                visualizarFila(&fila);
                break;
            case 0:
                printf("\nObrigado por jogar o Tetris Stack! Até a próxima.\n");
                break;
            default:
                printf("\n❌ Opção inválida. Tente novamente.\n");
        }
    } while (opcao != 0);

    return 0;
}