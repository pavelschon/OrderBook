#!/usr/bin/env python3

import unittest
import orderbook


userA, userB = 1, 2
buy, sell = 'B', 'S'


class TestOrderBook(unittest.TestCase):
    def assertResult(self, resultA, resultB):
        self.assertEqual(len(resultA), len(resultB))
        for messageA, messageB in zip(resultA, resultB):
            self.assertMessage(messageA, messageB)
                
    def assertMessage(self, messageA, messageB):
        self.assertEqual(len(messageA), len(messageB))
        for itemA, itemB in zip(messageA, messageB):
            self.assertEqual(itemA, itemB)
        
    
    def testScenario01(self):
        '''scenario1, balanced book'''
        
        book = orderbook.OrderBook()

        # build book, TOB = 10/11
        # N, 1, IBM, 10, 100, B, 1
        self.assertResult(
            book.newOrder(userA, 10, 100, buy, 1),
            [[ 'A', 1, 1 ],
             ['B', 'B', 10, 100]]
        )
        
        # N, 1, IBM, 12, 100, S, 2
        self.assertResult(
            book.newOrder(userA, 12, 100, sell, 2),
            [[ 'A', 1, 2],
             [ 'B', 'S', 12, 100]])
        
        # N, 2, IBM, 9, 100, B, 101
        self.assertResult(
            book.newOrder(userB, 9, 100, buy, 101),
            [[ 'A', 2, 101]])
        
        # N, 2, IBM, 11, 100, S, 102
        self.assertResult(
            book.newOrder(userB, 11, 100, sell, 102),
            [[ 'A', 2, 102],
             [ 'B', 'S', 11, 100]])
        
        # hit book on each side, generate trades, TOB = 9/12
        # N, 1, IBM, 11, 100, B, 3
        self.assertResult(
            book.newOrder(userA, 11, 100, buy, 3),
            [[ 'A', 1, 3],
             [ 'T', 1, 3, 2, 102, 11, 100 ],
             ['B', 'S', 12, 100 ]])
        
        # N, 2, IBM, 10, 100, S, 103
        self.assertEqual(
            book.newOrder(userB, 10, 100, sell, 103),
            [[ 'A', 2, 103 ],
             [ 'T', 2, 103, 1, 1, 10, 100 ],
             [ 'B', 'B', 9, 100 ]])
        
        # replenish book on each side, TOB = 10/11
        # N, 1, IBM, 10, 100, B, 4
        self.assertEqual(
            book.newOrder(userA, 10, 100, buy, 4),
            [[ 'A', 1, 4 ],
             [ 'B', 'B', 10, 100 ]])
        
        # N, 2, IBM, 11, 100, S, 104
        self.assertEqual(
            book.newOrder(userB, 11, 100, sell, 104),
            [[ 'A', 2, 104 ],
             ['B', 'S', 11, 100 ]])
        
        # F
        book.flush()
        
        
    def testScenario02(self):
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
    
    
    def testScenario03(self):
        '''scenario3, shallow ask'''

        book = orderbook.OrderBook()
        
        # build book, shallow ask, TOB = 10/11
        # N, 1, VAL, 10, 100, B, 1
        self.assertEqual(
            book.newOrder(userA, 10, 100, buy, 1),
            [[ 'A', 1, 1 ],
             [ 'B', 'B', 10, 100 ]])
        
        # N, 2, VAL, 9, 100, B, 101
        self.assertEqual(
            book.newOrder(userB, 9, 100, buy, 101),
            [[ 'A', 2, 101 ]])
        
        # N, 2, VAL, 11, 100, S, 102
        self.assertEqual(
            book.newOrder(userB, 11, 100, sell, 102),
            [[ 'A', 2, 102],
             [ 'B', 'S', 11, 100 ]])
        
        # hit ask, generate trade, TOB = 10/-
        # N, 1, VAL, 11, 100, B, 2
        self.assertEqual(
            book.newOrder(userA, 11, 100, buy, 2),
            [[ 'A', 1, 2 ],
             [ 'T', 1, 2, 2, 102, 11, 100 ],
             [ 'B', 'S', '-', '-' ]])

        # replenish ask, TOB = 10/11
        # N, 2, VAL, 11, 100, S, 103
        self.assertEqual(
            book.newOrder(userB, 11, 100, sell, 103),
            [[ 'A', 2, 103 ],
             [ 'B', 'S', 11, 100 ]])
        
        # F
        book.flush()

    
    def testScenario04(self):
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
        
    
    def testScenario05(self):
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
        
        
    def testScenario06(self):
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
        
        
    def testScenario07(self):
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
        
        
    def testScenario08(self):
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
        
        
    def testScenario09(self):
        '''scenario9, balanced book, market sell partial'''
        
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

        
    def testScenario10(self):
        '''scenario10, balanced book, market buy partial'''
        
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

        # market buy, generate partial trade, TOB = 10/11
        # N, 1, IBM, 0, 20, B, 3
        book.newOrder(userA, 0, 20, buy, 3)
        
        # F
        book.flush()
    
    
    def testScenario11(self):
        '''scenario11, balanced book, limit sell partial'''
        
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

        # limit sell, generate partial trade, TOB = 10/11
        # N, 2, IBM, 10, 20, S, 103
        # F
        book.flush()
        
        
    def testScenario12(self):
        '''scenario12, balanced book, limit buy partial'''
        
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

        # limit buy, generate partial trade, TOB = 10/11
        # N, 1, IBM, 11, 20, B, 3
        book.newOrder(userA, 11, 20, buy, 3)
        
        # F
        book.flush()
        
    
    def testScenario13(self):
        '''scenario13, balanced book multiple orders at best bid offer'''
        
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
        
        # N, 2, IBM, 10, 50, B, 103
        book.newOrder(userB, 10, 50, buy, 103)
        
        # N, 1, IBM, 11, 50, S, 3
        book.newOrder(userA, 11, 50, sell, 3)
        

        # market buy and sell, generate trades, TOB = 10/11
        # N, 1, IBM, 11, 100, B, 4
        book.newOrder(userA, 11, 100, buy, 4)
        
        # N, 2, IBM, 10, 100, S, 104
        book.newOrder(userB, 10, 100, sell, 104)
        
        # F
        book.flush()
        
    
    def testScenario14(self):
        '''scenario14, balanced book, cancel best bid and offer'''
        
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

        # limit buy, generate partial trade, TOB = 9/12
        # C, 1, 1
        book.cancelOrder(userA, 1)
        
        # C, 2, 102
        book.cancelOrder(userB, 102)
        
        # F
        book.flush()
        
    
    def testScenario15(self):
        '''scenario15, balanced book, cancel behind best bid and offer'''
        
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

        # cancel orders, TOB = 10/11
        # C, 1, 2
        book.cancelOrder(userA, 2)
        
        # C, 2, 101
        book.cancelOrder(userB, 101)
        
        # F
        book.flush()
        
        
    def testScenario16(self):
        '''scenario16, balanced book, cancel all bids'''
        
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

        # cancel all bids, TOB = -/11
        # C, 1, 1
        book.cancelOrder(userA, 1)
        
        # C, 2, 101
        book.cancelOrder(userB, 101)
        
        # F
        book.flush()
        
        


if __name__ == '__main__':
    unittest.main()

