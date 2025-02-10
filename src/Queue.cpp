#include "Queue.h"

#include <iostream>
#include <stdexcept>

// Constructor
template <typename T>
Queue<T>::Queue(int cap)
    : data(new T[cap]), size(0), capacity(cap), frontIdx(-1), rearIdx(-1) {}

// Destructor
template <typename T> Queue<T>::~Queue() { delete[] data; }

// Push
template <typename T> void Queue<T>::push(const T &value) {
  if (full()) {
    throw std::runtime_error("Queue is full");
  }
  if (empty()) {
    frontIdx = 0;
  }
  rearIdx = (rearIdx + 1) % capacity;
  data[rearIdx] = value;
  size++;
}

// Pop
template <typename T> void Queue<T>::pop() {
  if (empty()) {
    throw std::runtime_error("Queue is empty");
  }
  if (frontIdx == rearIdx) {
    // Only one element
    frontIdx = rearIdx = -1;
  } else {
    frontIdx = (frontIdx + 1) % capacity;
  }
  size--;
}

// Front
template <typename T> T Queue<T>::front() const {
  if (empty()) {
    throw std::runtime_error("Queue is empty");
  }
  return data[frontIdx];
}

// Full
template <typename T> bool Queue<T>::full() const { return size == capacity; }

// Empty
template <typename T> bool Queue<T>::empty() const { return size == 0; }

// We must explicitly instantiate the templates we use, or put it all in a
// header-only.
template class Queue<int>; // Example instantiation if you only use int
// If you use Queue<Ship*>, you can do template class Queue<Ship*> in an
// appropriate .cpp file.
