#pragma once

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// =============================================================================
// NOTA DIDATICA PARA OS ALUNOS (OSA - IFNMG):
// 1. O tipo int32_t equivale ao tipo 'int' convencional de 4 bytes do C++.
//    Ele garante precisao e tamanho fixo ao gravar structs diretamente em disco.
// 2. Um BUCKET (cesto) agrupa B slots em uma unica unidade fisica contigua de disco.
//    Ao ler um bucket com 1 unica operacao de I/O, todos os seus registros sao
//    carregados para a memoria RAM simultaneamente.
// =============================================================================

// Cada entrada de indice associa a chave de busca (matricula) ao RRN do registro de dados.
struct Slot {
    int32_t key;  // Matricula do aluno (chave de busca)
    int32_t rrn;  // Numero Relativo do Registro no arquivo fixo de dados (alunos_fixo.bin)
};

// Constante que indica ponteiro/posicao nula ou fim de cadeia de overflow
constexpr int32_t NIL = -1;

// Capacidade padrao B de slots agrupados por bucket
constexpr int32_t BUCKET_CAPACITY = 4;

// Estrutura fisica de um Bucket em disco
struct Bucket {
    int32_t count;                       // Quantidade de registros ocupados atualmente (0 ate B)
    Slot slots[BUCKET_CAPACITY];         // B slots contiguos armazenados no mesmo bloco
    int32_t nextBucket;                  // Indice do proximo bucket na cadeia de overflow ou NIL (-1)
};

// Funcao Hash pelo Metodo da Divisao: h(chave) = chave mod M
inline int32_t hashKey(int32_t key, int32_t buckets) {
    return (key % buckets + buckets) % buckets;
}

// Assercoes de integridade
inline void ensure(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// Operacoes de E/S binaria posicionada em disco
inline void writeAt(std::fstream& file, std::streamoff offset,
                    const void* data, std::size_t size) {
    file.clear();
    file.seekp(offset, std::ios::beg);
    file.write(reinterpret_cast<const char*>(data), size);
    ensure(static_cast<bool>(file), "Falha na escrita posicionada em disco");
}

inline void readAt(std::fstream& file, std::streamoff offset,
                   void* data, std::size_t size) {
    file.clear();
    file.seekg(offset, std::ios::beg);
    file.read(reinterpret_cast<char*>(data), size);
    ensure(static_cast<bool>(file), "Falha na leitura posicionada em disco");
}

inline std::fstream createIndex(const std::string& filename) {
    {
        std::ofstream output(filename, std::ios::binary | std::ios::trunc);
        ensure(static_cast<bool>(output), "Falha ao criar o arquivo binario");
    }
    std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
    ensure(static_cast<bool>(file), "Falha ao abrir o arquivo binario para I/O");
    return file;
}

// Metricas de avaliacao de desempenho
struct Stats {
    uint64_t reads = 0;       // Quantidade de leituras logicas de buckets em disco
    uint64_t writes = 0;      // Quantidade de escritas logicas de buckets em disco
    uint64_t overflows = 0;   // Quantidade de transbordamentos (quando um bucket enche e precisa encadear)
};
