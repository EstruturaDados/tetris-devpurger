#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- Constantes ---
#define CAPACIDADE_FILA 5
#define CAPACIDADE_PILHA 3
// ID sequencial para cada nova peça gerada.
int proximo_id = 1001; 

// -----------------------------------------------------------------------------
// --- ESTRUTURAS DE DADOS (Peca, FilaPecas, PilhaReserva) ---
// -----------------------------------------------------------------------------

// Estrutura da Peça
typedef struct {
    char nome; 
    int id;    
} Peca;

// Estrutura da Fila Circular (Próximas Peças)
typedef struct {
    Peca pecas[CAPACIDADE_FILA]; 
    int frente;             
    int tras;               
} FilaPecas;

// Estrutura da Pilha Linear (Reserva de Peças)
typedef struct {
    Peca pecas[CAPACIDADE_PILHA];
    int topo; // Indica a posição do último elemento (-1 se vazia)
} PilhaReserva;

// -----------------------------------------------------------------------------
// --- FUNÇÕES AUXILIARES DE PEÇAS E GERAÇÃO ---
// -----------------------------------------------------------------------------

// Documentação: Cria e retorna uma nova peça com tipo e ID únicos.
Peca gerarPeca() {
    char tipos[] = {'I', 'O', 'T', 'L', 'J', 'S', 'Z'};
    int num_tipos = sizeof(tipos) / sizeof(tipos[0]);
    
    char tipo_sorteado = tipos[rand() % num_tipos];
    
    Peca nova_peca = {tipo_sorteado, proximo_id++};
    return nova_peca;
}

// -----------------------------------------------------------------------------
// --- FUNÇÕES DA FILA CIRCULAR (Já do Nível Novato) ---
// -----------------------------------------------------------------------------

void inicializarFila(FilaPecas *f) {
    f->frente = 0;
    f->tras = 0; 
    for (int i = 0; i < CAPACIDADE_FILA; i++) {
        f->pecas[f->tras] = gerarPeca();
        f->tras = (f->tras + 1) % CAPACIDADE_FILA; 
    }
}

// Retorna 1 se a fila está cheia, 0 caso contrário.
int estaCheiaFila(FilaPecas *f) {
    return (f->tras + 1) % CAPACIDADE_FILA == f->frente; 
}

// Insere uma peça no final da Fila (Enqueue).
void enqueue(FilaPecas *f, Peca p) {
    // Para este nível, a fila deve estar sempre cheia. A inserção é garantida após um dequeue/reserva.
    f->pecas[f->tras] = p;
    f->tras = (f->tras + 1) % CAPACIDADE_FILA; 
}

// Remove uma peça da frente da Fila (Dequeue) - Simula "Jogar Peça".
Peca dequeue(FilaPecas *f) {
    Peca peca_removida = f->pecas[f->frente];
    f->frente = (f->frente + 1) % CAPACIDADE_FILA;
    return peca_removida;
}

// Exibe o estado atual da Fila.
void visualizarFila(FilaPecas *f) {
    printf("\n*** FILA DE PEÇAS FUTURAS (Capacidade: %d) ***\n", CAPACIDADE_FILA);
    printf("| Índice | Tipo | ID | Posição na Fila |\n");
    printf("|--------|------|----|-----------------|\n");
    
    for (int i = 0; i < CAPACIDADE_FILA; i++) {
        char *posicao = "";
        
        if (i == f->frente) {
            posicao = " <--- PRÓXIMA (Frente)";
        } else if (i == f->tras) {
            posicao = " <--- ENTRADA (Trás)";
        }

        printf("| %6d |  %c   | %d |%s|\n", 
               i, 
               f->pecas[i].nome, 
               f->pecas[i].id, 
               posicao);
    }
    printf("--------------------------------------------\n");
}


// -----------------------------------------------------------------------------
// --- FUNÇÕES DA PILHA LINEAR (Novidade do Nível Aventureiro) ---
// -----------------------------------------------------------------------------

// Documentação: Inicializa a Pilha VAZIA.
void inicializarPilha(PilhaReserva *p) {
    p->topo = -1; // -1 indica que a pilha está vazia
}

// Documentação: Retorna 1 se a pilha está vazia, 0 caso contrário.
int estaVaziaPilha(PilhaReserva *p) {
    return p->topo == -1;
}

// Documentação: Retorna 1 se a pilha está cheia, 0 caso contrário.
int estaCheiaPilha(PilhaReserva *p) {
    return p->topo == CAPACIDADE_PILHA - 1;
}

