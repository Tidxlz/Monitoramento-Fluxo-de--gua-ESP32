# Changelog

Registro das mudanças do projeto. Cada versão corresponde a uma etapa concluída do [roadmap](README.md#roadmap).

O formato segue o padrão [Keep a Changelog](https://keepachangelog.com/pt-BR/1.1.0/). Mudanças ainda não incluídas em uma versão ficam na seção **Não lançado**.

## [Não lançado]

A primeira versão está em validação. Os itens abaixo foram implementados, mas só entram na **v0.1.0** depois de testados na simulação.

### Adicionado
- Contagem de pulsos do YF-S201 por interrupção no GPIO27
- Cálculo de frequência (Hz) com correção pelo tempo real decorrido
- Cálculo de vazão (L/min) com fator de calibração 7,5
- Exibição de pulsos, frequência, vazão e alerta no OLED SSD1306
- LED de alerta quando a vazão passa de 5 L/min
- Simulação do sensor com gerador de pulsos (GPIO26) e potenciômetro (GPIO34)
- Compatibilidade com as versões 2.x e 3.x do core do ESP32
- Projeto PlatformIO com simulação pelo Wokwi para VS Code
- Configurações centralizadas em `include/config.h`
- Compilação automática com GitHub Actions
- Documentação técnica, registro de decisões, procedimento de calibração e lista de materiais
- Registro de validação por etapa em `docs/validacao.md`

### Corrigido
- Removida a tag `v0.1.0`, criada antes de a simulação ser testada
- Quebra de linha do Monitor Serial no terminal do Wokwi (`\r\n` no lugar de `\n`)
- Fios do OLED atravessando a tela na simulação