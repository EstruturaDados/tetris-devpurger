#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> // Para memmove ou memcpy, se necessário, mas vamos usar loops para clareza

// --- Constantes ---
#define CAPACIDADE_FILA 5
#define CAPACIDADE_PILHA 3
#define CAPACIDADE_UNDO 5 // Capacidade para o histórico de "desfazer"

// ID sequencial para cada nova peça gerada.
int proximo_id = 1001; 

// -----------------------------------------------------------------------------
// --- ESTRUTURAS DE DADOS ---
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
    int topo; 
} PilhaReserva;

// Estrutura da Pilha Linear para Histórico (Undo)
typedef struct {
    Peca pecas[CAPACIDADE_UNDO];
    int topo;
} PilhaUndo;

// -----------------------------------------------------------------------------
// --- FUNÇÕES AUXILIARES DE PEÇAS E GERAÇÃO ---
// -----------------------------------------------------------------------------

Peca gerarPeca() {
    char tipos[] = {'I', 'O', 'T', 'L', 'J', 'S', 'Z'};
    int num_tipos = sizeof(tipos) / sizeof(tipos[0]);
    
    char tipo_sorteado = tipos[rand() % num_tipos];
    
    Peca nova_peca = {tipo_sorteado, proximo_id++};
    return nova_peca;
}

// -----------------------------------------------------------------------------
// --- FUNÇÕES BÁSICAS DA PILHA (RESERVA E UNDO) ---
// -----------------------------------------------------------------------------

// Inicializa a Pilha VAZIA.
void inicializarPilha(PilhaReserva *p) { p->topo = -1; }
void inicializarPilhaUndo(PilhaUndo *u) { u->topo = -1; }

// Verifica se está vazia/cheia.
int estaVaziaPilha(PilhaReserva *p) { return p->topo == -1; }
int estaCheiaPilha(PilhaReserva *p) { return p->topo == CAPACIDADE_PILHA - 1; }
int estaVaziaUndo(PilhaUndo *u) { return u->topo == -1; }
int estaCheiaUndo(PilhaUndo *u) { return u->topo == CAPACIDADE_UNDO - 1; }

// Adiciona uma peça no topo da Pilha (Push).
int push(PilhaReserva *p, Peca peca_reservada) {
    if (estaCheiaPilha(p)) return 0;
    p->topo++;
    p->pecas[p->topo] = peca_reservada;
    return 1;
}

// Remove e retorna a peça do topo da Pilha (Pop).
Peca pop(PilhaReserva *p) {
    if (estaVaziaPilha(p)) return (Peca){' ', 0}; // Peça nula
    Peca peca_usada = p->pecas[p->topo];
    p->topo--;
    return peca_usada;
}

// Adiciona uma peça ao Histórico (Push Undo).
int pushUndo(PilhaUndo *u, Peca peca_jogada) {
    if (estaCheiaUndo(u)) {
        // Se o histórico estiver cheio, movemos todos os itens para liberar espaço
        // Mantendo apenas as 4 últimas jogadas.
        for (int i = 0; i < CAPACIDADE_UNDO - 1; i++) {
            u->pecas[i] = u->pecas[i + 1];
        }
        u->topo = CAPACIDADE_UNDO - 2; // O topo aponta para a penúltima
    }
    u->topo++;
    u->pecas[u->topo] = peca_jogada;
    return 1;
}

// Remove e retorna a peça do topo do Histórico (Pop Undo).
Peca popUndo(PilhaUndo *u) {
    if (estaVaziaUndo(u)) return (Peca){' ', 0}; // Peça nula
    Peca peca_revertida = u->pecas[u->topo];
    u->topo--;
    return peca_revertida;
}

// -----------------------------------------------------------------------------
// --- FUNÇÕES BÁSICAS DA FILA CIRCULAR ---
// -----------------------------------------------------------------------------

void inicializarFila(FilaPecas *f) {
    f->frente = 0;
    f->tras = 0; 
    for (int i = 0; i < CAPACIDADE_FILA; i++) {
        f->pecas[f->tras] = gerarPeca();
        f->tras = (f->tras + 1) % CAPACIDADE_FILA; 
    }
}

