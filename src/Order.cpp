/**
 * @brief Order implementation
 * 
 */

#include "Order.hpp"


/**
 * @brief Constructor
 * 
 */
Order::Order(const int price_, const int qty_, const char side_, const int userId_, const int orderId_):
    time{ std::chrono::high_resolution_clock::now() },
    price{ price_ },
    qty{ qty_ },
    side { side_ },
    userId{ userId_ },
    orderId{ orderId_ }
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
    return std::make_tuple(userId, orderId);
}


/**
 * @brief Get order user id
 * 
 */
int Order::getUserId() const
{
    return userId;
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
bool Order::isExecutableWith( const Order::SharedPtr& other ) const
{
    if( ! price)
    {
        // I'm market order
        return true;
    }
    else if(side == Side::Buy && other->side == Side::Sell)
    {
        return price >= other->price;
    }
    else if(side == Side::Sell && other->side == Side::Buy)
    {
        return price <= other->price;
    }
    else
    {
        return false;
    }
}


