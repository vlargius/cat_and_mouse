#include "out_stream.h"

#include <algorithm>
#include <cstring>

void OutStream::reallocBuffer(size_t new_length) {
    buffer = static_cast<char*>(std::realloc(buffer, new_length));
    //catch realloc errors

    capacity = new_length;
}

void OutStream::serialize(const void* data, size_t byte_count) {
    //check new space
    size_t resultHead = head + static_cast<uint32_t>(byte_count);
    if (resultHead > capacity) {
        reallocBuffer(std::max(capacity*2, resultHead));
    }

    std::memcpy(buffer + head, data, byte_count);

    head = resultHead;
}

void OutBitStream::writeBits(uint8_t data, size_t bit_count) {
    size_t nextBitHead = bitHead + bit_count;
    if (nextBitHead > capacity) {
        reallocBuffer(std::max(capacity*2, nextBitHead));
    }

    size_t byteOffset = bitHead >> 3;
    size_t bitOffset = bitHead & 0x7;

    uint8_t currentMask = ~(0xff << bitOffset);
    buffer[byteOffset] = (buffer[byteOffset] & currentMask)
        | (data << bitOffset);

    size_t bitsFreeThisByte = 8 - bitOffset;

    if (bitsFreeThisByte < bit_count) {
        buffer[byteOffset + 1] = data >> bitsFreeThisByte;
    }

    bitHead = nextBitHead;
}

void OutBitStream::writeBits(const void* data, size_t bit_count) {
    const uint8_t* srcByte = static_cast<const uint8_t*>(data);

    while(bit_count >= 8) {
        writeBits(*srcByte, 8);
        ++srcByte;
        bit_count -= 8;
    }
    if (bit_count > 0) {
        writeBits(*srcByte, bit_count);
    }
}


void OutBitStream::reallocBuffer(size_t new_length) {
    buffer = static_cast<char*>(std::realloc(buffer, new_length));
    //catch realloc errors

    capacity = new_length;
}