// Insere no final (Enqueue).
void enqueue(FilaPecas *f, Peca p) {
    f->pecas[f->tras] = p;
    f->tras = (f->tras + 1) % CAPACIDADE_FILA; 
}

// Remove da frente (Dequeue).
Peca dequeue(FilaPecas *f) {
    Peca peca_removida = f->pecas[f->frente];
    f->frente = (f->frente + 1) % CAPACIDADE_FILA;
    return peca_removida;
}

// NOVO: Insere na FRENTE da Fila (Necessário para Desfazer)
// Move todos os elementos uma posição para trás (circular) e insere na frente.
void enqueueFrente(FilaPecas *f, Peca p) {
    // Para inserir na frente (f->frente), precisamos que 'tras' aponte para a nova 'frente'.
    // Primeiro, movemos o 'frente' UMA posição para TRÁS (revertendo o módulo)
    f->frente = (f->frente - 1 + CAPACIDADE_FILA) % CAPACIDADE_FILA;
    
    // Depois, inserimos a peça na nova posição 'frente'.
    f->pecas[f->frente] = p;
    // O 'tras' permanece onde estava, pois o número de elementos não mudou.
}

// -----------------------------------------------------------------------------
// --- FUNÇÕES ESTRATÉGICAS DO NÍVEL MESTRE ---
// -----------------------------------------------------------------------------

// Documentação: Reverte a última jogada.
void desfazerJogada(FilaPecas *f, PilhaUndo *u) {
    Peca peca_revertida = popUndo(u);
    
    if (peca_revertida.id == 0) {
        printf("🚫 ERRO: Pilha de Desfazer Vazia. Não há jogadas para reverter.\n");
        return;
    }
    
    printf("↩️ DESFAZER: Peça [%c - ID:%d] voltando para a frente da Fila.\n", peca_revertida.nome, peca_revertida.id);
    
    // 1. Remove a última peça inserida (Enqueue automático) do final da Fila.
    // O 'tras' aponta para o próximo vazio. Voltando uma posição, encontramos a última inserida.
    f->tras = (f->tras - 1 + CAPACIDADE_FILA) % CAPACIDADE_FILA;
    
    // 2. Insere a peça revertida (peca_revertida) na FRENTE da Fila.
    enqueueFrente(f, peca_revertida);
    
    printf("   Peça mais recente [%c - ID:%d] removida do final da fila.\n", f->pecas[f->frente].nome, f->pecas[f->frente].id);
}

// Documentação: Troca a peça da frente da Fila com a do topo da Pilha.
void trocarFilaPilha(FilaPecas *f, PilhaReserva *p) {
    if (estaVaziaPilha(p)) {
        printf("🚫 Ação não realizada. A Pilha de Reserva está VAZIA.\n");
        return;
    }

    // 1. Guarda a peça da Fila (Frente)
    Peca temp_fila = f->pecas[f->frente];

    // 2. Guarda a peça da Pilha (Topo)
    Peca temp_pilha = p->pecas[p->topo];
    
    // 3. Coloca a peça da Pilha na Fila (Frente)
    f->pecas[f->frente] = temp_pilha;

    // 4. Coloca a peça da Fila na Pilha (Topo)
    p->pecas[p->topo] = temp_fila;

    printf("🔄 TROCA REALIZADA: \n");
    printf("   Frente da Fila agora é [%c - ID:%d]\n", temp_pilha.nome, temp_pilha.id);
    printf("   Topo da Pilha agora é [%c - ID:%d]\n", temp_fila.nome, temp_fila.id);
}

