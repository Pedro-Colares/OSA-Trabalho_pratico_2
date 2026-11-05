#pragma once

#include "HashCommon.h"

// =============================================================================
// Hashing com Encadeamento Interno (Coalesced Chaining de Buckets):
// - internal_index.bin: arquivo unico contendo:
//   [0 ate M-1]: M Buckets Primarios (Home Buckets) pre-alocados.
//   [M em diante]: Buckets de Overflow anexados ao final quando ocorre transbordo.
// =============================================================================
class HashInternal {
private:
    std::fstream file_;
    int32_t bucketCount_;       // Quantidade M de buckets primarios
    int32_t totalAllocated_;    // M + buckets de overflow ja criados
    Stats stats_;

    Bucket readBucket(int32_t bucketId) {
        Bucket bucket;
        readAt(file_, static_cast<int64_t>(bucketId) * sizeof(Bucket),
               &bucket, sizeof(bucket));
        ++stats_.reads;
        return bucket;
    }

    void writeBucket(int32_t bucketId, const Bucket& bucket) {
        writeAt(file_, static_cast<int64_t>(bucketId) * sizeof(Bucket),
                &bucket, sizeof(bucket));
        ++stats_.writes;
    }

public:
    explicit HashInternal(int32_t buckets) : bucketCount_(buckets), totalAllocated_(buckets) {
        ensure(buckets > 0, "Numero de buckets deve ser positivo");
        file_ = createIndex("internal_index.bin");

        // Inicializa os M buckets primarios como vazios (count = 0, nextBucket = NIL)
        Bucket emptyBucket{};
        emptyBucket.count = 0;
        emptyBucket.nextBucket = NIL;

        for (int32_t b = 0; b < bucketCount_; ++b) {
            writeBucket(b, emptyBucket);
        }
        stats_ = {}; // Zera estatisticas de inicializacao
    }

    // TAREFA DO ALUNO:
    // Insere no bucket primario correspondente.
    // Se o bucket tiver espaco (count < BUCKET_CAPACITY), grava diretamente.
    // Se estiver cheio, navega na cadeia ou anexa novo bucket de overflow no final do arquivo.
    bool insert(int32_t key, int32_t rrn);

    // TAREFA DO ALUNO:
    // Examina o bucket primario e, se necessario, navega na cadeia de overflow no mesmo arquivo.
    int32_t find(int32_t key);

    Stats stats() const { return stats_; }
    void resetStats() { stats_ = {}; }
    int32_t totalAllocated() const { return totalAllocated_; }
};
