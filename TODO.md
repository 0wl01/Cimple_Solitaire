# TODO

## Urgente
- [x] Refatorar golf.c por causa das breaking changes.
- [x] Resolver o loop infinito que ocorre com o end of input (CTRL-D).
- [X] Segmentation Fault em simon.c
    - check "if (cmd.src_col < 'A' || cmd.src_col > 'Z') return 0;"
- [X] Criar Função para Mover para as Fundações
    - Senão o jogo vai continuar impossível de ganhar

### Bugs
- [ ] No simon.c a função que verifica se há movimentos possíveis não considera colunas vazias.
    - Para resolver isso temos que mudar o card.c one_less para nn verificar cartas vazias e ent modificar o has_play_left para verificar colunas vazias.

## Documentação
- [X] Algumas funções novas precisam de documentação.
    - Lembrar que funções static tem a documentação escrita no arquivo source (.c) e as outras no header.

## Geral & Interface
- [x] Criar menu de seleção de jogo (Golf e Simple Simon).
- [ ] Escrever texto de ajuda para o simple simon.
- [ ] Adicionar opções de jogo: Dicas (Hints), Desfazer (Undo) e Reiniciar (Restart).
    - [x] Reiniciar.
    - [ ] Undo.
    - [ ] Dicas. 
        - Dicas é um pouco mais fácil de fazer que o undo. Apenas deve dizer quais são os possíveis movimentos atuais.
- [ ] Pintar cartas vermelhas de vermelho.
- [ ] Limpar a terminal a cada loop do jogo.
- [ ] (Opcional) Implementar interface gráfica no terminal usando `ncurses`.

## Lógica do Simple Simon (`simon.c`)
- [x] Implementar distribuição inicial das cartas (formato em escada decrescente).
- [x] Criar validação de movimentos (cartas individuais, blocos do mesmo naipe e colunas vazias).
- [x] Implementar deteção de vitória (sequência completa de Rei a Ás) e mover para as fundações.

## Testes & Qualidade (`CUnit` e `gcov`)
- [X] Escrever testes unitários para: condições de vitória, derrota e movimentos inválidos.
- [X] Atualizar a `Makefile` para gerar métricas de cobertura de código com `gcov`.
- [X] Testes precisam ser refatorados para por conta de algumas breaking changes em card.c
