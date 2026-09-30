# Registro de decisões do projeto

Este documento explica **por que** cada escolha técnica foi feita. Cada decisão traz o contexto, a escolha e as alternativas consideradas.

---

## 1. Contar os pulsos com interrupção

**Contexto:** o YF-S201 gera um pulso a cada fração de volta do rotor. Com vazão alta, podem chegar dezenas de pulsos por segundo.

**Decisão:** usar `attachInterrupt()` no GPIO27. A cada pulso, o hardware chama a função `countPulse()`, que apenas soma 1 ao contador.

**Alternativa considerada:** ler o pino continuamente dentro do `loop()` (polling). Foi descartada porque, enquanto o ESP32 estivesse ocupado atualizando o display, pulsos poderiam ser perdidos.

---

## 2. Detectar a borda de subida (RISING)

**Decisão:** a interrupção dispara quando o sinal passa de LOW para HIGH.

**Motivo:** cada pulso tem exatamente uma subida. Contar nas duas bordas (CHANGE) dobraria a contagem sem trazer informação nova.

---

## 3. Contador declarado como `volatile`

**Decisão:** `volatile unsigned long pulses`.

**Motivo:** a variável é alterada dentro da interrupção. O `volatile` impede que o compilador guarde uma cópia antiga dela e garante que o `loop()` sempre leia o valor real.

---

## 4. Copiar e zerar o contador com as interrupções desligadas

**Decisão:** envolver a cópia e o reset do contador entre `noInterrupts()` e `interrupts()`.

**Motivo:** se um pulso chegasse exatamente entre a cópia e o reset, ele seria apagado sem ser contado. Desligar as interrupções por alguns microssegundos elimina esse risco.

---

## 5. Usar `millis()` em vez de `delay()`

**Decisão:** medir a cada 1 segundo verificando `millis() - lastMeasure >= INTERVALO_MS`.

**Motivo:** o `delay()` congela o programa. Com `millis()`, o `loop()` continua livre para outras tarefas, como atualizar a simulação ou, no futuro, ler o sensor de nível e enviar dados por Wi-Fi.

---

## 6. Calcular a frequência com o tempo real decorrido

**Decisão:** `frequência = pulsos × 1000 / dt`, onde `dt` é o tempo realmente passado desde a última medição.

**Motivo:** o intervalo nunca é exatamente 1000 ms. Se o `loop()` atrasar alguns milissegundos, dividir por 1 segundo fixo gera um pequeno erro. Usar o tempo real corrige isso.

---

## 7. Escolha dos pinos

| GPIO | Função | Motivo |
|---|---|---|
| 21 e 22 | SDA e SCL do OLED | São os pinos I2C padrão do ESP32 |
| 27 | Sinal do YF-S201 | Aceita interrupção e não interfere na inicialização da placa |
| 25 | LED | Saída digital livre, sem função especial no boot |
| 26 | Gerador de pulsos (simulação) | Saída compatível com o PWM (LEDC) do ESP32 |
| 34 | Potenciômetro (simulação) | Pino de entrada do ADC1, que continua funcionando com o Wi-Fi ligado (o ADC2 não funciona com Wi-Fi) |

---

## 8. Simular o sensor com um gerador de pulsos

**Contexto:** o Wokwi não possui o YF-S201.

**Decisão:** o próprio ESP32 gera uma onda quadrada no GPIO26 usando o periférico LEDC, e um fio leva esse sinal ao GPIO27. Um potenciômetro controla a frequência, funcionando como uma torneira virtual.

**Motivo:** para o programa, o que importa é o sinal elétrico. Os pulsos simulados entram pelo mesmo pino e pela mesma interrupção que os pulsos reais, então a lógica testada na simulação é exatamente a que roda com o sensor físico.

**Alternativas consideradas:** um botão (cada clique seria um pulso, mas não permite simular vazões realistas) e um chip customizado do Wokwi (mais fiel, porém mais complexo para esta etapa).

---

## 9. Separar a simulação com `MODO_SIMULACAO`

**Decisão:** todo o código de simulação fica dentro de `#if MODO_SIMULACAO`.

**Motivo:** para usar o sensor real basta trocar um único valor em `config.h`. O código de simulação nem chega a ser compilado nesse caso.

---

## 10. Fator de calibração 7,5 como ponto de partida

**Decisão:** usar `vazão = frequência / 7,5`.

**Motivo:** é a referência mais citada para o YF-S201. Não é uma constante exata, por isso fica em `config.h` para ser ajustada após a calibração (ver [calibracao.md](calibracao.md)).

---

## 11. Limite de alerta de 5 L/min

**Decisão:** o LED acende quando a vazão passa de 5 L/min.

**Motivo:** é um valor de demonstração, no meio da faixa simulada (0 a 10 L/min). O limite real deve ser definido de acordo com a aplicação.

---

## 12. PlatformIO e Wokwi para VS Code

**Contexto:** no Wokwi pelo navegador, a compilação entra em uma fila compartilhada, que pode demorar muito em horários de pico.

**Decisão:** compilar localmente com o PlatformIO e simular com a extensão Wokwi para VS Code.

**Motivo:** elimina a fila de compilação, permite controle de versão com Git e facilita a compilação automática no GitHub Actions.

---

## 13. Compatibilidade com as versões 2.x e 3.x do core do ESP32

**Decisão:** detectar a versão com `ESP_ARDUINO_VERSION_MAJOR` e usar a API do LEDC correspondente.

**Motivo:** o Wokwi pelo navegador e o PlatformIO podem usar versões diferentes do core, e a API do LEDC mudou entre elas. Com a detecção, o mesmo código compila nos dois ambientes.

---

## 14. Configurações em `config.h`

**Decisão:** pinos e parâmetros ficam em `include/config.h`, separados da lógica em `src/main.cpp`.

**Motivo:** quem for calibrar o sensor ou mudar um pino mexe em um arquivo pequeno, sem risco de alterar a lógica do programa.
