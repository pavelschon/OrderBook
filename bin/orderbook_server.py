#!/usr/bin/env python3

import argparse
import sys
import os

from twisted.internet import reactor, protocol
from twisted.logger import Logger, textFileLogObserver


def LineBuffer(logger, streamer):
    '''accumulate data, split to lines, send to the streamer'''
    
    data, separator = b'', b'\r\n'
    
    while True:
        data += (yield)
        
        while True:
            boundary = data.find(separator)
            
            if boundary < 0:
                break

            message = data[0:boundary].strip()
            
            logger.info('Input: {msg}', msg=message.decode('ascii'))
            
            streamer.transport.write(message)
            streamer.transport.write(b'\n')
            
            data = data[boundary+1:]


class OrderBookSubprocess(protocol.ProcessProtocol):
    logger = Logger(observer=textFileLogObserver(sys.stdout))
    
    def outReceived(self, data):
        for line in data.decode('ascii').split('\n'):
            self.logger.info(line)
        
    def errReceived(self, data):
        for line in data.decode('ascii').split('\n'):
            self.logger.error(line)
            
    def processExited(self, status):
        self.logger.error(str(status))
        
    def processEnded(self, status):
        reactor.callFromThread(reactor.stop)


class OrderBookUDP(protocol.DatagramProtocol):
    logger = Logger(observer=textFileLogObserver(sys.stdout))
    
    def __init__(self, streamer):
        self.lineBuffer = LineBuffer(self.logger, streamer)
        next(self.lineBuffer)
                
    
    def datagramReceived(self, data, addr):
        self.lineBuffer.send(data)
        
        
class OrderBookTCP(protocol.Protocol):
    logger = Logger(observer=textFileLogObserver(sys.stdout))
    
    def __init__(self, streamer, addr):
        self.addr = addr
        self.lineBuffer = LineBuffer(self.logger, streamer)
        next(self.lineBuffer)
        
        
    def connectionMade(self):
        self.logger.info('{addr.host}:{addr.port} connected', addr=self.addr)
    
    
    def connectionLost(self, reason):
        self.logger.info('{addr.host}:{addr.port} disconnected, reason: {reason}',
                         addr=self.addr, reason=reason)
    
    
    def dataReceived(self, data):
        self.lineBuffer.send(data)


class OrderBookFactory(protocol.Factory):
    def __init__(self, streamer):
        self.streamer = streamer
        
        
    def buildProtocol(self, addr):
        return OrderBookTCP(self.streamer, addr)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '-p', '--port',
        type = int,
        default = 1234,
        help = 'Listen port'
    )
    
    parser.add_argument(
        '-s', '--streamer',
        default = '/usr/bin/orderbook_streamer.py',
        help = 'Path to orderbook_streamer.py'
    )
    
    namespace = parser.parse_args()
    
    logger = Logger(observer=textFileLogObserver(sys.stdout))
    logger.info('Starting orderbook streamer subprocess: {path}', path=namespace.streamer)
    
    streamer = OrderBookSubprocess()
    
    reactor.spawnProcess(
        streamer, namespace.streamer, args=[os.path.basename(namespace.streamer)])
    
    reactor.listenUDP(namespace.port, OrderBookUDP(streamer))
    reactor.listenTCP(namespace.port, OrderBookFactory(streamer))
    reactor.run()

