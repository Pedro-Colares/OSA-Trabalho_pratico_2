#include "BenchmarkRunner.h"
#include "HashExternal.h"
#include "HashInternal.h"

#include <algorithm>
#include <filesystem>
#include <random>

std::vector<int32_t> BenchmarkRunner::gerarChavesReproduziveis(int32_t total) {
    std::vector<int32_t> keys;
    keys.reserve(total);
    for (int32_t i = 0; i < total; ++i) {
        keys.push_back(100000 + i * 7);
    }
    std::mt19937 generator(2026);
    std::shuffle(keys.begin(), keys.end(), generator);
    return keys;
}

void BenchmarkRunner::imprimirTabela(const std::vector<ResultadoBenchmark>& resultados) {
    std::cout << "\n================================================================================================\n";
    std::cout << "          RELATORIO DE BENCHMARK - HASHING EXTERNO COM BUCKETS (CAPACIDADE B = " 
              << BUCKET_CAPACITY << ")\n";
    std::cout << "================================================================================================\n";
    std::cout << std::left 
              << std::setw(23) << "Estrategia"
              << std::setw(10) << "N (Chaves)"
              << std::setw(10) << "M (Buckets)"
              << std::setw(11) << "Fator (a)"
              << std::setw(15) << "Tempo Busca"
              << std::setw(12) << "Leituras"
              << std::setw(12) << "Overflows"
              << std::setw(12) << "Tam. Disco"
              << "\n";
    std::cout << "------------------------------------------------------------------------------------------------\n";

    for (const auto& r : resultados) {
        // Fator de carga densidade alfa = N / (M * B)
        double alfa = static_cast<double>(r.totalChaves) / (r.totalBuckets * BUCKET_CAPACITY);
        std::cout << std::left 
                  << std::setw(23) << r.estrategia
                  << std::setw(10) << r.totalChaves
                  << std::setw(10) << r.totalBuckets
                  << std::fixed << std::setprecision(2) << std::setw(11) << alfa
                  << std::setprecision(3) << (std::to_string(r.tempoBuscasMs) + " ms")
                  << std::setw(6) << ""
                  << std::setw(12) << r.leiturasLogicas
                  << std::setw(12) << r.colisoes
                  << (std::to_string(r.bytesOcupadosDisco) + " B")
                  << "\n";
    }
    std::cout << "================================================================================================\n\n";
}

void BenchmarkRunner::executar(int32_t totalChaves, int32_t totalBuckets) {
    ensure(totalChaves >= 10 && totalBuckets > 0, "Parametros de benchmark invalidos");

    std::cout << "\n[1/4] Gerando chaves reprodutíveis (N = " << totalChaves 
              << ", M = " << totalBuckets << ", Capacidade B = " << BUCKET_CAPACITY << ")...\n";
    const auto keys = gerarChavesReproduziveis(totalChaves);

    std::vector<ResultadoBenchmark> resultados;

    // 1. Encadeamento Externo
    {
        std::cout << "[2/4] Executando Hashing com Encadeamento Externo de Buckets...\n";
        HashExternal external(totalBuckets);

        for (int32_t rrn = 0; rrn < totalChaves; ++rrn) {
            ensure(external.insert(keys[rrn], rrn), "Erro: Insercao externa rejeitada");
        }

        // Teste de duplicata
        ensure(!external.insert(keys[0], 999999), "Erro: Duplicata externa aceita indevidamente!");

        external.resetStats();
        const auto start = std::chrono::steady_clock::now();
        for (int32_t rrn = 0; rrn < totalChaves; ++rrn) {
            ensure(external.find(keys[rrn]) == rrn, "Erro: Busca externa falhou para chave existente!");
        }
        for (int32_t i = 0; i < 100; ++i) {
            ensure(external.find(900000000 + i) == NIL, "Erro: Busca externa achou chave inexistente!");
        }
        const auto end = std::chrono::steady_clock::now();

        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
        uintmax_t bytesTotal = std::filesystem::file_size("external_heads.bin")
                             + std::filesystem::file_size("external_buckets.bin");

        resultados.push_back({
            "Encadeamento Externo",
            totalChaves,
            totalBuckets,
            elapsed,
            external.stats().reads,
            external.stats().writes,
            external.stats().overflows,
            bytesTotal
        });
    }

    // 2. Encadeamento Interno
    {
        std::cout << "[3/4] Executando Hashing com Encadeamento Interno de Buckets...\n";
        HashInternal internal(totalBuckets);

        for (int32_t rrn = 0; rrn < totalChaves; ++rrn) {
            ensure(internal.insert(keys[rrn], rrn), "Erro: Insercao interna rejeitada");
        }

        // Teste de duplicata
        ensure(!internal.insert(keys[0], 999999), "Erro: Duplicata interna aceita indevidamente!");

        internal.resetStats();
        const auto start = std::chrono::steady_clock::now();
        for (int32_t rrn = 0; rrn < totalChaves; ++rrn) {
            ensure(internal.find(keys[rrn]) == rrn, "Erro: Busca interna falhou para chave existente!");
        }
        for (int32_t i = 0; i < 100; ++i) {
            ensure(internal.find(900000000 + i) == NIL, "Erro: Busca interna achou chave inexistente!");
        }
        const auto end = std::chrono::steady_clock::now();

        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
        uintmax_t bytesTotal = std::filesystem::file_size("internal_index.bin");

        resultados.push_back({
            "Encadeamento Interno",
            totalChaves,
            totalBuckets,
            elapsed,
            internal.stats().reads,
            internal.stats().writes,
            internal.stats().overflows,
            bytesTotal
        });
    }

    std::cout << "[4/4] Consolidando estatisticas e formatando relatorio...\n";
    imprimirTabela(resultados);
}
