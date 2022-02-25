/**
 * @brief Order definition
 *
 */

#include <chrono>
#include <memory>
#include <tuple>


class Order
{
    friend class OrderBook;
    
public:
    using UniqueId = std::tuple<int, int>;
    using Time = std::chrono::time_point<std::chrono::high_resolution_clock>;
    using SharedPtr = std::shared_ptr<Order>;
    
    /* ctor */
    Order(const int price_, const int qty_, const char side_, const int userId_, const int orderId_);
    
    /* non-copyable */
    Order( const Order& ) = delete;
    
    /* non-assignable */
    Order& operator=(const Order&) = delete;
    
    /* Check if this order is executable with other order */
    bool isExecutableWith( const Order::SharedPtr& order ) const;

    Time getTime( void ) const;
    int getPrice( void ) const;
    int getQty( void ) const;
    UniqueId getUniqueId( void ) const;
    int getUserId( void ) const;

private:
    const Time time;
    const int price;
    int qty;
    const char side;
    const int userId;
    const int orderId;
};
