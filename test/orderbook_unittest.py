#!/usr/bin/env python3

import ipaddress
import itertools
import unittest
import orderbook

        
class User:
    def __init__(self, ip, port):
        self.ip = int(ipaddress.IPv4Address(ip))
        self.port = port
        self.orderId = itertools.count(1)
        

buy, sell = 'B', 'S'


class TestOrderBook(unittest.TestCase):
    def testTrade(self):
        userA = User('192.168.0.1', 10000)
        userB = User('192.168.0.2', 20000)
       
        book = orderbook.OrderBook()
       
        self.assertEqual(book.newOrder(userA.ip, userA.port, next(userA.orderId), 'B', 100, 10),
                         [['A', 3232235521, 10000, 1], ['B', 'B', 100, 10]])
       
        self.assertEqual(book.newOrder(userB.ip, userB.port, next(userB.orderId), 'S', 100, 10),
                         [['A', 3232235522, 20000, 1], ['T', 3232235521, 10000, 1, 3232235522, 20000, 1, 100, 10], ['B', 'B', '-', '-']])
    
    
    def testCancel(self):
        userA = User('192.168.0.1', 10000)
        orderId = next(userA.orderId)
       
        book = orderbook.OrderBook()
       
       
        self.assertEqual(book.newOrder(userA.ip, userA.port, orderId, 'B', 100, 10),
                         [['A', 3232235521, 10000, 1], ['B', 'B', 100, 10]])
       
        self.assertEqual(book.cancelOrder(userA.ip, userA.port, orderId),
                         [['A', 3232235521, 10000, 1], ['B', 'B', '-', '-']])
        
        
    def testCancelAll(self):
        userA = User('192.168.0.1', 10000)
       
        book = orderbook.OrderBook()
       
       
        self.assertEqual(book.newOrder(userA.ip, userA.port, next(userA.orderId), 'B', 100, 10),
                         [['A', 3232235521, 10000, 1], ['B', 'B', 100, 10]])
        
        self.assertEqual(book.newOrder(userA.ip, userA.port, next(userA.orderId), 'S', 101, 10),
                         [['A', 3232235521, 10000, 2], ['B', 'S', 101, 10]])


        self.assertEqual(book.cancelAll(userA.ip, userA.port),
                         [['A', 3232235521, 10000, 1], ['B', 'B', '-', '-'],
                          ['A', 3232235521, 10000, 2], ['B', 'S', '-', '-']])
        

if __name__ == '__main__':
    unittest.main()

