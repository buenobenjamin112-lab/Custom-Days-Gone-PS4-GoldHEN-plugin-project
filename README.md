# DaysGoneSpawner — etapa 1

Projeto-base de um plugin GoldHEN para PS4. **Esta versão ainda NÃO é um mod menu e NÃO cria hordas.**
Ela contém o ciclo de vida padrão de um plugin GoldHEN (carregar/descarregar) e uma notificação de teste.

## Alvo planejado
- Jogo: Days Gone
- Title ID: CUSA08966
- Versão informada: 1.81
- Console: PS4 com GoldHEN

## O que esta etapa faz
- Declara os metadados do plugin.
- Mostra uma notificação ao carregar.
- Escreve mensagens no log do GoldHEN ao carregar/descarregar.

## O que ainda falta
1. Integrar o GoldHEN_Plugins_SDK completo.
2. Compilar com OpenOrbis PS4 Toolchain e LLVM compatível.
3. Testar o carregamento/descarregamento no PS4.
4. Só depois pesquisar a versão 1.81 do jogo para descobrir, com evidências, como acessar entidades e spawn.
5. Criar o menu visual e os controles.

Não há endereços de memória, hooks de jogo ou rotinas de spawn neste projeto. Não instale como se fosse um mod funcional.

## Dependências oficiais
- GoldHEN Plugin SDK: https://github.com/GoldHEN/GoldHEN_Plugins_SDK
- Exemplos oficiais de plugins: https://github.com/GoldHEN/GoldHEN_Plugins_Repository/tree/main/plugin_src
- OpenOrbis Toolchain: https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain/releases

O SDK oficial orienta incluir `source/` e `include/` do próprio SDK no projeto e requer OpenOrbis Toolchain e LLVM 10+.
