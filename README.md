# OrderBook
OrderBook implementation in C++14 and Python 3

## Dependencies

* Boost (particulary Boost Python and Boost Multi-Index container)
* Python3-Twisted
* CMake

## Build

In the project folder:
```
$ mkdir obj-debug
$ cd obj-debug
$ cmake ..
$ make -j8 all
```
It creates _orderbook.so_, a python extension module, which can be then imported to python.


## Architecture
* Python API
* stdin/stdout streamer
* Twisted TCP/UDP server running streamer subprocess:

```
 2417 pts/3    Ss     0:00  |   \_ /bin/bash
13659 pts/3    R+     0:30  |   |   \_ python3 /usr/bin/orderbook_server.py
13661 pts/3    S+     0:01  |   |       \_ python3 /usr/bin/orderbook_streamer.py
```

## Python API
The _orderbook_ module exposes single class - _OrderBook_ with instancementhods:
 * newOrder
 * cancelOrder
 * flush

In the obj-debug folder (scenario 10):
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

## Unit test
Unit test of the Python API implements scenarios 1-16 from the provided input and output files.

In the obj-debug folder:

```
$ ../test/orderbook_unittest.py 
................
----------------------------------------------------------------------
Ran 16 tests in 0.004s

OK
```

## Performance test
Performance test implements random transactions.

In the obj-debug folder:

```
$ ../test/orderbook_perftest.py  
Processed 100000 transactions in 5.980295181274414 seconds
```

## Streaming
The script _orderbook_streamer.py_ implements message parser from stdin, dispatch messages, call _orderbook_ API and format output messages to stdout.

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
