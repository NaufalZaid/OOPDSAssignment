#ifndef QUEUE_H
#define QUEUE_H

#include <stdexcept>

/**
 * A simple templated circular queue with fixed capacity.
 * If you need more advanced features, use std::queue or std::deque directly.
 */
template <typename T> class Queue {
private:
  T *data;
  int size;
  int capacity;
  int frontIdx;
  int rearIdx;

public:
  explicit Queue(int cap = 5);
  ~Queue();

  void push(const T &value);
  void pop();
  T front() const;

  bool full() const;
  bool empty() const;
  int getSize() const { return size; }
};

#endif // QUEUE_H
