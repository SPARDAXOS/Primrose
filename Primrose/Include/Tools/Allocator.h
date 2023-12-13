
#ifndef CUSTOM_ALLOCATOR
#define CUSTOM_ALLOCATOR

template<class _Alloc>
class CustomAllocator final {
public:
	using value_type = _Alloc;
	using pointer = _Alloc*;
	using size_type = std::size_t;
	using const_pointer = const _Alloc*;
	using const_reference = const _Alloc&;

	using propagate_on_container_move_assignment = std::true_type;
	using propagate_on_container_copy_assignment = std::true_type;
	using propagate_on_container_swap			 = std::true_type;
	//using is_always_equal						 = std::true_type; //C++17 for non-empty allocators that are always equal

public:
	struct AllocatorLog {
		size_type m_Allocations = 0;
		size_type m_Deallocations = 0;
		size_type m_AllocatedMemory = 0;
		size_type m_DeallocatedMemory = 0;
		size_type m_Constructions = 0;
		size_type m_Deconstructions = 0;
	};

public:
	CustomAllocator() noexcept {}
	~CustomAllocator() {}

	template <class U>
	inline CustomAllocator(const CustomAllocator<U>&) noexcept {}

public:
	template<class T, class... args>
	constexpr inline void construct(T* address, args&&... arguments) {
		std::construct_at(address, std::forward<args>(arguments)...);
		m_Log.m_Constructions++;
	}
	template<class T>
	constexpr inline void destroy(T* address) {
		std::destroy_at(address);
		m_Log.m_Deconstructions++;
	}

public:
	//Incosistency between allocate and deallocate with size usage.
	[[nodiscard]] constexpr inline pointer allocate(const size_type size) {
		if (size == 0)
			return nullptr;

		m_Log.m_AllocatedMemory += size;
		m_Log.m_Allocations++;
		return static_cast<pointer>(malloc(size));;
	}
	template<typename T>
	constexpr inline void deallocate(T* address, size_type size) {
		if (!address)
			return;

		m_Log.m_DeallocatedMemory += size * sizeof(T);
		m_Log.m_Deallocations++;
		free(address);
	}

public:
	constexpr inline size_type max_size() const noexcept {
		return std::numeric_limits<size_type>::max() / sizeof(value_type);
	}
	constexpr inline AllocatorLog GetLog() const noexcept {
		return m_Log; 
	}

private:
	AllocatorLog m_Log;
};

namespace {
	template <class T, class U>
	constexpr bool operator==(const CustomAllocator<T>&, const CustomAllocator<U>&) noexcept {
		return true;
	}

	template <class T, class U>
	constexpr bool operator!=(const CustomAllocator<T>&, const CustomAllocator<U>&) noexcept {
		return false;
	}
}
#endif // !CUSTOM_ALLOCATOR


