#pragma once

#include <memory>
#include <cstring>
#include <vector>
#include <cassert>
#include <cmath>

const size_t MAX_CAPACITY = 1024;

class InStream {
public:
    explicit InStream(size_t max_capacity = MAX_CAPACITY) :
        maxCapacity(max_capacity)
    {
        InStream(new char[max_capacity], max_capacity);
    }

    InStream(void* buffer, size_t bit_count) :
        buffer(reinterpret_cast<char*>(buffer)),
        capacity(bit_count),
        head(0)
    {}

    ~InStream() { std::free(buffer); }

    bool empty() const { return getRemainDataSize() <= 0; }
    void* getBuffer() { return buffer; }
    size_t getRemainDataSize() const { return capacity - head; }
    void serialize(void* out, size_t byte_count);
    void serialize(uint32_t& out) { serialize(&out, sizeof(out)); }
    void serialize(int32_t& out) { serialize(&out, sizeof(out)); }

    void reset() { head = 0; capacity = maxCapacity; }

    template<class T>
    void serialize(T& data);

    template<class T>
    void serialize(std::vector<T>& vector);

private:
    char*   buffer;
    size_t  head;
    size_t  capacity;

    const int maxCapacity = MAX_CAPACITY;
};

class InBitStream {
public:
    InBitStream(size_t max_capacity = MAX_CAPACITY * 8) :
        maxCapacity(max_capacity) {
        InStream(new char[max_capacity], max_capacity);
    }

    InBitStream(void* buffer, size_t bit_count) :
        buffer(reinterpret_cast<char*>(buffer)),
        bitCapacity(bit_count),
        bitHead(0)
    {}

    bool empty() const { return bitHead >= bitCapacity; }

    void* getBuffer() { return buffer; }

    size_t bit_size() const { return bitHead; }
    size_t byte_size() const { return std::ceil((bitHead + 7) / 8.); }
    size_t      size() const { return byte_size(); }

    void readBits(uint8_t& data, size_t bit_count);
    void readBits(void* data, size_t bit_count);

    void serialize(void* out, size_t byte_count) {
        readBits(out, byte_count << 3);
    }

    template<class T>
    void serializeArray(T data[], size_t byte_count) {
        readBits(data, byte_count << 3);
    }

    template<class T>
    void serialize(T& data, size_t bit_count = sizeof(T) * 8) {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value || !std::is_array<T>::value,
            "Template Write only supports primirive data types");
        readBits(&data, bit_count);
    }

    template<class T>
    void serialize(std::vector<T>& vector) {
        size_t size = vector.size();
        serialize(size);
        vector.resize(size);
        for (const T& element : vector) {
            serialize(element);
        }
    }

private:
    char*   buffer = nullptr;
    size_t  bitHead = 0;
    size_t  bitCapacity = 0;

    const size_t maxCapacity = MAX_CAPACITY;
};

using InStreamType = InBitStream;