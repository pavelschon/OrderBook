/**
 * @brief Order implementation
 * 
 */

#include "Order.hpp"


/**
 * @brief Constructor
 * 
 */
Order::Order(const unsigned int ip_, const unsigned short port_, const int orderId_, const char side_, const int price_, const int qty_):
    time{ std::chrono::high_resolution_clock::now() },
    ip{ ip_ },
    port{ port_ },
    orderId{ orderId_ },
    side { side_ },
    price{ price_ },
    qty{ qty_ }
{

}


/**
 * @brief Get order time priority
 * 
 */
Time Order::getTime() const
{
    return time;
}


/**
 * @brief Get order price
 * 
 */
int Order::getPrice() const
{
    return price;
}


/**
 * @brief Get order quantity
 * 
 */
int Order::getQty() const
{
    return qty;
}


/**
 * @brief Get order unique id
 * 
 */
UniqueId Order::getUniqueId() const
{
    return std::make_tuple(ip, port, orderId);
}


/**
 * @brief Get order user id
 * 
 */
UserId Order::getUserId() const
{
    return std::make_tuple(ip, port);
}


/**
 * @brief Get order user id
 * 
 */
unsigned int Order::getIp() const
{
    return ip;
}


/**
 * @brief Get order port
 * 
 */
unsigned short Order::getPort() const
{
    return port;
}


/**
 * @brief Get order user id
 * 
 */
int Order::getOrderId() const
{
    return orderId;
}


/**
 * @brief get side of this order
 *
 */
char Order::getSide() const
{
    return side;
}


/**
 * @brief get other side of this order
 *
 */
char Order::getOtherSide() const
{
    switch(side)
    {
        case Side::Buy:
            return Side::Sell;
            
        case Side::Sell:
            return Side::Buy;
            
        default:
            throw std::exception();
    }
}


/**
 * @brief Check if this order is executable with other order
 * 
 */
bool Order::isTradeableWith( const Order& other ) const
{
    if( ! price)
    {
        // I'm market order
        return true;
    }
    else if(side == Side::Buy && other.side == Side::Sell)
    {
        return price >= other.price;
    }
    else if(side == Side::Sell && other.side == Side::Buy)
    {
        return price <= other.price;
    }
    else
    {
        return false;
    }
}


