#include <cstddef>
#include <algorithm>
#include <memory>

template<class T, class TDeleter = std::default_delete<T>>
class UniquePtr {
public:
    UniquePtr() = default;

    explicit UniquePtr(T* ptr) noexcept {
        std::get<0>(Ptr) = ptr;
    }

    explicit UniquePtr(T* ptr, const TDeleter& deleter) noexcept
            : Ptr(ptr, deleter) {}

    UniquePtr(UniquePtr&& other) noexcept {
        Ptr = std::tuple<T*, TDeleter>{other.release(), other.get_deleter()};
    }

    UniquePtr(const UniquePtr& other) = delete;
    UniquePtr& operator = (const UniquePtr& other) = delete;

    UniquePtr& operator = (std::nullptr_t ptr) noexcept {
        reset(ptr);
        return *this;
    }

    UniquePtr& operator = (UniquePtr&& other) noexcept {
        reset(other.release(), other.get_deleter());
        return *this;
    }

    T& operator * () {
        return *get();
    }

    const T& operator * () const {
        return *get();
    }

    T* operator -> () const noexcept {
        return get();
    }

    T* release() noexcept {
        T* oldPtr = std::get<0>(Ptr);
        std::get<0>(Ptr) = nullptr;
        return oldPtr;
    }

    void reset(T* ptr) noexcept {
        get_deleter()(get());
        std::get<0>(Ptr) = ptr;
    }

    void reset(T* ptr, const TDeleter& deleter) noexcept {
        get_deleter()(get());
        Ptr = std::tuple<T*, TDeleter>{ptr, deleter};
    }

    void swap(UniquePtr& other) noexcept {
        std::swap(Ptr, other.Ptr);
    }

    T* get() const noexcept {
        return std::get<0>(Ptr);
    }

    TDeleter& get_deleter() noexcept {
        return std::get<1>(Ptr);
    }

    const TDeleter& get_deleter() const noexcept {
        return std::get<1>(Ptr);
    }

    explicit operator bool() const noexcept {
        return get() != nullptr;
    }

    ~UniquePtr() {
        get_deleter()(std::get<0>(Ptr));
    }

private:
    std::tuple<T*, TDeleter> Ptr{nullptr, TDeleter{}};
};
