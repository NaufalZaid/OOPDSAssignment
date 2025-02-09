#include <exception>
#include <iostream>
#include <stdexcept>
using namespace std;

// implement a queue using a circular array
template <typename T> class Queue {
  T *data;
  int size;
  int capacity;
  int front;
  int rear;

public:
  Queue(int cap = 5) {
    size = 0;
    capacity = cap;
    data = new T[capacity];
    front = rear = -1;
  }

  ~Queue() { delete[] data; }

  void push(T value) {
    if (full())
      throw runtime_error("Queue is full");

    if (empty())
      front = 0;
    rear = (rear + 1) % capacity;
    data[rear] = value;
    size++;
  }

  void pop() {
    if (empty())
      throw runtime_error("Queue is empty");

    if (front == rear)
      front = rear = -1;
    else
      front = (front + 1) % capacity;
    size--;
  }

  T Front() {
    if (empty())
      throw runtime_error("Queue is empty");
    return data[front];
  }

  bool full() { return size == capacity; }

  bool empty() { return size == 0; }
};