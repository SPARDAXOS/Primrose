#include "Container.h"
#include "Utility.hpp"


#ifndef MEMORY_BLOCKS_BUCKET
#define MEMORY_BLOCKS_BUCKET


//Specifications:
//Contains a list of pointers with each pointing towards a memory block of fixed size
//One list contains the pointers to memory blocks
//One list contains bools lists to map out each memory block placements
//Adding an element will put it in the first found available spot in the first non full memory block
//If no available spot is found in all memory blocks, a new memory block and mapping are created.
//NO REALLOCATIONS! The main purpose should be to keep data as contigous as possible and avoid invalidating iterators on any changes!
//Removing an element does not deallocate memory but destroys the objects.
//If a memory block is empty then it is deallocated to save memory
//If elements of 2 different memory blocks can be gathered into 1 memory block then it should be done to save up on memory.
//Some book keeping for elements amount in each memory block is requiered to query the merge operation conditions quickly.

template<class T>
class MemoryBlockSpecification final {
public:
	using Type = T;
	using Reference = T&;
	using ConstantReference = const T&;
	using Pointer = T*;
	using ConstantPointer = const T*;
	using SizeType = std::size_t;
	using AllocatorTraits = std::allocator_traits<CustomAllocator<Type>>;

public:
	explicit MemoryBlockSpecification() = delete;
	MemoryBlockSpecification(CustomAllocator<Type>& allocator, SizeType size)
		:	m_Allocator(&allocator), m_Capacity(size)
	{
		m_Block = allocate();
		m_Mappings.assign(m_Capacity, false);
	}
	//Its not allocator aware
	//Add Special member functions!



	//TODO: Add some custom ctors
	~MemoryBlockSpecification() {
		deallocate();
	}


	constexpr inline Reference operator[](SizeType index) noexcept { return m_Block[index]; }
	constexpr inline ConstantReference operator[](SizeType index) const noexcept { return m_Block[index]; }

public:
	template<class... args>
	constexpr inline Pointer add(args&&... arguments) {
		if (!m_Block || is_full())
			return nullptr;

		//Findspot
		//Construct at spot
		//Update mapping

		auto placement = get_available_address();
		if (!placement.first)
			return nullptr;


		m_Size++;
		m_Mappings[placement.second] = true;
		return &construct(placement.first, std::forward<args>(arguments)...);
	}

public:
	constexpr inline Pointer begin() noexcept { return m_Block; }
	constexpr inline ConstantPointer begin() const noexcept { return m_Block; }

	constexpr inline Pointer end() noexcept { return m_Block + m_Size; }
	constexpr inline ConstantPointer end() const noexcept { return m_Block + m_Size; }

public:
	constexpr inline bool is_full() const noexcept {
		return m_Size == m_Capacity;
	}
	constexpr inline std::pair<Pointer, int32> get_available_address() const noexcept {
		if (is_full())
			return { nullptr, -1 };

		for (uint32 i = 0; i < m_Mappings.size(); i++) {
			if (!m_Mappings[i])
				return { m_Block + i, i };
		}

		return { nullptr, -1 };
	}
	constexpr inline Pointer find(uint64 ID) const noexcept {
		if (m_Size == 0)
			return nullptr;

		for (Pointer index = m_Block; index < m_Block + m_Capacity; index++) {
			if (index->GetOwnerID() == ID)
				return index;
		}

		return nullptr;
	}

private:
	constexpr inline Pointer allocate() {
		//if (capacity > max_size())
		//	throw std::length_error("Max allowed container size exceeded!");

		//No guarantee
		auto NewBuffer = AllocatorTraits::allocate(*m_Allocator, sizeof(Type) * m_Capacity);
		if (!NewBuffer)
			throw std::bad_alloc();

		return NewBuffer;
	}
	constexpr inline void deallocate() {
		if (!m_Block)
			return;

		AllocatorTraits::deallocate(*m_Allocator, m_Block, m_Capacity);
		m_Block = nullptr;
	}

private:
	template<class... args>
	constexpr inline Reference construct(Pointer address, args&&... arguments) {


		//Get vacant spot on block - Consider making it a class with helper functions then

		AllocatorTraits::construct(*m_Allocator, address, arguments...);
		return *address;
	}

	constexpr inline void destruct(Pointer target) noexcept {
		if (!target)
			return;

		if (std::destructible<Type>) // Will pass check even if fundemental
			AllocatorTraits::destroy(m_Allocator, target);
	}
	constexpr inline void destruct(ConstantPointer target) noexcept {
		if (!target)
			return;

		if (std::destructible<Type>) // Will pass check even if fundemental
			AllocatorTraits::destroy(m_Allocator, target);
	}
	constexpr inline void destruct(Pointer first, Pointer last) noexcept {
		if (!first || !last)
			return;

		if (first > last)
			return;

		if (first == last)
			destruct(first);

		if (std::destructible<Type>) { // Will pass check even if fundemental
			for (Pointer i = first; i < last; i++)
				AllocatorTraits::destroy(m_Allocator, i);
		}

		//m_Size -= last - first;
	}

public:
	Pointer m_Block = nullptr;
	SizeType m_Capacity = 0;
	SizeType m_Size = 0;
	std::vector<bool> m_Mappings;
	CustomAllocator<Type>* m_Allocator;
};



