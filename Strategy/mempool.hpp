#ifndef MEMPOOL_HPP
#define MEMPOOL_HPP

 // single thread fix-sized memory pool
template <class T>
class MemPool {
    struct Chunk {
        Chunk* next;
    };
    enum {
        EXPAND_SIZE = 30
    };
    static constexpr int size = sizeof(T) < sizeof(Chunk) ? sizeof(Chunk) : sizeof(T);

public:
    static inline Chunk* get() noexcept {
        if (!head) {
            expand();
        }

        Chunk* old = head;
        head = head->next;

        return old;
    }
    static inline void push(void* memory) noexcept { 
        Chunk* newChunk = reinterpret_cast<Chunk*>(memory);
        newChunk->next = head;
        head = newChunk;
    }

private:
    static Chunk* head;
    static void expand() noexcept {
        unsigned char* allocated = new unsigned char[size * EXPAND_SIZE];
        head = reinterpret_cast<Chunk*>(allocated);

        Chunk* ptr = head;
        for (int i = 0; i < EXPAND_SIZE - 1; ++i) {
            allocated += size;
            ptr->next = reinterpret_cast<Chunk*>(allocated);
            ptr = ptr->next;
        }

        ptr->next = nullptr;
    }
};

template<class T> 
typename MemPool<T>::Chunk* MemPool<T>::head = nullptr;

#endif
