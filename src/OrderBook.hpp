/**
 * @brief OrderBook definition
 *
 */

#pragma once

#include "Response.hpp"

#include <boost/optional.hpp>


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
    Response::Type newOrder(const int userId, const int price, const int qty, const char side, const int orderId);
    
    /* Cancel existing order */
    void cancelOrder(const int userId, const int userOrderId);
    
    /* Flush orderbook */
    void flush();
    
    /* Get the best price from the order container */
    template<class OrderContainer>
    static boost::optional<int> getBestPrice(const OrderContainer& container);
    
private:
    /* Create new order */
    template<class OrderContainer, class OtherContainer>
    static void newOrderImpl(OrderContainer& container, OtherContainer& otherContainer, Response& response,
                             const int price, const int qty, const char side, const int userId, const int orderId);

    /* Cancel existing order */
    template<class OrderContainer>
    static void cancelOrderImpl(OrderContainer& container, const int userId, const int orderId);
    
    /* Handle order execution (trade) */
    template<class OrderContainer>
    static int trade(Response& response, OrderContainer& container, const Order::SharedPtr& order);
        
    BidOrderContainer bidOrders;
    AskOrderContainer askOrders;
};

