#pragma once

#include "HashCommon.h"

// =============================================================================
// Hashing com Encadeamento Externo (Separate Chaining de Buckets):
// - external_heads.bin: vetor de M inteiros (int32_t), onde a posicao b
//   armazena o indice do primeiro Bucket do bucket b, ou NIL (-1).
// - external_buckets.bin: arquivo com todos os Buckets (primarios e de overflow).
// =============================================================================
class HashExternal {
private:
    std::fstream headsFile_;
    std::fstream bucketsFile_;
    int32_t bucketCount_;
    int32_t totalBuckets_ = 0;
    Stats stats_;

    int32_t readHead(int32_t bucketIndex) {
        int32_t head;
        readAt(headsFile_, static_cast<int64_t>(bucketIndex) * sizeof(int32_t),
               &head, sizeof(head));
        ++stats_.reads;
        return head;
    }

    void writeHead(int32_t bucketIndex, int32_t head) {
        writeAt(headsFile_, static_cast<int64_t>(bucketIndex) * sizeof(int32_t),
                &head, sizeof(head));
        ++stats_.writes;
    }

    Bucket readBucket(int32_t bucketId) {
        Bucket bucket;
        readAt(bucketsFile_, static_cast<int64_t>(bucketId) * sizeof(Bucket),
               &bucket, sizeof(bucket));
        ++stats_.reads;
        return bucket;
    }

    void writeBucket(int32_t bucketId, const Bucket& bucket) {
        writeAt(bucketsFile_, static_cast<int64_t>(bucketId) * sizeof(Bucket),
                &bucket, sizeof(bucket));
        ++stats_.writes;
    }

public:
    explicit HashExternal(int32_t buckets) : bucketCount_(buckets) {
        ensure(buckets > 0, "Numero de buckets deve ser positivo");
        headsFile_ = createIndex("external_heads.bin");
        bucketsFile_ = createIndex("external_buckets.bin");

        // Inicializa a tabela de cabecas com NIL (-1)
        for (int32_t b = 0; b < bucketCount_; ++b) {
            writeHead(b, NIL);
        }
        stats_ = {}; // Zera estatisticas de inicializacao
    }

    // TAREFA DO ALUNO:
    // Insere (key, rrn) no bucket correspondente.
    // Se o bucket tiver espaco livre (< BUCKET_CAPACITY), insere diretamente.
    // Se estiver cheio, navega pela cadeia de nextBucket ou anexa novo bucket de overflow.
    // Rejeita duplicatas retornando false.
    bool insert(int32_t key, int32_t rrn);

    // TAREFA DO ALUNO:
    // Localiza o bucket home e percorre seus slots.
    // Se nao encontrar e houver overflow, segue nextBucket ate achar ou alcancar NIL.
    int32_t find(int32_t key);

    Stats stats() const { return stats_; }
    void resetStats() { stats_ = {}; }
    int32_t totalBuckets() const { return totalBuckets_; }
};
