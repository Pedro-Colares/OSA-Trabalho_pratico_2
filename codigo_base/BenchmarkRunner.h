#pragma once

#include "HashCommon.h"
#include <string>

// Estrutura para consolidar os resultados de desempenho coletados no benchmark
struct ResultadoBenchmark {
    std::string estrategia;
    int32_t totalChaves;
    int32_t totalBuckets;
    double tempoBuscasMs;
    uint64_t leiturasLogicas;
    uint64_t escritasLogicas;
    uint64_t colisoes;
    uintmax_t bytesOcupadosDisco;
};

class BenchmarkRunner {
public:
    // Executa a carga de dados, validacao de integridade e afericao de metricas
    static void executar(int32_t totalChaves, int32_t totalBuckets);

private:
    static std::vector<int32_t> gerarChavesReproduziveis(int32_t total);
    static void imprimirTabela(const std::vector<ResultadoBenchmark>& resultados);
};
