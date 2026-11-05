#include "HashInternal.h"

bool HashInternal::insert(int32_t key, int32_t rrn) {

    int32_t b = hashKey(key, bucketCount_);
    int32_t lastBucket = NIL;
    int32_t bucketDisponivel = NIL;
    while(b!=NIL){
        Bucket bucket = readBucket(b);
        for(int32_t i=0; i<bucket.count; i++){
            if(bucket.slots[i].key==key)return false;
        }

        if(bucketDisponivel==NIL && bucket.count < BUCKET_CAPACITY)bucketDisponivel = b;

        lastBucket = b;
        b = bucket.nextBucket;

    }

    if(bucketDisponivel!=NIL){
        Bucket bucket = readBucket(bucketDisponivel);
        bucket.slots[bucket.count++] = {key, rrn};
        writeBucket(bucketDisponivel, bucket);
        return true;
    }
    
    stats_.overflows++;
    int32_t novoIndex = totalAllocated_++;
    Bucket novo{};

    novo.count = 1;
    novo.slots[0] = {key, rrn};
    novo.nextBucket = NIL;
    writeBucket(novoIndex, novo);
    
    Bucket ultimo = readBucket(lastBucket);
    ultimo.nextBucket = novoIndex;
    writeBucket(lastBucket, ultimo);

    return true;
}

int32_t HashInternal::find(int32_t key) {
    int32_t current = hashKey(key, bucketCount_);

    while (current != NIL) {
        Bucket bucket = readBucket(current);

        for (int32_t i = 0; i < bucket.count; ++i) {
            if (bucket.slots[i].key == key)return bucket.slots[i].rrn;
            
        }

        current = bucket.nextBucket;
    }

    return NIL;
}
