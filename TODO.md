# TODO

- Gotta make all vars type signatures in the code more uniform.
## Tarefas Pendentes - Lógica do Jogo (golf.c)

- [ ] **Corrigir Regra do "1 Return" (card.c)**
  - Refatorizar as funções `create_deck` e `push` para terem apenas um único `return` no final, cumprindo a regra imposta pelo professor.
  
- [ ] **Criar a Suite de Testes do Jogo**
  - `test_golf.c` continuar a criar testes, para testar as regras do jogo e garantir 100% de cobertura da lógica.
  - Atualizar o `Makefile` e/ou o comando de compilação manual para incluir os novos testes.

- [ ] **Estancar a Fuga de Memória (Memory Leak)**
  - Criar uma função `clean_golf(golf_state *table)` que faça `eliminate_deck` ao `stock`, ao `waste` e às 7 colunas.
  - Substituir todas as chamadas `exit(0)` na função `game_loop` por um regresso (return) limpo ao `main`, para que a memória seja libertada antes de o programa fechar.

- [ ] **Documentação Doxygen**
  - Substituir os comentários `// TODO: Docs` por blocos Doxygen `/** ... */` nas funções do `golf.c` e na `struct golf_state` do `golf.h`.

- [ ] **Verificar redundâncias nos Testes**
  - Verificar redundâncias e separar grouptests em funções singulares .