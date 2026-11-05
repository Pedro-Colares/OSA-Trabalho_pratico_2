# Orientações para os Alunos - TP02 (Parte 1)

## 📌 Missão
Sua tarefa é implementar as operações fundamentais de **inserção** e **busca** de índices em disco para duas estruturas de Hashing Externo:
1. `HashExternal.cpp` (`insert` e `find`)
2. `HashInternal.cpp` (`insert` e `find`)

## ⚙️ Compilação e Execução
Execute no terminal (Linux/WSL/MinGW/PowerShell com g++):
```bash
make
./tp02 1000 2000
./tp02 1000 1000
./tp02 1000 500
```

## 📊 O que preencher no seu Relatório (README.md ou PDF)
Apresente a tabela preenchida com as saídas obtidas e uma breve discussão (1 a 2 páginas) respondendo:
1. Como o fator de carga $\alpha = N/M$ afetou o número de leituras lógicas em cada método?
2. Em qual situação o Encadeamento Interno é mais vantajoso que o Externo?
3. Qual método consumiu menos bytes de armazenamento em disco e por quê?