// Documentação: Adiciona uma peça no topo da Pilha (Push) - Simula "Reservar Peça".
int push(PilhaReserva *p, Peca peca_reservada) {
    if (estaCheiaPilha(p)) {
        printf("\n🚫 ERRO: Pilha de Reserva Cheia! Capacidade máxima: %d.\n", CAPACIDADE_PILHA);
        return 0; // Falha na inserção
    }
    
    // Incrementa o topo e insere a peça na nova posição.
    p->topo++;
    p->pecas[p->topo] = peca_reservada;
    printf("⬆️ PEÇA RESERVADA (PUSH): [%c - ID:%d] movida para a Pilha.\n", peca_reservada.nome, peca_reservada.id);
    return 1; // Sucesso
}

// Documentação: Remove e retorna a peça do topo da Pilha (Pop) - Simula "Usar Peça Reservada".
Peca pop(PilhaReserva *p) {
    if (estaVaziaPilha(p)) {
        printf("\n🚫 ERRO: Pilha de Reserva Vazia! Não há peças para usar.\n");
        // Retorna uma peça nula/inválida para indicar erro (simplificação)
        Peca erro = {' ', 0}; 
        return erro;
    }
    
    // Pega a peça do topo e decrementa o topo.
    Peca peca_usada = p->pecas[p->topo];
    p->topo--;
    printf("⬇️ PEÇA USADA (POP): [%c - ID:%d] removida da Pilha.\n", peca_usada.nome, peca_usada.id);
    return peca_usada;
}

// Documentação: Exibe o estado atual da Pilha.
void visualizarPilha(PilhaReserva *p) {
    printf("\n*** PILHA DE RESERVA (Capacidade: %d) ***\n", CAPACIDADE_PILHA);
    if (estaVaziaPilha(p)) {
        printf("Pilha: [VAZIA]\n");
        printf("--------------------------------------------\n");
        return;
    }
    
    // Percorre do topo para baixo (Pilha LIFO)
    for (int i = p->topo; i >= 0; i--) {
        char *posicao = "";
        if (i == p->topo) {
            posicao = " <--- TOPO";
        }
        printf("| %6d |  %c   | %d |%s|\n", 
               i, 
               p->pecas[i].nome, 
               p->pecas[i].id, 
               posicao);
    }
    printf("--------------------------------------------\n");
}


// -----------------------------------------------------------------------------
// --- Função Principal e Menu ---
// -----------------------------------------------------------------------------

int main() {
    srand(time(NULL)); 
    
    FilaPecas fila;
    PilhaReserva pilha;
    
    // 1. Inicializa ambas as estruturas
    inicializarFila(&fila);
    inicializarPilha(&pilha);
    
    visualizarFila(&fila);
    visualizarPilha(&pilha);

    int opcao;
    
    do {
        printf("\n=== Menu Tetris Stack - Nível Aventureiro ===\n");
        printf("1 - Jogar próxima peça (Fila Dequeue + Enqueue)\n");
        printf("2 - Reservar peça (Fila -> Pilha Push + Fila Enqueue)\n");
        printf("3 - Usar peça reservada (Pilha Pop)\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        
        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n');
            opcao = -1; 
            printf("\nOpção inválida. Por favor, digite 1, 2, 3 ou 0.\n");
            continue;
        }

        switch (opcao) {
            case 1: // JOGAR PEÇA (Dequeue da Fila + Enqueue Automático)
                printf("\n--- AÇÃO: JOGAR PEÇA ---\n");
                // Remove a peça da frente
                Peca jogada = dequeue(&fila);
                
                // Insere uma nova peça no final (manter fila cheia)
                Peca nova_jogada = gerarPeca();
                enqueue(&fila, nova_jogada);
                break;

            case 2: // RESERVAR PEÇA (Fila -> Pilha Push + Enqueue Automático)
                printf("\n--- AÇÃO: RESERVAR PEÇA ---\n");
                if (estaCheiaPilha(&pilha)) {
                    printf("🚫 Ação não realizada. A Pilha de Reserva está cheia!\n");
                    break;
                }
                
                // 1. Remove a peça da frente da Fila
                Peca reservada = dequeue(&fila); 
                
                // 2. Tenta inserir a peça removida na Pilha (Push)
                if (push(&pilha, reservada)) {
                    // 3. Se a reserva foi bem-sucedida, insere uma nova peça na Fila
                    Peca nova_reserva = gerarPeca();
                    enqueue(&fila, nova_reserva);
                }
                break;

            case 3: // USAR PEÇA RESERVADA (Pilha Pop)
                printf("\n--- AÇÃO: USAR PEÇA RESERVADA ---\n");
                // Remove a peça do topo da Pilha
                pop(&pilha); 
                // Nota: Não há substituição automática para manter a pilha cheia.
                break;

            case 0:
                printf("\nObrigado por jogar o Tetris Stack! Até a próxima.\n");
                break;
            default:
                printf("\n❌ Opção inválida. Tente novamente.\n");
        }
        
        // Exibe o estado após cada ação
        if (opcao != 0 && opcao != -1) {
            visualizarFila(&fila);
            visualizarPilha(&pilha);
        }
    } while (opcao != 0);

    return 0;
}