// Documentação: Inverte a Fila com a Pilha.
void inverterFilaPilha(FilaPecas *f, PilhaReserva *p) {
    if (CAPACIDADE_FILA != CAPACIDADE_PILHA) {
        printf("🚫 Ação complexa não suportada: Ação de Inverter exige que as estruturas tenham o mesmo número de elementos. (5 != 3)\n");
        return;
    }
    // NOTA: Para este desafio, a capacidade da Fila é 5 e da Pilha é 3. A inversão total não é logicamente segura
    // ou desejável. Deixamos a mensagem de erro para demonstrar a restrição, mas a implementaremos
    // parcialmente (inverter o que for possível, 3 peças) ou de forma conceituada. 
    // Vamos implementar a troca segura de APENAS a parte cheia da Pilha com a Frente da Fila.

    printf("❌ INVERTER ESTRUTURAS: Esta operação é complexa, pois as capacidades são diferentes (Fila: 5, Pilha: 3).\n");
    printf("   Implementando inversão parcial: A Fila (frente) troca com a Pilha (topo).\n");
    
    // Para o Nível Mestre, vamos implementar a lógica que inverte o conteúdo de AMBAS as estruturas
    // (o que couber). Isso é um desafio complexo. Usaremos um array temporário.
    
    Peca temp_pilha[CAPACIDADE_PILHA];
    int cont_pilha = 0;

    // 1. Retirar TUDO da Pilha para um buffer
    while (!estaVaziaPilha(p)) {
        temp_pilha[cont_pilha++] = pop(p);
    }
    
    // 2. Retirar N peças da Fila (N = CAPACIDADE_PILHA) para a Pilha
    printf("   - Movendo %d peças da Fila para a Pilha...\n", CAPACIDADE_PILHA);
    for (int i = 0; i < CAPACIDADE_PILHA; i++) {
        Peca peca_fila = dequeue(f);
        push(p, peca_fila);
        // Garante que a fila não perca a circularidade (insere nova peça)
        enqueue(f, gerarPeca());
    }

    // 3. Inserir as peças do buffer de volta na Fila
    printf("   - Movendo %d peças do buffer para a Fila...\n", cont_pilha);
    for (int i = 0; i < cont_pilha; i++) {
        // Remove a peça mais recente da Fila (último Enqueue)
        f->tras = (f->tras - 1 + CAPACIDADE_FILA) % CAPACIDADE_FILA;
        // Coloca a peça do buffer na frente da Fila (simulando a inversão de conteúdo)
        enqueueFrente(f, temp_pilha[i]);
    }
    printf("✅ INVERSÃO PARCIAL CONCLUÍDA!\n");
}


// -----------------------------------------------------------------------------
// --- FUNÇÕES DE VISUALIZAÇÃO ---
// -----------------------------------------------------------------------------

void visualizarFila(FilaPecas *f) {
    printf("\n*** FILA DE PEÇAS FUTURAS (Capacidade: %d) ***\n", CAPACIDADE_FILA);
    // ... (Código de visualização da Fila) ...
    printf("| Índice | Tipo | ID | Posição na Fila |\n");
    printf("|--------|------|----|-----------------|\n");
    
    for (int i = 0; i < CAPACIDADE_FILA; i++) {
        char *posicao = "";
        if (i == f->frente) {
            posicao = " <--- PRÓXIMA (Frente)";
        } else if (i == f->tras) {
            posicao = " <--- ENTRADA (Trás)";
        }
        printf("| %6d |  %c   | %d |%s|\n", i, f->pecas[i].nome, f->pecas[i].id, posicao);
    }
    printf("--------------------------------------------\n");
}

void visualizarPilha(PilhaReserva *p) {
    printf("\n*** PILHA DE RESERVA (Capacidade: %d) ***\n", CAPACIDADE_PILHA);
    if (estaVaziaPilha(p)) {
        printf("Pilha: [VAZIA]\n");
        printf("--------------------------------------------\n");
        return;
    }
    
    printf("| Índice | Tipo | ID | Posição na Pilha |\n");
    printf("|--------|------|----|-----------------|\n");
    for (int i = p->topo; i >= 0; i--) {
        char *posicao = "";
        if (i == p->topo) {
            posicao = " <--- TOPO";
        }
        printf("| %6d |  %c   | %d |%s|\n", i, p->pecas[i].nome, p->pecas[i].id, posicao);
    }
    printf("--------------------------------------------\n");
}

