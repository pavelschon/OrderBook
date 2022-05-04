/**
 * @brief OrderBook definition
 *
 */

#pragma once

#include "OrderBookFwd.hpp"
#include "OrderContainer.hpp"


class OrderBook
{
public:
    /* default ctor */
    OrderBook() = default;
    
    /* non-copyable */
    OrderBook( const OrderBook& ) = delete;
    
    /* non-assignable */
    OrderBook& operator=(const OrderBook&) = delete;
    
    /* Create new order */
    PyList newOrder(const unsigned int ip, const unsigned short port, const int orderId, const char side, const int price, const int qty);
    
    /* Cancel existing order */
    PyList cancelOrder(const unsigned int ip, const unsigned short port, const int orderId);
    
    /* Cancel all orders of user */
    PyList cancelAll(const unsigned int ip, const unsigned short port);
    
    /* get top-of-book */
    PyList topOfBook() const;
    
    /* Flush orderbook */
    void flush();
    
    /* Get the best price+quantity from the order container */
    template<class OrderContainer>
    static TopOfBook::Optional getTopOfBook(const OrderContainer& container);
    
private:
    /* Create new order */
    template<class OrderContainer, class OtherContainer>
    static void newOrderImpl(OrderContainer& container, OtherContainer& otherContainer, Response& response,
                             const unsigned int ip, const unsigned short port, const int orderId, const char side, const int price, const int qty);

    /* Cancel existing order */
    template<class OrderContainer>
    static void cancelOrderImpl(Response& response, OrderContainer& container, const unsigned int ip, const unsigned short port, const int orderId);
    
    /* Cancel existing order */
    template<class OrderContainer>
    static void cancelAllImpl(Response& response, OrderContainer& container, const unsigned int ip, const unsigned short port, const char side);
    
    /* Handle order execution (trade) */
    template<class OrderContainer>
    static int trade(Response& response, OrderContainer& container, Order& order);
        
    BidOrderContainer bidOrders;
    AskOrderContainer askOrders;
};

