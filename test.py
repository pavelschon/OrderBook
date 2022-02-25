#!/usr/bin/env python3

import unittest
import orderbook


userA, userB = 1, 2
buy, sell = 'B', 'S'


class TestOrderBook(unittest.TestCase):
    def testScenario1(self):
        '''scenario1, balanced book'''
        
        book = orderbook.OrderBook()

        # build book, TOB = 10/11
        # N, 1, IBM, 10, 100, B, 1
        book.newOrder(userA, 10, 100, buy, 1)
        # N, 1, IBM, 12, 100, S, 2
        book.newOrder(userA, 12, 100, sell, 2)
        # N, 2, IBM, 9, 100, B, 101
        book.newOrder(userB, 9, 100, buy, 101)
        # N, 2, IBM, 11, 100, S, 102
        book.newOrder(userB, 11, 100, sell, 102)
        
        # hit book on each side, generate trades, TOB = 9/12
        # N, 1, IBM, 11, 100, B, 3
        book.newOrder(userA, 11, 100, buy, 3)
        # N, 2, IBM, 10, 100, S, 103
        book.newOrder(userB, 10, 100, sell, 103)
        
        # replenish book on each side, TOB = 10/11
        # N, 1, IBM, 10, 100, B, 4
        book.newOrder(userA, 10, 100, buy, 4)
        # N, 2, IBM, 11, 100, S, 104
        book.newOrder(userB, 11, 100, sell, 104)
        
        # F
        book.flush()
        
        
    def testScenario2(self):
        '''scenario2, shallow bid'''
        
        book = orderbook.OrderBook()
        
        # build book, shallow bid, TOB = 10/11
        # N, 1, AAPL, 10, 100, B, 1
        book.newOrder(userA, 10, 100, buy, 1)
        
        # N, 1, AAPL, 12, 100, S, 2
        book.newOrder(userA, 12, 100, sell, 2)
        
        # N, 2, AAPL, 11, 100, S, 102
        book.newOrder(userB, 11, 100, sell, 102)
        
        # hit bid, generate trade, TOB = -/11
        # N, 2, AAPL, 10, 100, S, 103
        book.newOrder(userB, 10, 100, sell, 103)
        
        # replenish bid, TOB = 10/11
        # N, 1, AAPL, 10, 100, B, 3
        book.newOrder(userA, 10, 100, buy, 3)
        
        # F
        book.flush()
    
    
    def testScenario3(self):
        '''scenario3, shallow ask'''

        book = orderbook.OrderBook()
        
        # build book, shallow ask, TOB = 10/11
        # N, 1, VAL, 10, 100, B, 1
        book.newOrder(userA, 10, 100, buy, 1)
        
        # N, 2, VAL, 9, 100, B, 101
        book.newOrder(userB, 11, 100, sell, 101)
        
        # N, 2, VAL, 11, 100, S, 102
        book.newOrder(userB, 11, 100, sell, 102)
        
        # hit ask, generate trade, TOB = 10/-
        # N, 1, VAL, 11, 100, B, 2
        book.newOrder(userA, 11, 100, buy, 2)

        # replenish ask, TOB = 10/11
        # N, 2, VAL, 11, 100, S, 103
        book.newOrder(userB, 11, 100, sell, 103)
        
        # F
        book.flush()

    
    def testScenario4(self):
        '''scenario4, balanced book, limit below best bid'''

        book = orderbook.OrderBook()
        
        # build book, TOB = 10/11
        # N, 1, IBM, 10, 100, B, 1
        book.newOrder(userA, 10, 100, buy, 1)
        
        # N, 1, IBM, 12, 100, S, 2
        book.newOrder(userA, 12, 100, sell, 2)
        
        # N, 2, IBM, 9, 100, B, 101
        book.newOrder(userB, 9, 100, buy, 101)
        
        # N, 2, IBM, 11, 100, S, 102
        book.newOrder(userB, 11, 100, sell, 102)

        # limit below best bid, generate trade, TOB = 9/11
        # N, 2, IBM, 9, 100, S, 103
        book.newOrder(userB, 9, 100, sell, 103)
        
        # F
        book.flush()
        
    
    def testScenario5(self):
        '''scenario5, balanced book, limit above best ask'''
        
        book = orderbook.OrderBook()

        # build book, TOB = 10/11
        # N, 1, IBM, 10, 100, B, 1
        book.newOrder(userA, 10, 100, buy, 1)
        
        # N, 1, IBM, 12, 100, S, 2
        book.newOrder(userA, 12, 100, sell, 2)
        
        # N, 2, IBM, 9, 100, B, 101
        book.newOrder(userB, 9, 100, buy, 101)
        
        # N, 2, IBM, 11, 100, S, 102
        book.newOrder(userB, 11, 100, sell, 102)

        # limit above best ask, generate trade, TOB = 10/12
        # N, 1, IBM, 12, 100, B, 103
        book.newOrder(userA, 12, 100, buy, 103)
        
        # F
        book.flush()
        
        
    def testScenario6(self):
        '''scenario6, balanced book, market sell'''
        
        book = orderbook.OrderBook()

        # build book, TOB = 10/11
        # N, 1, IBM, 10, 100, B, 1
        book.newOrder(userA, 10, 100, buy, 1)
        
        # N, 1, IBM, 12, 100, S, 2
        book.newOrder(userA, 12, 100, sell, 2)
        
        # N, 2, IBM, 9, 100, B, 101
        book.newOrder(userB, 9, 100, buy, 101)
        
        # N, 2, IBM, 11, 100, S, 102
        book.newOrder(userB, 11, 100, sell, 102)

        # market sell, generate trade, TOB = 9/11
        # N, 2, IBM, 0, 100, S, 103
        book.newOrder(userB, 0, 100, sell, 103)
        
        # F
        book.flush()
        
        
    def testScenario7(self):
        '''scenario7, balanced book, market buy'''
        
        book = orderbook.OrderBook()

        # build book, TOB = 10/11
        # N, 1, IBM, 10, 100, B, 1
        book.newOrder(userA, 10, 100, buy, 1)
        
        # N, 1, IBM, 12, 100, S, 2
        book.newOrder(userA, 12, 100, sell, 2)
        
        # N, 2, IBM, 9, 100, B, 101
        book.newOrder(userB, 9, 100, buy, 101)
        
        # N, 2, IBM, 11, 100, S, 102
        book.newOrder(userB, 11, 100, sell, 102)
        
        book.flush()

        # market buy, generate trade, TOB = 10/12
        # N, 1, IBM, 0, 100, B, 3
        book.newOrder(userA, 0, 100, buy, 3)
        
        # F
        book.flush()
        
        
    def testScenario8(self):
        '''scenario8, tighten spread through new limit orders'''
        
        book = orderbook.OrderBook()

        # build book, TOB = 10/11
        # N, 1, IBM, 10, 100, B, 1
        book.newOrder(userA, 10, 100, buy, 1)
        
        # N, 1, IBM, 16, 100, S, 2
        book.newOrder(userA, 16, 100, sell, 2)
        
        # N, 2, IBM, 9, 100, B, 101
        book.newOrder(userB, 9, 100, buy, 101)
        
        # N, 2, IBM, 15, 100, S, 102
        book.newOrder(userB, 15, 100, sell, 102)

        # new bid, ask TOB = 11/14
        # N, 2, IBM, 11, 100, B, 103
        book.newOrder(userB, 11, 100, buy, 103)
        
        # N, 1, IBM, 14, 100, S, 3
        book.newOrder(userA, 14, 100, sell, 3)
        
        # F
        book.flush()
        
        
    def testScenario9(self):
        #scenario9, balanced book, market sell partial
        
        book = orderbook.OrderBook()

        # build book, TOB = 10/11
        # N, 1, IBM, 10, 100, B, 1
        book.newOrder(userA, 10, 100, buy, 1)
        
        # N, 1, IBM, 12, 100, S, 2
        book.newOrder(userA, 12, 100, sell, 2)
        
        # N, 2, IBM, 9, 100, B, 101
        book.newOrder(userB, 9, 100, buy, 101)
        
        # N, 2, IBM, 11, 100, S, 102
        book.newOrder(userB, 11, 100, sell, 102)

        # market sell, generate partial trade, TOB = 10/11
        # N, 2, IBM, 0, 20, S, 103
        book.newOrder(userB, 0, 20, sell, 103)
        
        # F
        book.flush()

        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        


if __name__ == '__main__':
    unittest.main()

