# TODO

## Urgente
- [x] Refator golf.c por causa das breaking changes.
- [x] Resolver o loop infinito que ocorre com o end of input (CTRL-D).

## Documentação
- [ ] Algumas funções novas precisam de documentação.
    - Lembrar que funções static tem a documentação escrita no arquivo source (.c) e as outras no header.

## Geral & Interface
- [x] Criar menu de seleção de jogo (Golf e Simple Simon).
- [ ] Adicionar opções de jogo: Dicas (Hints), Desfazer (Undo) e Reiniciar (Restart).
- [ ] (Opcional) Implementar interface gráfica no terminal usando `ncurses`.

## Lógica do Simple Simon (`simon.c`)
- [x] Implementar distribuição inicial das cartas (formato em escada decrescente).
- [ ] Criar validação de movimentos (cartas individuais, blocos do mesmo naipe e colunas vazias).
- [ ] Implementar deteção de vitória (sequência completa de Rei a Ás) e mover para as fundações.

## Testes & Qualidade (`CUnit` e `gcov`)
- [ ] Escrever testes unitários para: condições de vitória, derrota e movimentos inválidos.
- [ ] Atualizar a `Makefile` para gerar métricas de cobertura de código com `gcov`.
- [ ] Testes precisam ser refatorados para por conta de algumas breaking changes em card.c