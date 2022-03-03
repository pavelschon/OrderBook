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
$ make -j8 install
```
On Debian/Ubuntu distro, run:
```
$ make -j8 package
$ dpkg -i orderbook-1.0.0-Linux.deb
$ apt-get install --fix-missing
```

Hint: compiled extension module (_orderbook.so_) must be on PYTHONPATH in order to be importable to python.


## Architecture
* Python API
* stdin/stdout streamer
* Twisted TCP/UDP server running streamer subprocess

## Python API
The _orderbook_ module exposes single class - _OrderBook_ with instancementhods:
 * newOrder
 * cancelOrder
 * flush

Usage:
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

```
$ /usr/share/orderbook/test/orderbook_unittest.py
................
----------------------------------------------------------------------
Ran 16 tests in 0.004s

OK
```

## Performance test
Performance test implements random transactions.

```
$ /usr/share/orderbook/test/orderbook_perftest.py 
Processed 100000 transactions in 4.149225950241089 seconds
```

## Streaming via stdin
The script _orderbook_streamer.py_ implements message parser from stdin, dispatch messages, call _orderbook_ API and format output messages to stdout.

```
$ /usr/bin/orderbook_streamer.py
#scenario1, balanced book

# build book, TOB = 10/11
N, 1, IBM, 10, 100, B, 1
N, 1, IBM, 12, 100, S, 2
N, 2, IBM, 9, 100, B, 101
N, 2, IBM, 11, 100, S, 102 

# hit book on each side, generate trades, TOB = 9/12
N, 1, IBM, 11, 100, B, 3
N, 2, IBM, 10, 100, S, 103

# replenish book on each side, TOB = 10/11
N, 1, IBM, 10, 100, B, 4
N, 2, IBM, 11, 100, S, 104
F

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
```

## Running TCP/UDP server
```
$ orderbook_server.py 
2022-02-27T23:15:23+0000 [__main__#info] Starting orderbook streamer subprocess: /usr/bin/orderbook_streamer.py
```

Check how the process tree looks:
```
$ ps f
  PID TTY      STAT   TIME COMMAND
13706 pts/3    Ss     0:00 -bash
13712 pts/3    R+     0:00  \_ ps f
 5304 pts/2    Ss     0:00 -bash
13697 pts/2    S+     0:00  \_ python3 /usr/bin/orderbook_server.py
13698 pts/2    S+     0:00      \_ python3 /usr/bin/orderbook_streamer.py
```

Send messages via network:
```
cat input.csv | netcat -u 192.168.110.22 1234
```

```
$ orderbook_server.py 
2022-02-27T23:15:23+0000 [__main__#info] Starting orderbook streamer subprocess: /usr/bin/orderbook_streamer.py
2022-02-27T23:18:35+0000 [__main__.OrderBookTCP#info] 192.168.100.4:43180 connected
2022-02-27T23:18:35+0000 [__main__.OrderBookTCP#info] Input: #scenario1, balanced book
2022-02-27T23:18:35+0000 [__main__.OrderBookTCP#info] Input: 
2022-02-27T23:18:35+0000 [__main__.OrderBookTCP#info] Input: # build book, TOB = 10/11
2022-02-27T23:18:35+0000 [__main__.OrderBookTCP#info] Input: N, 1, IBM, 10, 100, B, 1
2022-02-27T23:18:35+0000 [__main__.OrderBookTCP#info] Input: N, 1, IBM, 12, 100, S, 2
2022-02-27T23:18:35+0000 [__main__.OrderBookTCP#info] Input: N, 2, IBM, 9, 100, B, 101
2022-02-27T23:18:35+0000 [__main__.OrderBookTCP#info] Input: N, 2, IBM, 11, 100, S, 102
<<snip>>
2022-02-27T23:18:35+0000 [__main__.OrderBookSubprocess#info] A, 1, 1
2022-02-27T23:18:35+0000 [__main__.OrderBookSubprocess#info] B, B, 10, 100
2022-02-27T23:18:35+0000 [__main__.OrderBookSubprocess#info] A, 1, 2
2022-02-27T23:18:35+0000 [__main__.OrderBookSubprocess#info] B, S, 12, 100
2022-02-27T23:18:35+0000 [__main__.OrderBookSubprocess#info] A, 2, 101
2022-02-27T23:18:35+0000 [__main__.OrderBookSubprocess#info] A, 2, 102
2022-02-27T23:18:35+0000 [__main__.OrderBookSubprocess#info] B, S, 11, 100
2022-02-27T23:18:35+0000 [__main__.OrderBookSubprocess#info] A, 1, 3
2022-02-27T23:18:35+0000 [__main__.OrderBookSubprocess#info] T, 1, 3, 2, 102, 11, 100
2022-02-27T23:18:35+0000 [__main__.OrderBookSubprocess#info] B, S, 12, 100
2022-02-27T23:18:35+0000 [__main__.OrderBookSubprocess#info] A, 2, 103
2022-02-27T23:18:35+0000 [__main__.OrderBookSubprocess#info] T, 1, 1, 2, 103, 10, 100
```

## Notes and ideas for enhancements
* Use strong types in C++ like Price, Quantity etc, in order to avoid mismatch.
* Use PyBind11 instead of Boost Python
* Implement _modifyOrder_ message
* Distinguish public (market-data) and private messages
* Publish output messages to the (TCP) client
* Publish output messages via UDP multicast
* _cancelOrder_ message was not explained in ProgrammingExcercise.docx thus I implemented it according to my best experience.
* I've found few errors in outputFile.csv, which seems like copy-paste errors, e.g. swapped price and quantity.
