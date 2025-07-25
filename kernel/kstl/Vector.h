#ifndef M__VECTOR_H__
#define M__VECTOR_H__
#include "Error.h"
#include "UniquePtr.h"

namespace ministl {
    
    template <typename T>
    class vector {
        unique_ptr<T[]> storage;
        size_t _size;
        size_t _capacity;

        void grow() {
            size_t new_capacity = _capacity << 1;
            unique_ptr<T[]> new_storage(new T[new_capacity]);

            for (size_t i = 0; i < _size; ++i) {
                new_storage[i] = ministl::move(storage[i]);
            }

            storage = ministl::move(new_storage);
            _capacity = new_capacity;
        }

    public:
        vector() : _size(0), _capacity(8) {}

        void push_back(const T& value) {
            if (_size >= _capacity) {
                grow();
            }
            storage[_size++] = value;
        }

        void push_back(T&& value) {
            if (_size >= _capacity) {
                grow();
            }
            storage[_size++] = ministl::move(value);
        }

        void pop_back() {
            assert(_size > 0);
            storage[_size--].~T();
        }

        T& operator[](size_t index) {
            // No bounds checking for minimal implementation
            return storage[index];
        }

        const T& operator[](size_t index) const {
            return storage[index];
        }

        size_t size() const { return _size; }
        size_t capacity() const { return _capacity; }
        bool empty() const { return _size == 0; }

        void clear() { _size = 0; }

        // optional reserve
        void reserve(size_t new_capacity) {
            if (new_capacity <= _capacity) return;

            // Allocate new array with default-constructed elements
            ministl::unique_ptr<T[]> new_storage(new T[new_capacity]);
            T* old_data = storage.get();
            T* new_data = new_storage.get();

            // Move-construct over first _size slots
            for (size_t i = 0; i < _size; ++i) {
                // Destroy the default-constructed new element
                new_data[i].~T();
                // Placement-new move-construct from old
                new (new_data + i) T(ministl::move(old_data[i]));
                // Destroy old element
                old_data[i].~T();
            }

            // Replace storage
            storage = ministl::move(new_storage);
            _capacity = new_capacity;
        }

        void resize(size_t newSize) {
            // Access raw pointer from unique_ptr
            T* data = storage.get();

            if (newSize < _size) {
                // Destroy elements beyond newSize
                for (size_t i = newSize; i < _size; ++i) {
                    data[i].~T();
                }
            } else if (newSize > _size) {
                // Ensure capacity (reserve uses move construction, no memcpy)
                if (newSize > _capacity) {
                    reserve(newSize);
                    data = storage.get(); // raw pointer might change
                }
                // Default-construct new elements in-place
                for (size_t i = _size; i < newSize; ++i) {
                    new (data + i) T();
                }
            }
            _size = newSize;
        }

        vector(const vector& other)
            : storage(new T[other._capacity]), _size(other._size), _capacity(other._capacity) {
            for (size_t i = 0; i < _size; ++i) {
                storage[i] = other.storage[i];
            }
        }

        vector& operator=(const vector& other) {
            if (this != &other) {
                unique_ptr<T[]> new_storage(new T[other._capacity]);
                for (size_t i = 0; i < other._size; ++i) {
                    new_storage[i] = other.storage[i];
                }
                storage = ministl::move(new_storage);
                _size = other._size;
                _capacity = other._capacity;
            }
            return *this;
        }

        vector(vector&& other)
            : storage(ministl::move(other.storage)), _size(other._size), _capacity(other._capacity) {
            other._size = 0;
            other._capacity = 0;
        }

        vector& operator=(vector&& other) {
            if (this != &other) {
                storage = ministl::move(other.storage);
                _size = other._size;
                _capacity = other._capacity;
                other._size = 0;
                other._capacity = 0;
            }
            return *this;
        }

        inline T* data() {
            return storage.get();
        }

        inline const T* data() const {
            return storage.get();
        }
    };

} 




#endif