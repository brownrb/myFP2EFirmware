/*
 * Queue.h
 * 
 * By Steven de Salas
 * 
 * Defines a templated (generic) class for a queue of things.
 * Used for Arduino projects, just #include "Queue.h" and add this file via the IDE.
 * 
 * Examples:
 * 
 * Queue<char> queue(10); // Max 10 chars in this queue
 * queue.push('H');
 * queue.push('e');
 * queue.count(); // 2
 * queue.push('l');
 * queue.push('l');
 * queue.count(); // 4
 * Serial.print(queue.pop()); // H
 * Serial.print(queue.pop()); // e
 * queue.count(); // 2
 * queue.push('o');
 * queue.count(); // 3
 * Serial.print(queue.pop()); // l
 * Serial.print(queue.pop()); // l
 * Serial.print(queue.pop()); // o
 * 
 * struct Point { int x; int y; }
 * Queue<Point> points(5);
 * points.push(Point{2,4});
 * points.push(Point{5,0});
 * points.count(); // 2
 * 
 */

#ifndef ESPQueue_H
#define ESPQueue_H

//#include <Arduino.h>

class Queue {
  private:
    int _front, _back, _count;
    String *_data;
    int _maxitems;
  public:
    Queue(int maxitems = 256) 
    { 
      _front = 0;
      _back = 0;
      _count = 0;
      _maxitems = maxitems;
      _data = new String[maxitems + 1];   
    }
    ~Queue() 
    {
      delete[] _data;  
    }
    inline int count();
    inline int front();
    inline int back();
    void push(const String &item);
    String peek();
    String pop();
    void clear();
};

inline int Queue::count() 
{
  return _count;
}

inline int Queue::front() 
{
  return _front;
}

inline int Queue::back() 
{
  return _back;
}

void Queue::push(const String &item)
{
  if(_count < _maxitems) 
  { 
    // Drops out when full
    _data[_back++]=item;
    ++_count;
    // Check wrap around
    if (_back > _maxitems)
    {
      _back -= (_maxitems + 1);
    }
  }
}

String Queue::pop() {
  if(_count <= 0) 
  {
    return String(); // Returns empty
  }
  else 
  {
    String result = _data[_front];
    _front++;
    --_count;
    // Check wrap around
    if (_front > _maxitems) 
    {
      _front -= (_maxitems + 1);
    }
    return result; 
  }
}

String Queue::peek() {
  if(_count <= 0)
  {
    return String(); // Returns empty
  }
  else
  {
    return _data[_front];
  }
}

void Queue::clear() 
{
  _front = _back;
  _count = 0;
}

#endif
