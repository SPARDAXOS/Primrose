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
	explicit MemoryBlockSpecification() = default;
	//TODO: Add some custom ctors
	~MemoryBlockSpecification() {
		//TODO: Clean up mem or... Maybe it takes a reference to a CustomAllocator that it uses to free up the memory! It also uses in the ctor to allocate the mem!
	}



public:
	constexpr inline bool IsFull() const noexcept {
		return m_Size == m_Capacity;
	}
	constexpr inline T* GetAvailableAddress() const noexcept {
		if (IsFull())
			return nullptr;

		for (uint32 i = 0; i < m_Mappings.size(); i++) {
			if (!m_Mappings[i])
				return m_Block + i;
		}

		return nullptr;
	}


public:
	T* m_Block = nullptr;
	uint32 m_Capacity = 0;
	uint32 m_Size = 0;
	std::vector<bool> m_Mappings;
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
	using Iterator = T*;
	using ConstantIterator = const T*;
	using SizeType = std::size_t;
	using AllocatorTraits = std::allocator_traits<CustomAllocator<Type>>;

	static_assert(std::is_object_v<T>, "The C++ Standard forbids containers of non-object types "
		"because of [container.requirements].");

private:


public:
	MemoryBlocksBucket() = delete;
	MemoryBlocksBucket(SizeType blockSize) 
		:	m_BlockSize(blockSize)
	{
		static_assert(blockSize > 0, "blockSize must be above 0!");
	}
	//Takes the size that should be each block



public:
	//Add
	//Remove
	//[]
	//At
	//FindElement
	//GetAll
	template<class... args>
	constexpr inline Reference Add(args&&... arguments) {

		MemoryBlockSpecification* MemoryBlock = nullptr;
		Iterator Address = nullptr;
		for (auto& block : m_Blocks) {
			if (!block.IsFull()) {
				Address = block.GetAvailableAddress();
				MemoryBlock = &block;
			}
		}
		if (!Address) {
			//Add New Memory Block if possible!
			//Get Address from it!
			//Set it as target memory block
		}

		Reference NewElement = Construct(Address);
		MemoryBlock->m_Size++;
		return NewElement;
	}

	constexpr inline bool Remove(const Type& element) { //Maybe get an ID instead. Would limit this data structure to components only
		//Iterate through all memory blocks looking for an equal element. Might be a problem since the gameobject doesnt have the comp but rather just the ID
	}

public:
	constexpr inline SizeType Capacity() const noexcept {
		return m_BlockSize * m_Blocks.size();
	}
	constexpr inline SizeType MemoryBlocksCount() const noexcept {
		return m_Blocks.size();
	}
	constexpr inline SizeType Size() const noexcept {
		auto Results = 0;
		for (auto& block : m_Blocks)
			Results += block.m_ContainedElements;
		return Results;
	}
	constexpr inline SizeType BlockSize() const noexcept {
		return m_BlockSize;
	}
	constexpr inline SizeType MaxSize() const noexcept { return static_cast<SizeType>(pow(2, sizeof(Iterator) * 8) / sizeof(Type) - 1); }



private:


private: //Memory
	constexpr inline MemoryBlockSpecification<Type>& AddMemoryBlock() {

		MemoryBlockSpecification<Type> NewMemoryBlock;
		NewMemoryBlock.m_Block = AllocateMemoryBlock();
		NewMemoryBlock.m_Capacity = m_BlockSize;
		NewMemoryBlock.m_Size =	0;
		NewMemoryBlock.m_Mappings.assign(m_BlockSize, false);

		m_Blocks.emplace_back(NewMemoryBlock);
	}
	constexpr inline Iterator AllocateMemoryBlock() {
		//if (capacity > max_size())
		//	throw std::length_error("Max allowed container size exceeded!");

		//No guarantee
		Iterator NewBuffer = m_Allocator.allocate(sizeof(Type) * m_BlockSize);
		if (!NewBuffer)
			throw std::bad_alloc();


		return NewBuffer;
	}
	constexpr inline void DeallocateMemoryBlock() {
		//if (!m_Iterator)
		//	return;


		//m_Allocator.deallocate<Type>(m_Iterator, size);
		//m_Iterator = nullptr;
	}

	constexpr inline Reference Construct(Iterator address) {
		

		//Get vacant spot on block - Consider making it a class with helper functions then

		

		//AllocatorTraits::construct(m_Allocator, m_Iterator + IndexPosition, arguments...);


	}
	
	constexpr inline void Destruct(Iterator target) noexcept {
		if (!target)
			return;

		if (std::destructible<Type>) // Will pass check even if fundemental
			AllocatorTraits::destroy(m_Allocator, target);
	}
	constexpr inline void Destruct(ConstantIterator target) noexcept {
		if (!target)
			return;

		if (std::destructible<Type>) // Will pass check even if fundemental
			AllocatorTraits::destroy(m_Allocator, target);
	}
	constexpr inline void Destruct(Iterator first, Iterator last) noexcept {
		if (!first || !last)
			return;

		if (first > last)
			return;

		if (first == last)
			Destruct(first);

		if (std::destructible<Type>) { // Will pass check even if fundemental
			for (Iterator i = first; i < last; i++)
				AllocatorTraits::destroy(m_Allocator, i);
		}

		//m_Size -= last - first;
	}

	constexpr inline MemoryBlockSpecification<Type> Merge(MemoryBlockSpecification<Type>& a, MemoryBlockSpecification<Type>& b) noexcept {
		//Check if they can be merged even
		//Returns the resulting merged block
	}

private:
	Marigold::Container<MemoryBlockSpecification<Type>> m_Blocks;
	CustomAllocator<Type> m_Allocator;
	SizeType m_BlockSize = 0;
};
#endif // !MEMORY_BLOCKS_BUCKET
