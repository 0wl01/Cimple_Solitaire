# TODO

- Gotta make all vars type signatures in the code more uniform.
## Tarefas Pendentes - Lógica do Jogo (golf.c)

- [ ] **Corrigir Regra do "1 Return" (card.c)**
  - Refatorizar as funções `create_deck` e `push` para terem apenas um único `return` no final, cumprindo a regra imposta pelo professor.
  - Refatorizar as funções create_deck(2 returns) e push(2) no card.c
  - Refatorizar game_loop no golf.c
  - Refatorizar get_input no cli.c

- [ ] **Estancar a Fuga de Memória (Memory Leak)**
  - Substituir todas as chamadas `exit(0)` na função `game_loop` por um regresso (return) limpo ao `main`, para que a memória seja libertada antes de o programa fechar.
  - Substituir todas as chamadas exit(0) na função game_loop por um regresso (return) limpo ao main
  - Garantir que a função clean_golf é chamada no final da execução para libertar o stock, waste e as 7 colunas

- [ ] **Limpeza de Avisos (Warnings)**
  - Tratar o valor de retorno do fgets na função get_input (cli.c) para silenciar o aviso do compilador
  
- [ ] **Validação Final**
  - Correr valgrind --leak-check=full ./bin/golf para confirmar que não existem fugas de memória