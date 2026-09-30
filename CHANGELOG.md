# Changelog

Registro das mudanças do projeto, organizado por versão. Cada versão corresponde a uma etapa do [roadmap](README.md#roadmap).

O formato segue o padrão [Keep a Changelog](https://keepachangelog.com/pt-BR/1.1.0/).

## [Não lançado]

### Planejado
- Modo de calibração com contagem acumulada de pulsos
- Cálculo de volume acumulado

## [0.1.0] - 2026-09-30

Primeira versão funcional: medição de vazão simulada no Wokwi.

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

[Não lançado]: ../../compare/v0.1.0...HEAD
[0.1.0]: ../../releases/tag/v0.1.0
