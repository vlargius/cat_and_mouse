#include "in_stream.h"

#include <algorithm>

template <class T>
void InStream::serialize(T& data) {
  static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
                "write only supports primitive data types");

  read(&data, sizeof(data));
}

template<class T>
void InStream::serialize(std::vector<T>& vector) {
    size_t size;
    serialize(size);
    vector.resize(size);
    for (T& element : vector) {
        serialize(element);
    }
}

void InStream::serialize(void* data, size_t byte_count) {
    //catch new space
    size_t resultHead = head + byte_count*8;

    if (resultHead <= capacity) {
        //copy into data
        std::memcpy(data, buffer + head, byte_count);
    }

    head = resultHead;
}

void InBitStream::readBits(uint8_t& data, size_t bit_count) {

    size_t byteOffset = bitHead >> 3;
    size_t bitOffset = bitHead & 0x7;

    data = static_cast<uint8_t>(buffer[byteOffset]) >> bitOffset;

    size_t bitsFree = 8 - bitOffset;
    if (bitsFree < bit_count) {
        data |= static_cast<uint8_t>(buffer[byteOffset + 1]) << bitsFree;
    }

    data &= ( ~(0x00ff << bit_count));
    bitHead += bit_count;
}

void InBitStream::readBits(void* data, size_t bit_count) {
    uint8_t* dstByte = reinterpret_cast<uint8_t*>(data);

    while(bit_count >= 8) {
        readBits(*dstByte, 8);
        ++dstByte;
        bit_count -= 8;
    }
    if (bit_count > 0) {
        readBits(*dstByte, bit_count);
    }
}
