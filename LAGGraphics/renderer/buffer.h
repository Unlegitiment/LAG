#pragma once
class grcBuffer {
public:
	grcBuffer() : m_pMemory(nullptr){}
	grcBuffer(void* mem) : m_pMemory(mem) {}
	void* GetRawMemory() { return this->m_pMemory; }
	//size_t GetMemorySize() { return sizeof this->m_pMemory; } // it doesn't have to be specific since this just represents a blank buffer. 
private:
	//void* m_pMemory;
};