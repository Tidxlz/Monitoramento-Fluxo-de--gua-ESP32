# Calibração do sensor YF-S201

## Por que calibrar

O programa converte pulsos em vazão usando um fator de calibração:

```
vazão (L/min) = frequência (Hz) ÷ FATOR_CALIBRACAO
```

O valor padrão, **7,5**, é uma referência comum para o YF-S201, mas cada sensor físico tem pequenas diferenças de fabricação. Além disso, a posição de instalação e a pressão da água influenciam a leitura. Calibrar é descobrir o fator do **seu** sensor.

## Relação entre as grandezas

Se o fator é K, então:

```
pulsos por litro = K × 60
K = pulsos por litro ÷ 60
```

**Exemplo:** com K = 7,5, o sensor gera 7,5 × 60 = **450 pulsos por litro**.

## Materiais

- Sensor YF-S201 montado no circuito
- Recipiente com marcação de volume (jarra medidora de 1 L ou mais)
- Mangueira ligada à entrada do sensor
- Computador com o Monitor Serial aberto

## Procedimento

1. Posicione a saída do sensor sobre o recipiente vazio.
2. Anote o total de pulsos no início (ou reinicie o ESP32 para começar do zero).
3. Abra a água com vazão moderada e constante.
4. Feche a água quando o recipiente atingir exatamente **1 litro**.
5. Anote o total de pulsos contados durante o ensaio.
6. Calcule `pulsos por litro` e `K = pulsos por litro ÷ 60`.
7. Repita pelo menos **3 vezes** e use a média dos resultados.
8. Registre tudo em [`dados/calibracao/modelo-calibracao.csv`](../dados/calibracao/modelo-calibracao.csv).
9. Atualize `FATOR_CALIBRACAO` em `include/config.h` com o valor médio.

> **Observação:** a versão atual do programa zera o contador a cada segundo. Para somar o total de pulsos de um ensaio, está planejado um **modo de calibração** na versão 0.2, que acumula os pulsos até um comando de reset.

## Exemplo de registro

| Ensaio | Volume (L) | Pulsos | Pulsos por litro | K calculado |
|---|---|---|---|---|
| 1 | 1,0 | 462 | 462 | 7,70 |
| 2 | 1,0 | 455 | 455 | 7,58 |
| 3 | 1,0 | 458 | 458 | 7,63 |
| **Média** | | | **458** | **7,64** |

*Valores ilustrativos. Substitua pelos dados reais dos seus ensaios.*

## Avaliando o erro

Para verificar a calibração, faça um novo ensaio com um volume diferente (por exemplo, 2 litros) e compare:

```
erro (%) = (volume medido − volume real) ÷ volume real × 100
```

Um erro de até 5% é aceitável para um protótipo com o YF-S201. Ele tende a ser menos preciso em vazões muito baixas, então vale testar também com a torneira pouco aberta.

## Resultados

*A preencher após os ensaios com o sensor físico.*
