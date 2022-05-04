#!/usr/bin/env python3

import re
import sys
import codecs
import weakref
import argparse
import ipaddress
import collections

import orderbook

from twisted.internet import reactor, protocol
from twisted.protocols import basic
from twisted.logger import Logger, textFileLogObserver

# N 1 B 100 10
# N 2 S 150 2

class NewOrder:
    def __init__(self, *args):
        orderId, side, price, quantity = args
        
        self.orderId = int(orderId)
        self.side = side
        self.price = int(price)
        self.quantity = int(quantity)
        
        
    def toTuple(self, ip, port):
        return ip, port, self.orderId, self.side, self.price, self.quantity
    
class ModifyOrder:
    def __init__(self, *args):
        orderId, quantity = args
        
        self.orderId = int(orderId)
        self.quantity = int(quantity) # new quantity to be changed
        
    def toTuple(self, ip, port):
        return ip, port, self.orderId, self.quantity
        
        
class CancelOrder:
    def __init__(self, *args):
        orderId, = args
        
        self.orderId = int(orderId)
        
        
    def toTuple(self, ip, port):
        return ip, port, self.orderId
        
        
class TopOfBook:
    def __init__(self, *args):
        self.message_type, self.side, self.price, self.quantity = args
    
    
    def toTuple(self):
        return self.message_type, self.side, self.price, self.quantity
        
        
class Ack:
    def __init__(self, *args):
        self.message_type, self.ip, self.port, self.orderId = args
        
        
    def toTuple(self):
        return (self.message_type, self.orderId)
        
        
class Trade:
    def __init__(self, *args):
        ( self.message_type, self.bidIp, self.bidPort, self.bidOrderId,
         self.askIp, self.askPort, self.askOrderId, self.matchPrice, self.matchQuantity ) = args
        
        
    def toBid(self):
        return ( self.message_type, self.bidOrderId, self.matchPrice, self.matchQuantity )
    
    
    def toAsk(self):
        return ( self.message_type, self.askOrderId, self.matchPrice, self.matchQuantity )

        