void visualizarUndo(PilhaUndo *u) {
    printf("\n*** PILHA DE HISTÓRICO (UNDO) (Capacidade: %d) ***\n", CAPACIDADE_UNDO);
    if (estaVaziaUndo(u)) {
        printf("Histórico: [VAZIO]\n");
        printf("--------------------------------------------\n");
        return;
    }
    
    printf("| Índice | Tipo | ID | Posição no Histórico |\n");
    printf("|--------|------|----|----------------------|\n");
    for (int i = u->topo; i >= 0; i--) {
        char *posicao = "";
        if (i == u->topo) {
            posicao = " <--- ÚLTIMA JOGADA (Topo)";
        }
        printf("| %6d |  %c   | %d |%s|\n", i, u->pecas[i].nome, u->pecas[i].id, posicao);
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
    PilhaUndo historico; // Nova Pilha
    
    inicializarFila(&fila);
    inicializarPilha(&pilha);
    inicializarPilhaUndo(&historico);
    
    visualizarFila(&fila);
    visualizarPilha(&pilha);
    visualizarUndo(&historico);

    int opcao;
    
    do {
        printf("\n=== Menu Tetris Stack - Nível MESTRE ===\n");
        printf("1 - Jogar próxima peça (Fila Dequeue + Enqueue)\n");
        printf("2 - Reservar peça (Fila -> Pilha Push + Fila Enqueue)\n");
        printf("3 - Usar peça reservada (Pilha Pop)\n");
        printf("4 - Trocar peça (Fila Frente <-> Pilha Topo)\n");
        printf("5 - Desfazer última jogada (Undo)\n");
        printf("6 - Inverter Fila com Pilha (Parcial)\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        
        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n');
            opcao = -1; 
            printf("\nOpção inválida. Por favor, digite um número.\n");
            continue;
        }

        switch (opcao) {
            case 1: // JOGAR PEÇA (Dequeue da Fila + Enqueue Automático + Undo Push)
                printf("\n--- AÇÃO: JOGAR PEÇA ---\n");
                Peca jogada = dequeue(&fila);
                pushUndo(&historico, jogada); // Adiciona ao histórico!
                Peca nova_jogada = gerarPeca();
                enqueue(&fila, nova_jogada);
                break;

            case 2: // RESERVAR PEÇA (Fila -> Pilha Push + Enqueue Automático)
                printf("\n--- AÇÃO: RESERVAR PEÇA ---\n");
                if (estaCheiaPilha(&pilha)) {
                    printf("🚫 Ação não realizada. A Pilha de Reserva está cheia!\n");
                    break;
                }
                Peca reservada = dequeue(&fila); 
                if (push(&pilha, reservada)) {
                    Peca nova_reserva = gerarPeca();
                    enqueue(&fila, nova_reserva);
                }
                break;

            case 3: // USAR PEÇA RESERVADA (Pilha Pop)
                printf("\n--- AÇÃO: USAR PEÇA RESERVADA ---\n");
                pop(&pilha); 
                break;

            case 4: // TROCAR PEÇA
                printf("\n--- AÇÃO: TROCAR PEÇA ---\n");
                trocarFilaPilha(&fila, &pilha);
                break;
                
            case 5: // DESFAZER ÚLTIMA JOGADA
                printf("\n--- AÇÃO: DESFAZER ÚLTIMA JOGADA ---\n");
                desfazerJogada(&fila, &historico);
                break;

            case 6: // INVERTER FILA COM PILHA
                printf("\n--- AÇÃO: INVERTER ESTRUTURAS ---\n");
                inverterFilaPilha(&fila, &pilha);
                break;

            case 0:
                printf("\nObrigado por completar o Desafio Tetris Stack - Nível Mestre! Excelente trabalho com Estruturas de Dados.\n");
                break;
            default:
                printf("\n❌ Opção inválida. Tente novamente.\n");
        }
        
        // Exibe o estado após cada ação
        if (opcao != 0 && opcao != -1) {
            visualizarFila(&fila);
            visualizarPilha(&pilha);
            visualizarUndo(&historico);
        }
    } while (opcao != 0);

    return 0;
}