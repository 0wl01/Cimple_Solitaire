# TODO

## Urgente
- [ ] A função flip_deal foi removida então todo o código do golf e dos testes precisa ser refatorado.
- [ ] A função eliminate deck agora recebe um ponteiro para outro ponteiro em vez de apenas um ponteiro. Golf e Testes precisam ser refatorados.

## Geral & Interface
- [ ] Criar menu de seleção de jogo (Golf e Simple Simon).
- [ ] Adicionar opções de jogo: Dicas (Hints), Desfazer (Undo) e Reiniciar (Restart).
- [ ] (Opcional) Implementar interface gráfica no terminal usando `ncurses`.

## Lógica do Simple Simon (`simon.c`)
- [ ] Implementar distribuição inicial das cartas (formato em escada decrescente).
- [ ] Criar validação de movimentos (cartas individuais, blocos do mesmo naipe e colunas vazias).
- [ ] Implementar deteção de vitória (sequência completa de Rei a Ás) e mover para as fundações.

## Testes & Qualidade (`CUnit` e `gcov`)
- [ ] Escrever testes unitários para: condições de vitória, derrota e movimentos inválidos.
- [ ] Atualizar a `Makefile` para gerar métricas de cobertura de código com `gcov`.
