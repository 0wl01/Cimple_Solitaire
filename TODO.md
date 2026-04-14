# TODO

## Tarefas Pendentes - Simple Simon (simon.c)
- [ ] **Lógica de Distribuição**
  - Finalizar a lógica de setup para distribuir as 52 cartas pelo padrão decrescente de 10 colunas (8, 8, 8, 7, 6, 5, 4, 3, 2, 1).
- [ ] **Movimentação de Cartas e Blocos**
  - Criar função para validar movimentos de cartas simples (uma carta pode ir sobre outra se for 1 valor abaixo, de qualquer naipe).
  - Criar função para mover blocos inteiros (o bloco tem de estar em sequência decrescente rigorosa e todas as cartas do **mesmo naipe**).
  - Permitir mover qualquer carta/bloco válido para uma coluna que tenha ficado vazia.
- [ ] **Condição de Vitória e Fundações**
  - Criar um detetor automático que verifica se uma coluna completou a sequência de Rei a Ás do mesmo naipe.
  - Implementar a função que retira a sequência completa do tabuleiro e a move para os `foundations[4]`.

## Testes Unitários e Qualidade de Código
- [ ] **Expandir Testes Unitários (CUnit)**
  - Testes de Situação de Vitória (para ambos os jogos).
  - Testes de Situação de Derrota (sem mais movimentos válidos).
  - Testes de Rejeição (garantir que a função `move` no Simon rejeita blocos de naipes mistos).
- [ ] **Test Coverage (gcov)**
  - Atualizar a Makefile para compilar com profiling de cobertura.
  - Correr os testes com `gcov` para documentar as métricas de cobertura para a avaliação.

## Arquitetura e Expansão
- [ ] **Menu Principal Escalável**
  - Sistema de menu com *Function Pointers* para seleção dinâmica entre o Golf e o Simple Simon.
- [ ] **Interface de Utilizador (TUI)**
  - (Opcional) Refatorizar os `printf` do tabuleiro usando a biblioteca `ncurses` para um visual estático e sem "flicker" no terminal.

## Tarefas Pendentes - Motor Base e Solitário Golf (golf.c / card.c)
- [ ] **Regras do Jogo (Golf)**
  - Adicionar uma função para puxar automático 1 carta do deck (stock para o waste).
- [ ] **Corrigir Regra do "1 Return"**
  - Refatorizar as funções `create_deck` e `push` no `card.c` para terem apenas um único `return` no final, cumprindo a regra imposta pelo professor.
  - Refatorizar `game_loop` no `golf.c` para 1 return.
  - Refatorizar `get_input` no `cli.c` para 1 return.
- [x] **Estancar a Fuga de Memória (Memory Leak)**
  - Substituir todas as chamadas `exit(0)` na função `game_loop` por um regresso (return) limpo ao `main`, para que a memória seja libertada antes de o programa fechar.
  - Garantir que a função `clean_golf` é chamada no final da execução para libertar o stock, waste e as 7 colunas.
- [ ] **Adicionar proteção contra Endless Loop**
  - Adicionar proteção e tratamento adequado para atalhos de interrupção (ex: "Ctrl + D" / "Ctrl + C") para evitar loops infinitos no CLI.