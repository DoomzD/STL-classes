#include <cstddef>
#include <algorithm>

template<typename T>
class SharedPtr {
public:
    SharedPtr() noexcept = default;

    explicit SharedPtr(T* ptr)
        : Ptr(ptr), Cnt(new size_t(1)) {}

    SharedPtr(const SharedPtr& other) noexcept
        : Ptr(other.Ptr), Cnt(other.Cnt) {
        if (Cnt)
            ++*Cnt;
    }

    SharedPtr(SharedPtr&& other) noexcept {
        Ptr = other.Ptr;
        Cnt = other.Cnt;
        other.Ptr = nullptr;
        other.Cnt = nullptr;
    }

    SharedPtr& operator = (T* ptr) {
        SharedPtr<T>(ptr).swap(*this);
        return *this;
    }

    SharedPtr& operator = (const SharedPtr& other) noexcept {
            SharedPtr<T>(other).swap(*this);
            return *this;
    }

    SharedPtr& operator = (SharedPtr&& other) noexcept {
            SharedPtr<T>(std::move(other)).swap(*this);
            return *this;
    }

    T& operator * () noexcept {
        return *get();
    }

    const T& operator * () const noexcept {
        return *get();
    }

    T* operator ->() const noexcept {
        return Ptr;
    }

    void reset(T* ptr) {
        SharedPtr<T>(ptr).swap(*this);
    }

    void swap(SharedPtr& other) noexcept {
        std::swap(Cnt, other.Cnt);
        std::swap(Ptr, other.Ptr);
    }

    T* get() const noexcept {
        return Ptr;
    }

    explicit operator bool() const noexcept {
        return Ptr != nullptr;
    }

    ~SharedPtr() {
        decrease();
    }

private:
    void decrease() {
        if (Cnt) {
            --*Cnt;
            if (*Cnt == 0) {
                delete Ptr;
                delete Cnt;
            }
        }
    }

    size_t* Cnt = nullptr;
    T* Ptr = nullptr;
};
