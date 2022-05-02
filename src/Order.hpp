/**
 * @brief Order definition
 *
 */

#pragma once

#include "OrderBookFwd.hpp"


class Order
{
    /* access to private members is allowed */
    friend class OrderBook;
    
public:
    /* ctor */
    Order(const unsigned int ip_, const unsigned short port_, const int orderId_, const char side_, const int price_, const int qty_);
    
    /* Check if this order is executable with other order */
    bool isTradeableWith( const Order& order ) const;

    /* Get order time priority */
    Time getTime( void ) const;
    
    /* Get order price */
    int getPrice( void ) const;
    
    /* Get order quantity */
    int getQty( void ) const;
    
    /* Get order unique id */
    UniqueId getUniqueId( void ) const;
    
    /* Get order unique id */
    UserId getUserId( void ) const;
    
    /* Get order user id */
    unsigned int getIp( void ) const;
    
    /* Get order user id */
    unsigned short getPort( void ) const;
    
    /* Get order id */
    int getOrderId( void ) const;
    
    /* get side of this order */
    char getSide() const;
    
    /* get other side of this order */
    char getOtherSide() const;

private:
    const Time time;
    const unsigned int ip;
    const unsigned short port;
    const int orderId;
    const char side;
    const int price;
    
    /* quantity changes during (partial) execution, hence not const */
    int qty;
};