template<
	class T,
	class Alloc = CustomAllocator<T>
>
class MemoryBlocksBucket final {
public:
	using Type = T;
	using Reference = T&;
	using ConstantReference = const T&;
	using Pointer = T*;
	using ConstantPointer = const T*;
	using SizeType = std::size_t;
	using Allocator = Alloc;
	using AllocatorTraits = std::allocator_traits<CustomAllocator<Type>>;

	static_assert(std::is_object_v<T>, "The C++ Standard forbids containers of non-object types "
		"because of [container.requirements].");

public:
	MemoryBlocksBucket() = delete;
	MemoryBlocksBucket(SizeType blockSize, const Allocator allocator = Allocator())
		:	m_Allocator(allocator), m_BlockSize(blockSize)
	{
		//static_assert(blockSize > 0, "blockSize must be above 0!");
	}
	//Special member functions!


public: //Access
	constexpr inline Reference at(SizeType index) {
		if (index >= m_Blocks.size())
			throw std::out_of_range("Access Violation - " + index);

		return m_Blocks[index];
	}
	constexpr inline ConstantReference at(SizeType index) const {
		if (index >= m_Blocks.size())
			throw std::out_of_range("Access Violation - " + index);

		return m_Blocks[index];
	}

	constexpr inline Reference operator[](SizeType index) noexcept { 

		uint32 BlockIndex = floor(index / m_BlockSize);
		uint32 ElementIndex = index % m_BlockSize;
		return m_Blocks[m_BlockSize][ElementIndex];
	}
	constexpr inline ConstantReference operator[](SizeType index) const noexcept { 
		
		uint32 BlockIndex = static_cast<unsigned int>(floor(index / m_BlockSize));
		uint32 ElementIndex = static_cast<unsigned int>(index % m_BlockSize);
		return m_Blocks[BlockIndex][ElementIndex];
	}

	constexpr inline Pointer find(uint64 ID) const noexcept {
		Pointer Result = nullptr;
		for(auto& block : m_Blocks) {
			Result = block.find(ID);
			if (Result)
				return Result;
		}

		return nullptr;
	}

public: //Iterators - Reconsider
	constexpr inline Pointer begin() noexcept { return m_Blocks.begin()->m_Block; }
	constexpr inline ConstantPointer begin() const noexcept { return m_Blocks.begin()->m_Block; }

	constexpr inline Pointer end() noexcept { return m_Blocks.end()->m_Block; } //sus
	constexpr inline ConstantPointer end() const noexcept { return m_Blocks.end()->m_Block; } //sus



public: //Insertion
	//GetAll
	template<class... args>
	constexpr inline Pointer emplace_back(args&&... arguments) {

		for (auto& block : m_Blocks) {
			if (!block.is_full())
				return block.add(std::forward<args>(arguments)...);
		}

		return add_block().add(std::forward<args>(arguments)...);
	}

	constexpr inline bool erase(uint64 id) { //Maybe get an ID instead. Would limit this data structure to components only
		//Iterate through all memory blocks looking for an equal element. Might be a problem since the gameobject doesnt have the comp but rather just the ID

		return true;
	}

public:
	constexpr inline SizeType capacity() const noexcept {
		return m_BlockSize * m_Blocks.size();
	}
	constexpr inline SizeType blocks_count() const noexcept {
		return m_Blocks.size();
	}
	constexpr inline SizeType blocks_size() const noexcept {
		return m_BlockSize;
	}
	constexpr inline SizeType size() const noexcept {
		std::size_t Results = 0;
		for (auto& block : m_Blocks)
			Results += block.m_Size;
		return Results;
	}
	constexpr inline SizeType max_size() const noexcept { return static_cast<SizeType>(pow(2, sizeof(Pointer) * 8) / sizeof(Type) - 1); }



private:


private: //Memory
	constexpr inline MemoryBlockSpecification<Type>& add_block() {

		//MemoryBlockSpecification<Type> NewMemoryBlock(m_Allocator, m_BlockSize);
		return m_Blocks.emplace_back(MemoryBlockSpecification<Type>(m_Allocator, m_BlockSize));
	}
	constexpr inline Pointer allocate_block() {
		//if (capacity > max_size())
		//	throw std::length_error("Max allowed container size exceeded!");

		//No guarantee
		//Not needed here
		Pointer NewBuffer = AllocatorTraits::allocate(m_Allocator, sizeof(Type) * m_BlockSize);
		if (!NewBuffer)
			throw std::bad_alloc();


		return NewBuffer;
	}
	constexpr inline void deallocate_block() {
		//if (!m_Iterator)
		//	return;

		//Not needed here
		//AllocatorTraits::deallocate<Type>(m_Allocator, m_Block, m_Capacity);

		//m_Allocator.deallocate<Type>(m_Iterator, size);
		//m_Iterator = nullptr;
	}


	


	constexpr inline MemoryBlockSpecification<Type> merge(MemoryBlockSpecification<Type>& a, MemoryBlockSpecification<Type>& b) noexcept {
		//Check if they can be merged even
		//Returns the resulting merged block
	}

private:
	Marigold::Container<MemoryBlockSpecification<Type>> m_Blocks;
	CustomAllocator<Type> m_Allocator;
	SizeType m_BlockSize = 0;
};
#endif // !MEMORY_BLOCKS_BUCKET
