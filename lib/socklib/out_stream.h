#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <cmath>

class OutStream {
public:
    OutStream() { reallocBuffer(32); }
    ~OutStream() {
        std::free(buffer);
    }

    const void* getBuffer() const { return reinterpret_cast<const void*>(buffer); }
    size_t      size() const { return head; }

    void serialize(const void* data, size_t byte_count);

    template<class T>
    void serialize(const T& data) {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
        "write only supports primitive data types");

        serialize(&data, sizeof(data));
    }

    template<class T>
    void serialize(const T& data, int byte_count) {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
        "write only supports primitive data types");

        serialize(&data, byte_count);
    }

    template<class T>
    void serialize(const std::vector<T>& vector) {
        size_t size = vector.size();
        serialize(size);
        for (const T& element : vector) {
            serialize(element);
        }
    }

private:
    char*   buffer = nullptr;
    size_t  head = 0;
    size_t  capacity = 0;

    void reallocBuffer(size_t new_length);
};

class OutBitStream {
public:
    OutBitStream() { reallocBuffer(256); }
    ~OutBitStream() { std::free(buffer); }

    void writeBits(uint8_t data, size_t bit_count);
    void writeBits(const void* data, size_t bit_count);

    const char* getBuffer() const { return buffer; }
    size_t bit_size() const { return bitHead; }
    size_t byte_size() const {return std::ceil((bitHead + 7) / 8.); }
    size_t      size() const { return byte_size(); }

    void serialize(const void* data, size_t byte_count) {
        writeBits(data, byte_count << 3);
    }

    void serialize(bool data) {
        writeBits(data, 1);
    }

    template<class T>
    void serializeArray(T data[], size_t byte_count) {
        writeBits(data, byte_count << 3);
    }

    template<class T>
    void serialize(const T& data, size_t bit_count = sizeof(T) * 8) {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value || !std::is_array<T>::value,
            "Template Write only supports primirive data types");
        writeBits(&data, bit_count);
    }

    template<class T>
    void serialize(const std::vector<T>& vector) {
        size_t size = vector.size();
        serialize(size);
        for (const T& element : vector) {
            serialize(element);
        }
    }

private:
    char*   buffer = nullptr;
    size_t  bitHead = 0;
    size_t  capacity = 0;

    void reallocBuffer(size_t new_length);
};

using OutStreamType = OutBitStream;
