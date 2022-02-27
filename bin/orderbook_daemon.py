#!/usr/bin/env python3
##
# @brief Streaming wrapper for orderbook module
#
# Usage:
#     ./orderbook_daemon.py < input_file.txt
#
# or
#     cat input_file.txt | ./orderbook_daemon.py
#
#

import sys
import signal
import orderbook
            

def print_result():
    '''print formatted result to stdout'''
    
    while True:
        result = (yield)
        for response in result:
            print(', '.join(map(str, response)))
            
            
def dispatch_message(target):
    '''validate message parameters and call orderbook API'''
    
    book, orderbooks = None, {}
    
    def get_orderbook(symbol):
        '''get or create orderbook instance'''
        
        try:
            return orderbooks[symbol]
        except KeyError:
            orderbooks[symbol] = orderbook.OrderBook()
            return orderbooks[symbol]
    
    
    while True:
        message_type, *args = (yield).split(',')
        
        if message_type == 'N':
            try:
                userId, symbol, price, qty, side, orderId = args
                userId = int(userId)
                price = int(price)
                qty = int(qty)
                orderId = int(orderId)
                side = side.strip()
            except (TypeError, ValueError):
                continue
            
            book = get_orderbook(symbol)
            target.send(book.newOrder(userId, price, qty, side, orderId))
        
        elif message_type == 'C':
            try:
                userId, orderId = args
                userId = int(userId)
                orderId = int(orderId)
            except (TypeError, ValueError):
                continue
            
            if book is not None:
                target.send(book.cancelOrder(userId, orderId))
                
        elif message_type == 'F':
            if book is not None:
                book.flush()

            
def read_messages(target):
    while True:
        message = (yield).strip()
        
        # skip comments and blank lines
        if not message or message.startswith('#'):
            continue
        
        target.send(message)


if __name__ == '__main__':
    result_printer = print_result()
    next(result_printer)
    
    message_dispatcher = dispatch_message(result_printer)
    next(message_dispatcher)
    
    message_reader = read_messages(message_dispatcher)
    next(message_reader)
    
    i = 0
    
    while True:
        try:
            message_reader.send(input())
        except (KeyboardInterrupt, EOFError):
            break

