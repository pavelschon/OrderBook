# OrderBook
OrderBook implementation in C++17 and Python 3

## Dependencies

* Boost (particulary Boost Python and Boost Multi-Index container)
* Python 3.5+
* CMake

## Build

In the project folder:
```
mkdir obj-debug
cd obj-debug
cmake ..
make -j8 all
```
It creates orderbook.so (python module).

## Python API
In the obj-debug folder:
```python
$ python3
Python 3.5.3 (default, Jul  9 2020, 13:00:10) 
[GCC 6.3.0 20170516] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import orderbook
>>> book = orderbook.OrderBook()
>>> book
<orderbook.OrderBook object at 0x7fb9df520ce0>
>>> book.newOrder(1, 10, 100, 'B', 1)
[['A', 1, 1], ['B', 'B', 10, 100]]
>>> book.newOrder(1, 12, 100, 'S', 2)
[['A', 1, 2], ['B', 'S', 12, 100]]
>>> book.newOrder(2, 9, 100, 'B', 101)
[['A', 2, 101]]
>>> book.newOrder(2, 11, 100, 'S', 102)
[['A', 2, 102], ['B', 'S', 11, 100]]
>>> book.newOrder(1, 11, 100, 'B', 3)
[['A', 1, 3], ['T', 1, 3, 2, 102, 11, 100], ['B', 'S', 12, 100]]
>>> book.cancelOrder(1, 2)
[['A', 1, 2], ['B', 'S', '-', '-']]
>>> book.flush()
```

## Streaming
In the obj-debug folder:
```
$ cat input.txt | ../bin/orderbook_streamer.py
A, 1, 1
B, B, 10, 100
A, 1, 2
B, S, 12, 100
A, 2, 101
A, 2, 102
B, S, 11, 100
A, 1, 3
T, 1, 3, 2, 102, 11, 100
B, S, 12, 100
A, 2, 103
T, 1, 1, 2, 103, 10, 100
B, B, 9, 100
A, 1, 4
B, B, 10, 100
A, 2, 104
B, S, 11, 100
A, 1, 1
B, B, 10, 100
A, 2, 101
A, 2, 102
B, S, 11, 100
A, 1, 2
T, 1, 2, 2, 102, 11, 100
B, S, -, -
A, 2, 103
B, S, 11, 100
A, 1, 1
B, B, 10, 100
A, 1, 2
B, S, 12, 100
A, 2, 101
A, 2, 102
B, S, 11, 100
A, 1, 103
T, 1, 103, 2, 102, 11, 100
```
