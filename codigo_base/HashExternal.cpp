#include "HashExternal.h"

bool HashExternal::insert(int32_t key, int32_t rrn) {
    int b = hashKey(key, bucketCount_);
    int head = readHead(b);
    if(head==NIL){
        Bucket novoBucket;
        novoBucket.count = 1;
        novoBucket.slots[0] = {key, rrn};
        novoBucket.nextBucket = NIL;
        writeBucket(totalBuckets_, novoBucket);
        writeHead(b, totalBuckets_++);

        return true;
    }

    int32_t current = head;
    int32_t lastBucket = NIL;
    int32_t bucketDisponivel = NIL;
    while(current != NIL){
        Bucket atual = readBucket(current);
        for(int32_t i=0; i<atual.count; i++){
            if(atual.slots[i].key==key)return false;
        }

        if(bucketDisponivel == NIL && atual.count < BUCKET_CAPACITY)bucketDisponivel = current;

        lastBucket = current;
        current = atual.nextBucket;
    }

    if(bucketDisponivel != NIL){
        Bucket atual = readBucket(bucketDisponivel);
        atual.slots[atual.count++] = {key, rrn};
        writeBucket(bucketDisponivel, atual);

        return true;
    }

    stats_.overflows++;
    Bucket novo;

    novo.count = 1;
    novo.slots[0] = {key, rrn};
    novo.nextBucket = NIL;
    writeBucket(totalBuckets_, novo);
    
    Bucket ultimo = readBucket(lastBucket);
    ultimo.nextBucket = totalBuckets_++;
    writeBucket(lastBucket, ultimo);

    return true;
}

int32_t HashExternal::find(int32_t key) {

    int32_t b = hashKey(key, bucketCount_);
    int32_t head = readHead(b);
    if(head==NIL)return -1;
    int currBucket = head;
    while(currBucket != NIL){
        Bucket atual = readBucket(currBucket);
        for(int32_t i=0; i<atual.count; i++){
            if(atual.slots[i].key == key)return atual.slots[i].rrn;
        }

        currBucket = atual.nextBucket;
    }

    return NIL; 
}