class OrderBookTCP(basic.LineReceiver):
    logger = Logger(observer=textFileLogObserver(sys.stdout))
    
    def __init__(self, factory, addr):
        self.factory = weakref.proxy(factory)
        self.addr = addr
        self.ip = int(ipaddress.ip_address(addr.host))
        
        # add self to connected clients
        self.factory.addClient(self)
        
        
    
    ##
    # @brief On new connection, 
    #
    #
    def connectionMade(self):
        result = self.factory.orderbook.topOfBook()
        
        for message in result:
            topOfBook = TopOfBook(*message)
            
            self.transport.write(self.messageToBytes(topOfBook.toTuple()))
            self.transport.write(self.delimiter)
        
        self.logger.info('{addr.host}:{addr.port} connected', addr=self.addr)
    
    
    
    ##
    # @brief Connection lost handler
    #
    #
    def connectionLost(self, reason):
        result = self.factory.orderbook.cancelAll(self.ip, self.addr.port)
        
        self.dispatchResult(result)
        
        # delete self from connected clients
        self.factory.delClient(self)
        
        self.logger.info('{addr.host}:{addr.port} disconnected, reason: {reason}',
                         addr=self.addr, reason=reason)
    
    
    ##
    # @brief Dispatch incoming messages
    #
    #
    def lineReceived(self, data):
        message_type, *args = data.decode('ascii').split(' ')
        
        if message_type == 'N':
            self.newOrder(args)
            
        elif message_type == 'M':
            self.modiyOrder(args)
        
        elif message_type == 'C':
            self.cancelOrder(args)
            
        else:
            self.onError(ValueError('Invalid message'))
            
        
    ##
    # @brief Dispatch NewOrder messages
    #
    #
    def newOrder(self, args):
        try:
            newOrder = NewOrder(*args)
        except (TypeError, ValueError) as e:
            self.onError(e)
        
        else:
            result = self.factory.orderbook.newOrder(
                *newOrder.toTuple(self.ip, self.addr.port))
            
            self.dispatchResult(result)
    
    
    ##
    # @brief Implement tmodify order message
    #
    #
    def modifyorder(self, args):
        try:
            modifiedOrder = ModifyOrder(*args)
        except (TypeError, ValueError) as e:
            self.onError(e)
        else:
            result = self.factory.orderbook.modifyOrder(*modifedOrder.toTuple(self.ip, self.port))
            
            self.dispatchResult(result)

    
    ##
    # @brief Dispatch CancelOrder messages
    #
    #
    def cancelOrder(self, args):
        try:
            cancelOrder = CancelOrder(*args)
        except (TypeError, ValueError) as e:
            self.onError(e)
            
        else:
            result = self.factory.orderbook.cancelOrder(
                *cancelOrder.toTuple(self.ip, self.addr.port))
            
            self.dispatchResult(result)
            
    
    ##
    # @brief Dispatch result messages
    #
    #
    def dispatchResult(self, result):
        for message in result:
            if message[0] == 'B':
                self.sendTopOfBook(message)
            
            elif message[0] == 'A':
                self.sendAck(message)
                
            elif message[0] == 'T':
                self.sendTrade(message)
    
    
    ##
    # @brief Send TopOfBook message
    #
    #
    def sendTopOfBook(self, message):
        topOfBook = TopOfBook(*message)
                
        self.factory.sendAll(self.messageToBytes(topOfBook.toTuple()))
        
    
    ##
    # @brief Send Ack message
    #
    #
    def sendAck(self, message):
        ack = Ack(*message)
                
        self.transport.write(self.messageToBytes(ack.toTuple()))
        self.transport.write(self.delimiter)
        
    
    
    ##
    # @brief Send Trade message
    #
    #
    def sendTrade(self, message):
        trade = Trade(*message)
        
        self.factory.sendToClient(trade.bidIp, trade.bidPort, self.messageToBytes(trade.toBid()))
        self.factory.sendToClient(trade.askIp, trade.askPort, self.messageToBytes(trade.toAsk()))
    
    
    ##
    # @brief Log error and disconnect client
    #
    #
    def onError(self, err):
        self.logger.error('{addr.host}:{addr.port}: {err}', addr=self.addr, err=err)
        
        self.transport.loseConnection()
        
    
    ##
    # @brief Convert message tuple to bytes
    #
    #
    @staticmethod
    def messageToBytes(message):
        return b' '.join(codecs.iterencode(map(str, message), 'ascii'))


class OrderBookFactory(protocol.Factory):
    logger = Logger(observer=textFileLogObserver(sys.stdout))
    
    ##
    # @brief Constructor
    #
    #
    def __init__(self, symbol, port):
        self.symbol = symbol
        self.orderbook = orderbook.OrderBook()
        self.clients = {}
        
        self.logger.info('Started orderbook for {} on port tcp/{}'.format(symbol, port))
        
        
    ##
    # @brief Build client
    #
    #
    def buildProtocol(self, addr):
        return OrderBookTCP(self, addr)
    
    
    ##
    # @brief Register client
    #
    #
    def addClient(self, client):
        self.clients[client.addr] = client
    
    
    ##
    # @brief Unregister client
    #
    #
    def delClient(self, client):
        del(self.clients[client.addr])
        
    
    ##
    # @brief Send message to all clients
    #
    #
    def sendAll(self, message):
        for client in self.clients.values():
            client.transport.write(message)
            client.transport.write(OrderBookTCP.delimiter)
            
           
    ##
    # @brief Send message to client specified by ip and port
    #
    #
    def sendToClient(self, ip, port, message):
        for client in self.clients.values():
            if all((client.ip == ip, client.addr.port == port)):
                client.transport.write(message)
                client.transport.write(OrderBookTCP.delimiter)


symbol_re = re.compile('[A-Z]+\:\d+')

##
# @brief Validate symbol:port
#
#
def symbol(value):
    if not symbol_re.match(value):
        raise argparse.ArgumentTypeError('invalid symbol: {}'.format(value))
    return value


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        'symbols',
        type = symbol,
        metavar ='symbol:port',
        nargs = '+',
        help = 'Symbols'
    )
    
    namespace = parser.parse_args()
    
    for value in namespace.symbols:
        symbol, port = value.split(':')
        
        reactor.listenTCP(int(port), OrderBookFactory(symbol, port))
    
    reactor.run()

