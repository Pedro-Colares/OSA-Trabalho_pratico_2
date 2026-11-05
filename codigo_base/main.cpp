#include "BenchmarkRunner.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    try {
        // Parametros padrao: 1000 chaves e 2000 buckets (fator de carga alfa = 0.5)
        const int32_t totalChaves  = (argc > 1) ? std::stoi(argv[1]) : 1000;
        const int32_t totalBuckets = (argc > 2) ? std::stoi(argv[2]) : 2000;

        std::cout << "========================================================================\n";
        std::cout << "  IFNMG - Bacharelado em Ciencia da Computacao\n";
        std::cout << "  Organizacao e Sistemas de Arquivos (OSA) - TP02 (Parte 1)\n";
        std::cout << "  Benchmark de Hashing Externo em Disco\n";
        std::cout << "========================================================================\n";

        BenchmarkRunner::executar(totalChaves, totalBuckets);

        return 0;
    } catch (const std::exception& error) {
        std::cerr << "\n[FALHA NA EXECUCAO]: " << error.what() << "\n\n";
        return 1;
    }
}
