#include "Utility.hpp"

#ifndef SORTING_H
#define SORTING_H

template<typename Type>
concept IsPointer = std::is_pointer_v<Type>;

class Sorting final {
public:
	template<IsPointer Pointer>
	constexpr static inline void ExchangeSort(Pointer start, Pointer end) noexcept {
		if (!ValidateRange(start, end))
			return;

		for (Pointer Cursor = start; Cursor < end; Cursor++) {
			for (Pointer AfterCursor = Cursor + 1; AfterCursor < end; AfterCursor++) {
				if (*Cursor < *AfterCursor)
					Swap(Cursor, AfterCursor);
			}
		}
	}

	template<IsPointer Pointer>
	constexpr static inline void SelectionSort(Pointer start, Pointer end) noexcept {
		if (!ValidateRange(start, end))
			return;

		for (Pointer Cursor = start; Cursor < end; Cursor++) {
			Pointer LowestValueIterator = Cursor;
			for (Pointer AfterCursor = Cursor + 1; AfterCursor < end; AfterCursor++) {
				if (*AfterCursor < *LowestValueIterator)
					LowestValueIterator = AfterCursor;
			}
			if (*Cursor != *LowestValueIterator)
				Swap(Cursor, LowestValueIterator);
		}
	}

	template<IsPointer Pointer>
	constexpr static inline void BubbleSort(Pointer start, Pointer end) noexcept {
		if (!ValidateRange(start, end))
			return;

		for (Pointer Cursor = start; Cursor < end; Cursor++) {
			for (Pointer AfterCursor = start; AfterCursor < end - 1; AfterCursor++) {
				if (*AfterCursor > *(AfterCursor + 1))
					Swap(AfterCursor, AfterCursor + 1);
			}
		}
	}

	template<IsPointer Pointer>
	constexpr static inline void InsertionSort(Pointer start, Pointer end) noexcept {
		if (!ValidateRange(start, end))
			return;

		for (Pointer Cursor = start + 1; Cursor < end; Cursor++) {
			auto Key = *Cursor;
			Pointer BeforeCursor = Cursor - 1;

			while (Key < *BeforeCursor && BeforeCursor >= start) {
				Swap(BeforeCursor + 1, BeforeCursor);
				BeforeCursor--;
			}

			Swap(BeforeCursor + 1, &Key);
		}
	}

	constexpr static inline void CountingSort(int* start, int* end) noexcept {
		if (!ValidateRange(start, end))
			return;

		int* CurrentElement = start;
		size_t HighestElementValue = *start;
		while (CurrentElement != end) {
			if (*CurrentElement > HighestElementValue)
				HighestElementValue = *CurrentElement;
			CurrentElement++;
		}

		int32 DefaultCount = 0;
		std::vector<int> Count; //Look into this!
		std::vector<int> ElementsCopy(start, end);
		Count.assign(++HighestElementValue, DefaultCount);

		for (int* i = start; i < end; i++)
			Count[*i]++;

		for (uint32 i = 1; i < Count.size(); i++)
			*(Count.begin() + i) += *(Count.begin() + i - 1);

		for (int* i = ElementsCopy.end()._Ptr - 1; i > ElementsCopy.begin()._Ptr - 1; i--) {
			Count[*i]--;
			*(start + (Count[*i])) = *i;
		}
	}

	template<IsPointer Pointer>
	constexpr static inline void QuickSort(Pointer start, Pointer end) noexcept {
		if (!ValidateRange(start, end))
			return;

		RecursiveQuickSort(start, end - 1);
	}

	template<IsPointer Pointer>
	constexpr static inline void MergeSort(Pointer start, Pointer end) noexcept {
		if (!ValidateRange(start, end))
			return;

		int64 RangeSize = end - start;
		int64 SplitPoint = RangeSize / 2;

		Pointer FirstHalfStart = start;
		Pointer FirstHalfEnd = start + (SplitPoint - 1);

		Pointer SecondHalfStart = start + SplitPoint;
		Pointer SecondHalfEnd = end - 1;

		std::pair<Pointer, Pointer> Range1 = MergePartition(FirstHalfStart, FirstHalfEnd);
		std::pair<Pointer, Pointer> Range2 = MergePartition(SecondHalfStart, SecondHalfEnd);

		InsertionSort(Range1.first, Range2.second + 1);
	}

