#!/usr/bin/env python3

import time
import random
import orderbook
import argparse

# constants
prices = list(range(10, 21))
quantities = list(range(100, 201))
userIds = (1, 2, 3)
sides = ( 'B', 'S' )


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '-v', '--verbose',
        action = 'store_true',
        help = 'Print output from orderbook'
    )
    
    parser.add_argument(
        '-t', '--transactions',
        action = 'store',
        default = 100000,
        type = int,
        help = 'Count of transactions'
    )
    
    namespace = parser.parse_args()
    book = orderbook.OrderBook()
    t0 = time.time()

    for i in range(namespace.transactions):
        result = book.newOrder(
            random.choice(userIds),             # userId
            random.choice(prices),              # price
            random.choice(quantities),          # qty
            random.choice(sides),               # side
            i                                   # orderId
        )
        
        if namespace.verbose:
            print(result)

    t = time.time() - t0
    
    print('Processed {} transactions in {} seconds'.format(namespace.transactions, t))
    
