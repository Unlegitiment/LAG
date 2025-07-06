#pragma once
#include "Allocator.h"
namespace lit {
	template<typename T, template<typename, typename> typename _allocator = Allocator> // u32 const is pretty nice lmao. 
	class List {
	public:
		template<typename T> class ListIteration { // funzies.
		public:
			ListIteration(T* data) : m_pData(data){}
			ListIteration operator++() {
				m_pData + 1;
				return *this; // check lmao?
			}
		private:
			T* m_pData;
		};
		using Type = T;
		using ListAllocator = _allocator<Type, u32>;
		using SizeType = typename ListAllocator::SizeType; //  I think this is how it would work lmao? 
		using ListIterator = ListIteration<Type>;
		using ConstantListIterator = ListIteration<const Type>;
		List() {
			m_Capacity = 1;
			m_Size = 0;
			m_pArray = m_Allocator.Allocate(m_Capacity); // create a default of 1.
		}
		void Add(const Type& data) {
			if (DoINeedToGrow()) {
				Grow();
			}
			m_Allocator.Construct(&m_pArray[m_Size++], data); // fuhhh. 
		}
	private:
		void Grow() {
			m_Capacity *= 2;
			Type* newArr = m_Allocator.Allocate(m_Capacity);
			for (SizeType i = 0; i <= m_Size; ++i) { 
				newArr[i] = m_pArray[i];
				m_Allocator.Deconstruct(m_pArray[i]); // im not sure if this should be done or now since we copy the data. This might break pointers. 
			}
			m_Allocator.Deallocate(m_pArray);
			m_pArray = newArr;
		}
		bool DoINeedToGrow() {
			SizeType newSize = this->m_Size + 1;
			if (newSize >= m_Capacity) {
				return true;
			}
			return false;
		}
		ListAllocator m_Allocator; // persistent. Doesn't need a new bc not lifetime dependant?
		Type* m_pArray = nullptr;
		SizeType m_Size;
		SizeType m_Capacity; 
	};
}