	template<IsPointer Pointer>
	constexpr static inline void HeapSort(Pointer start, Pointer end) noexcept {
		if (!ValidateRange(start, end))
			return;

		uint32 RangeSize = static_cast<uint32>(end - start);

		for (int32 i = RangeSize / 2 - 1; i >= 0; i--)
			Heapify(start, RangeSize, i);

		for (int32 i = RangeSize - 1; i >= 0; i--) {
			Swap(start, start + i);
			Heapify(start, i, 0);
		}
	}


private: //Helpers
	template<IsPointer Pointer>
	constexpr static inline bool ValidateRange(Pointer start, Pointer end) noexcept {
		if (start == end)
			return false;

		if (start > end)
			return false;

		if (std::distance(start, end) < 2)
			return false;

		return true;
	}

	template<IsPointer Pointer>
	constexpr static inline void Swap(Pointer first, Pointer second) noexcept {
		if (first == second)
			return;

		auto Temp = *first;
		*first = *second;
		*second = Temp;
	}


private: //Sorting Helpers
	template<IsPointer Pointer>
	constexpr static inline Pointer QuickSortPartition(Pointer start, Pointer end) noexcept {

		size_t TotalElements = std::distance(start, end + 1);
		auto Pivot = *(start + (TotalElements - 1) / 2);

		Pointer LeftCursor = start - 1;
		Pointer RightCursor = end + 1;

		while (true) {
			do {
				LeftCursor++;
			} while (*LeftCursor < Pivot);

			do {
				RightCursor--;
			} while (*RightCursor > Pivot);

			if (LeftCursor >= RightCursor)
				return RightCursor;

			Swap(LeftCursor, RightCursor);
		}
	}

	template<IsPointer Pointer>
	constexpr static inline void RecursiveQuickSort(Pointer firstElement, Pointer lastElement) noexcept {
		if (firstElement < lastElement) {
			Pointer Pivot = QuickSortPartition(firstElement, lastElement);
			RecursiveQuickSort(firstElement, Pivot);
			RecursiveQuickSort(Pivot + 1, lastElement);
		}
	}

	template<IsPointer Pointer>
	static std::pair<Pointer, Pointer> MergePartition(Pointer start, Pointer end) noexcept {

		int64 RangeSize = (end - start) + 1;
		if (RangeSize == 1)
			return std::pair<Pointer, Pointer>(start, start);
		else if (RangeSize == 2) {
			if (*start > *end)
				Swap(start, end);
			return std::pair<Pointer, Pointer>(start, end);
		}
		else {
			int64 SplitPoint = RangeSize / 2;

			Pointer FirstHalfStart = start;
			Pointer FirstHalfEnd = start + (SplitPoint - 1);

			Pointer SecondHalfStart = start + SplitPoint;
			Pointer SecondHalfEnd = end;

			std::pair<Pointer, Pointer> Range1 = MergePartition(FirstHalfStart, FirstHalfEnd);
			std::pair<Pointer, Pointer> Range2 = MergePartition(SecondHalfStart, SecondHalfEnd);

			InsertionSort(Range1.first, Range2.second + 1);
			return std::pair<Pointer, Pointer>(start, end);
		}
	}

	template<IsPointer Pointer>
	constexpr static inline void Heapify(Pointer start, size_t count, uint32 index) noexcept {
		uint32 Largest = index;
		uint32 Left = 2 * index + 1;
		uint32 Right = 2 * index + 2;

		if (Left < count && *(start + Left) > *(start + Largest))
			Largest = Left;

		if (Right < count && *(start + Right) > *(start + Largest))
			Largest = Right;

		if (Largest != index) {
			Swap(start + index, start + Largest);
			Heapify(start, count, Largest);
		}
	}
};
#endif // !SORTING_H

