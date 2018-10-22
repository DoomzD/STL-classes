#include <cstddef>
#include <algorithm>
#include <type_traits>

template<typename T>
class VectorBase {
public:
    T* Data;
    size_t Capacity;

    VectorBase()
            : Data(nullptr)
            , Capacity(0) {}

    VectorBase(size_t n) {
        Data = static_cast<T*>(::operator new(n * sizeof(T)));
        Capacity = n;
    }

    T& operator[] (size_t i) const {
        return Data[i];
    }

    void reallocate(size_t newCapacity) {
        T* newData = nullptr;
        newData = static_cast<T*>(::operator new(newCapacity * sizeof(T)));

        uninitialized_copy(Data, Data + Capacity, newData);
        Destroy();
        Data = newData;
        Capacity = newCapacity;
    }

    ~VectorBase() {
        ::operator delete(Data);
    }

private:
    template<typename InputIter, typename OutputIter>
    void uninitialized_copy(InputIter begin, InputIter end, OutputIter out) {
        using U = typename std::iterator_traits<InputIter>::value_type;
        OutputIter cur = out;
        try {
            while (begin != end) {
                new (static_cast<void*>(&*cur++)) U(*begin++);
            }
        } catch (...) {
            while (out != cur) {
                out->~U();
            }

            throw;
        }
    }

    void Destroy() {
        T* begin = Data;
        T* end = Data + Capacity;

        while (begin != end) {
            begin->~T();
            ++begin;
        }

        ::operator delete(Data);
    }
};

template<typename T>
class Vector {
public:
    Vector() : Base(), Size(0) {}

    Vector(size_t size)
            : Base(size) {
        size_t i = 0;
        try {
            for (; i != size; ++i) {
                new (Base.Data + i) T();
            }
        } catch (...) {
            for (size_t j = 0; j != i; ++j) {
                Base[j].~T();
                throw;
            }
        }

        Size = size;
    }

    Vector(const Vector& other) {
        (*this) = other;
    }

    Vector& operator = (const Vector& other) {
        Size = other.Size;

        Base = VectorBase<T>{other.Size};
        size_t i = 0;
        try {
            for (; i != Size; ++i) {
                new (Base.Data + i) T(other[i]);
            }
        } catch (...) {
            for (size_t j = 0; j != i; ++j) {
                Base[j].~T();
                throw;
            }
        }

        return *this;
    }

    void reserve(size_t size) {
        if (Base.Capacity >= size)
            return;

        Base.reallocate(size);
    }

    void resize(size_t size) {
        if (size < Size) {
            for (size_t i = size; i < Size; ++i) {
                Base[i].~T();
            }
        } else if (size > Size) {
            if (size > Base.Capacity) {
                VectorBase<T> newBase(size);
                for (size_t i = 0; i < Size; ++i)
                    new (newBase.Data + i) T(Base[i]);
                for (size_t i = Size; i < size; ++i)
                    new (newBase.Data + i) T();

                for (size_t i = 0; i < Size; ++i)
                    Base[i].~T();

                std::swap(Base.Data, newBase.Data);
                std::swap(Base.Capacity, newBase.Capacity);
            } else {
                size_t i = Size;
                try {
                    for (; i < size; ++i) {
                        new (Base.Data + i) T();
                    }
                } catch (...) {
                    for (size_t j = Size; j < i; ++j) {
                        Base[j].~T();
                    }
                    throw;
                }
            }
        }

        Size = size;
    }

    void clear() {
        for (size_t i = 0; i < Size; ++i) {
            Base[i].~T();
        }

        Size = 0;
    }

    void swap(Vector<T>& other) {
        std::swap(Base, other.Base);
        std::swap(Size, other.Size);
    }

    T* begin() {
        return Base.Data;
    }

    T* end() {
        return Base.Data + Size;
    }

    void push_back(const T& elem) {
        if (Size == Base.Capacity)
            Base.reallocate(Base.Capacity ? Base.Capacity * 2 : 1);

        new (Base.Data + Size) T(elem);
        ++Size;
    }

    void push_back(T&& elem) {
        if (Size == Base.Capacity)
            Base.reallocate(Base.Capacity ? Base.Capacity * 2 : 1);

        new (Base.Data + Size) T(std::move(elem));
        ++Size;
    }

    void pop_back() {
        Base[Size - 1].~T();
        --Size;
    }

    size_t size() const {
        return Size;
    }

    T& operator[] (int i) const {
        return Base[i];
    }

    size_t capacity() const {
        return Base.Capacity;
    }

    ~Vector() {
        for (size_t i = 0; i < Size; ++i)
            Base[i].~T();
    }

private:
    VectorBase<T> Base{};
    size_t Size = 0;
};

#ifdef LOCAL_HOME

#include <iostream>
#include <string>
#include <utility>

class C {
public:
    std::string val = "(empty)";

    C(const std::string& s = ""): val(s) {
        std::cout << "constructor C() called: " << val << "\n";
    }
    C(const C& other): val(other.val) {
        std::cout << "constructor C(const C&) called: " << val << "\n";
    }
    C(C&& other): val(std::move(other.val)) {
        other.val = "(moved)";
        std::cout << "constructor C(C&&) called: " << val << "\n";
    }
    C& operator = (const C& other) {
        std::cout << "operator = (C&) called: " << val << " " << other.val << "\n";
        val = other.val;
        return *this;
    }
    C& operator = (C&& other) {
        std::cout << "operator = (C&&) called: " << val << " " << other.val << "\n";
        val = std::move(other.val);
        other.val = "(moved)";
        return *this;
    }
    ~C() {
        std::cout << "destructor called: " << val << "\n";
    }
};

template <typename V>
void print(const V& v) {
    std::cout << "size: " << v.size() << "\n";
    std::cout << "capacity: " << v.capacity() << "\n";
}

int main() {
    static_assert(sizeof(Vector<C>) <= sizeof(C*) * 3, "");

    std::cout << "Empty vector:\n";
    Vector<C> v;
    print(v);

    std::cout << "\npush_back:\n";
    C c("hello");
    v.push_back(c);
    print(v);

    std::cout << "\nreserve:\n";
    v.reserve(5);
    print(v);

    std::cout << "\nresize\n";
    v.resize(3);
    print(v);
    v.resize(2);
    print(v);

    v.resize(7);
    print(v);
    v.resize(4);
    print(v);

    std::cout << "\npop_back\n";
    v.pop_back();
    print(v);
    v.pop_back();
    print(v);

    std::cout << "\npush_back with move semantics\n";
    v.push_back(C("world"));
    print(v);

    std::cout << "\niteration\n";
    for (const auto& item : v)
        std::cout << item.val << "\n";

    std::cout << "\nindexing\n";
    for (size_t i = 0; i != v.size(); ++i)
        std::cout << v[i].val << "\n";
}

#endif
