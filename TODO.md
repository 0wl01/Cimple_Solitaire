# TODO

## Tarefas - Simple Simon

- [ ] **Game Menu**
  - [ ] Add tests for the Game Menu
  - [ ] 

## Tarefas Pendentes - Lógica do Jogo (golf.c)

- [ ] **Game Rules**
  - Adicionar uma função para puxar automático 1 carta do deck
- [ ] **Corrigir Regra do "1 Return" (card.c)**
  - Refatorizar as funções `create_deck` e `push` para terem apenas um único `return` no final, cumprindo a regra imposta pelo professor.
  - Refatorizar as funções create_deck(2 returns) e push(2) no card.c
  - Refatorizar game_loop no golf.c
  - Refatorizar get_input no cli.c

- [x] **Estancar a Fuga de Memória (Memory Leak)**
  - Substituir todas as chamadas `exit(0)` na função `game_loop` por um regresso (return) limpo ao `main`, para que a memória seja libertada antes de o programa fechar.
  - Substituir todas as chamadas exit(0) na função game_loop por um regresso (return) limpo ao main
  - Garantir que a função clean_golf é chamada no final da execução para libertar o stock, waste e as 7 colunas

- [ ] **Mais testes unitários(CUnit)**
  - Teste de Situação de Vitória
  - Teste d

- [ ] **Test Coverage CUnit**
  - Compilar com profiling
  - Correr os testes com gcov 

- [ ] **Adicionar proteção contra Endless Loop**
  - Proteção contra "Control +"