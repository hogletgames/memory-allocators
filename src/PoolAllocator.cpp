#include "PoolAllocator.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>     /* malloc, free */

#ifdef _DEBUG
  #include <iostream>
#endif

PoolAllocator::PoolAllocator(const std::size_t totalSize, const std::size_t chunkSize) 
  : Allocator(totalSize) {
  assert(chunkSize >= 8 && "Chunk size must be greater or equal to 8");
  assert(totalSize % chunkSize == 0 && "Total Size must be a multiple of Chunk Size" );
  this->m_chunkSize = chunkSize;
}

void PoolAllocator::Init(){
  m_start_ptr = malloc(m_totalSize);
  this->Reset();
}

PoolAllocator::~PoolAllocator(){
  free(m_start_ptr);
}


void *PoolAllocator::Allocate(const std::size_t allocationSize, const std::size_t alignment)
{
  assert(allocationSize == this->m_chunkSize && "Allocation size must be equal to chunk size");
  
  FreeHeader freePosition = m_freeList.pop();

  assert(freePosition.self != nullptr && "The pool allocator is full");

  m_used += m_chunkSize;

#ifdef _DEBUG
  std::cout << "A" << "\t@S " << m_start_ptr << "\t@R " << (void*) freePosition.self << "\tM " << m_used << std::endl;
#endif

  return freePosition.self;
}

void PoolAllocator::Free(void * ptr)
{
  m_used -= m_chunkSize;
  
  m_freeList.push(FreeHeader{ptr}, ptr);

#ifdef _DEBUG
  std::cout << "F" << "\t@S " << m_start_ptr << "\t@F " << ptr << "\tM " << m_used << std::endl;
#endif
}

void PoolAllocator::Reset(){
  // Create a linked-list with all free positions
  const int nChunks = m_totalSize / m_chunkSize;
  for (int i = 0; i < nChunks; ++i){
    std::size_t address = (std::size_t) m_start_ptr + i * m_chunkSize;
    m_freeList.push(FreeHeader{(void*) address},(void*) address);
  }
}