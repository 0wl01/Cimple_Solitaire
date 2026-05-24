# TODO

- [ ] Programa deve abortar se por alguma razão alocar memória falhe (tecnicamente impossível em um sistema moderno);
- [ ] Algumas funções precisam de atenção urgente.
    - populate_entries
    - show_main_menu
    - first_scan
    - try_auto_rule
    - check_single_flag
    - scan_game_file
    - print_menu_page
    - load_files
    - parse_game_rules
- [ ] Ainda precisamos do SAVE e LOAD
- [ ] Ainda precisamos do UNDO

## Bugs
- [x] No simon.c a função que verifica se há movimentos possíveis não considera colunas vazias.
    - Para resolver isso temos que mudar o card.c one_less para nn verificar cartas vazias e ent modificar o has_play_left para verificar colunas vazias.

## Documentação
- [X] Algumas funções novas precisam de documentação.
    - Lembrar que funções static tem a documentação escrita no arquivo source (.c) e as outras no header.
- [ ] Documentar a função de cada arquivo (talvez renomear eles).

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

## Testes & Qualidade (`CUnit` e `gcov`)
- [X] Escrever testes unitários para: condições de vitória, derrota e movimentos inválidos.
- [X] Atualizar a `Makefile` para gerar métricas de cobertura de código com `gcov`.
- [X] Testes precisam ser refatorados para por conta de algumas breaking changes em card.c
- [X] CUnit Setup / Teardown refatorar funções de testes que usem instruções para criação de contexto.
- [X] FAz testes melhores seu burro
- [ ] Testar leitura de ficheiros de texti (test_dsl.c)
- [ ] Testar a alocação (test_registry.c)
- [ ] Testar a validação de regras (test_dsl_game.c)

## Otimização
- [x] Criar tipo próprio **size** como um uint16 para substituir o size_t (economizando 48 bits).
- [ ] Muda cards de um bitfield para apenas masks. (melhor para construir uma função de save)
- [ ] É preciso melhorar como os decks são feitos.


## Finalizado
- [x] Implementar distribuição inicial das cartas (formato em escada decrescente).
- [x] Criar validação de movimentos (cartas individuais, blocos do mesmo naipe e colunas vazias).
- [x] Implementar deteção de vitória (sequência completa de Rei a Ás) e mover para as fundações.
- [x] Refatorar golf.c por causa das breaking changes.
- [x] Resolver o loop infinito que ocorre com o end of input (CTRL-D).
- [X] Segmentation Fault em simon.c
    - check "if (cmd.src_col < 'A' || cmd.src_col > 'Z') return 0;"
- [X] Criar Função para Mover para as Fundações
    - Senão o jogo vai continuar impossível de ganhar
