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
    Order(const int price_, const int qty_, const char side_, const int userId_, const int orderId_);
    
    /* non-copyable */
    Order( const Order& ) = delete;
    
    /* non-assignable */
    Order& operator=(const Order&) = delete;
    
    /* Check if this order is executable with other order */
    bool isTradeableWith( const OrderPtr& order ) const;

    /* Get order time priority */
    Time getTime( void ) const;
    
    /* Get order price */
    int getPrice( void ) const;
    
    /* Get order quantity */
    int getQty( void ) const;
    
    /* Get order unique id */
    UniqueId getUniqueId( void ) const;
    
    /* Get order user id */
    int getUserId( void ) const;
    
    /* Get order id */
    int getOrderId( void ) const;
    
    /* get side of this order */
    char getSide() const;
    
    /* get other side of this order */
    char getOtherSide() const;

private:
    const Time time;
    const int price;
    
    /* quantity changes during (partial) execution, hence not const */
    int qty;
    
    const char side;
    const int userId;
    const int orderId;
};
