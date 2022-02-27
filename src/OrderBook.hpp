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
    PyList newOrder(const int userId, const int price, const int qty, const char side, const int orderId);
    
    /* Cancel existing order */
    PyList cancelOrder(const int userId, const int userOrderId);
    
    /* Flush orderbook */
    void flush();
    
    /* Get the best price+quantity from the order container */
    template<class OrderContainer>
    static TopOfBook::Optional getTopOfBook(const OrderContainer& container);
    
private:
    /* Create new order */
    template<class OrderContainer, class OtherContainer>
    static void newOrderImpl(OrderContainer& container, OtherContainer& otherContainer, Response& response,
                             const int price, const int qty, const char side, const int userId, const int orderId);

    /* Cancel existing order */
    template<class OrderContainer>
    static void cancelOrderImpl(Response& response, OrderContainer& container, const int userId, const int orderId);
    
    /* Handle order execution (trade) */
    template<class OrderContainer>
    static int trade(Response& response, OrderContainer& container, const OrderPtr& order);
        
    BidOrderContainer bidOrders;
    AskOrderContainer askOrders;
};

