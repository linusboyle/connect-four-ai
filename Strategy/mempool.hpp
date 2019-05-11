#ifndef MEMPOOL_HPP
#define MEMPOOL_HPP

// single thread fix-sized memory pool
template <class T>
class MemPool {

    struct Chunk {
        Chunk* next;
    };

	static constexpr int EXPAND_SIZE = 30;
    static constexpr int size = sizeof(T) < sizeof(Chunk) ? sizeof(Chunk) : sizeof(T);

    int count;

	MemPool() : head(nullptr), count(0) {}

	MemPool(const MemPool&) = delete;
	MemPool(MemPool&&) = delete;
	void operator=(const MemPool&) = delete;
	void operator=(MemPool&&) = delete;

	~MemPool() {
		Chunk* cur = head;
		while (cur) {
			Chunk* next = cur->next;
			unsigned char* freed = reinterpret_cast<unsigned char*>(cur);
			delete[]freed;
			cur = next;
		}
	}
public:
	static MemPool& instance() {
		static MemPool ins;
		return ins;
	}
	inline int cnt() noexcept {
		return count;
	}
    inline Chunk* get() noexcept {
        if (!head) {
            expand();
        }

        Chunk* old = head;
        head = head->next;

		count--;

        return old;
    }
    inline void push(void* memory) noexcept { 
        Chunk* newChunk = reinterpret_cast<Chunk*>(memory);
        newChunk->next = head;
        head = newChunk;
		count++;
    }

private:
    Chunk* head;
//    void expand() noexcept {
//        unsigned char* allocated = new unsigned char[size * EXPAND_SIZE];
//        head = reinterpret_cast<Chunk*>(allocated);
//
//        Chunk* ptr = head;
//        for (int i = 0; i < EXPAND_SIZE - 1; ++i) {
//            allocated += size;
//            ptr->next = reinterpret_cast<Chunk*>(allocated);
//            ptr = ptr->next;
//        }
//
//        ptr->next = nullptr;
//		count += EXPAND_SIZE;
//    }

	// this version is much slower
	void expand() noexcept {
		for (int i = 0; i < EXPAND_SIZE; ++i) {
			unsigned char* allocated = new unsigned char[size];
			Chunk* ptr = reinterpret_cast<Chunk*>(allocated);
			ptr->next = head;
			head = ptr;
		}
		count += EXPAND_SIZE;
	}
};

#endif
