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
* Twisted TCP/UDP server running streamer subprocess

## Python API
The _orderbook_ module exposes single class - _OrderBook_ with instancementhods:
 * newOrder
 * cancelOrder
 * cancelAll
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



## Running TCP/UDP server
```
$ orderbook_server.py  BTCUSD:8001 BTCEUR:8002
2022-05-04T16:32:38+0000 [__main__.OrderBookFactory#info] Started orderbook for BTCUSD on port tcp/8001
2022-05-04T16:32:38+0000 [__main__.OrderBookFactory#info] Started orderbook for BTCEUR on port tcp/8002
```

## Notes and ideas for enhancements
* Use strong types in C++ like Price, Quantity etc, in order to avoid mismatch.
* Use PyBind11 instead of Boost Python
* Implement _modifyOrder_ message

