# Relatório de benchmark: hashing em disco

### Gustavo Tavares de Sousa e Pedro Pereira Gonçalves Colares

## Objetivo e metodologia

Foram comparadas as implementações de hashing com encadeamento externo e
encadeamento interno para `N = 1000` chaves, capacidade `B = 4` registros por
bucket e três quantidades de buckets primários (`M`). As chaves são geradas de
forma reprodutível. As leituras e os tempos apresentados correspondem às buscas
de todas as chaves inseridas e de 100 chaves inexistentes; portanto, não medem
as operações de inserção.

Há uma diferença de notação a considerar: o README define o fator de carga como
`α = N/M`, enquanto o programa imprime `N/(M·B)`, isto é, a ocupação média em
relação à capacidade total dos buckets. A tabela abaixo mostra ambas as
medidas; a coluna “Fator impresso” corresponde ao valor exibido pelo programa.

| Estratégia | N | M | α = N/M (README) | Fator impresso N/(M·B) | Tempo de busca (ms) | Leituras lógicas | Overflows | Tamanho em disco (B) |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Externo | 1000 | 2000 | 0,50 | 0,12 | 2,905374 | 2157 | 0 | 48000 |
| Interno | 1000 | 2000 | 0,50 | 0,12 | 1,038087 | 1100 | 0 | 80000 |
| Externo | 1000 | 1000 | 1,00 | 0,25 | 3,095017 | 2200 | 0 | 44000 |
| Interno | 1000 | 1000 | 1,00 | 0,25 | 1,603338 | 1100 | 0 | 40000 |
| Externo | 1000 | 500 | 2,00 | 0,50 | 2,754480 | 2200 | 0 | 22000 |
| Interno | 1000 | 500 | 2,00 | 0,50 | 1,215634 | 1100 | 0 | 20000 |

## Discussão

### Efeito do fator de carga nas leituras

Pela definição do README, `α` aumenta de `0,50` para `2,00` à medida que `M`
diminui. Apesar disso, a capacidade total é `M·B`; assim, mesmo no caso de
`M = 500`, há espaço para até 2000 registros. Isso ajuda a explicar por que
nenhuma configuração apresentou overflow.

No encadeamento externo, as leituras passaram de 2157 para 2200 e se mantiveram
nesse valor. Essa organização precisa consultar o vetor de cabeças e, quando a
lista do bucket existe, ler o bucket correspondente. Com `M = 2000`, há mais
posições vazias e algumas buscas por chaves inexistentes terminam após a
consulta da cabeça; com `M = 1000` e `M = 500`, as 2200 leituras correspondem
ao padrão de duas leituras nas 1100 buscas realizadas. No encadeamento interno,
foram registradas 1100 leituras em todos os casos: neste conjunto de dados,
cada busca exigiu uma leitura de bucket e não houve necessidade de percorrer
overflow. Os resultados mostram que, para esta execução, o aumento da carga
não elevou as leituras internas, enquanto as externas aumentaram ligeiramente
entre `M = 2000` e `M = 1000`.

### Quando o encadeamento interno é mais vantajoso

Neste benchmark, o encadeamento interno foi mais vantajoso em número de
leituras em todas as configurações e também apresentou menor tempo medido:
1,04–1,60 ms, contra 2,75–3,10 ms no externo. A diferença é coerente com o
menor número de acessos lógicos observado. Esse resultado é especialmente
favorável quando buckets primários pré-alocados cabem no orçamento de disco e
as buscas podem ser resolvidas sem percorrer cadeias de overflow. Porém, os
tempos são de uma única execução e dependem de fatores como cache e sistema;
não devem ser tratados como uma medição estatística conclusiva.

### Espaço em disco

O encadeamento externo ocupou menos espaço nos três cenários: 48.000 contra
80.000 bytes para `M = 2000`, 44.000 contra 40.000 bytes para `M = 1000` e
22.000 contra 20.000 bytes para `M = 500`. Portanto, **o interno foi menor**
nos dois últimos casos, enquanto **o externo foi menor** quando `M = 2000`.
Isso ocorre porque o interno pré-aloca todos os `M` buckets; o externo mantém
um vetor de cabeças, mas armazena buckets apenas para cadeias utilizadas. Com
muitos buckets primários em relação às chaves, essa alocação sob demanda pode
compensar o espaço adicional das cabeças. Quando todos ou quase todos os
buckets são utilizados, o vetor de cabeças acrescenta espaço sem reduzir
suficientemente a quantidade de buckets armazenados.

## Conclusão

Para os dados testados, o encadeamento interno realizou menos leituras e teve
menores tempos de busca, sem overflows. O consumo de disco dependeu de `M`: o
interno foi ligeiramente menor em `M = 1000` e `M = 500`, mas o externo foi
consideravelmente menor em `M = 2000`, por evitar a pré-alocação de buckets
vazios. As conclusões valem para as chaves, parâmetros e buscas desta
execução; outras distribuições ou cadeias com overflow podem alterar os
resultados.
