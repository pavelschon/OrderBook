/**
 * @brief Order definition
 *
 */

#include <chrono>
#include <memory>
#include <tuple>


class Order
{
    /* access to private members is allowed */
    friend class OrderBook;
    
public:
    /* helper types */
    using UniqueId = std::tuple<int, int>;
    using Time = std::chrono::time_point<std::chrono::high_resolution_clock>;
    using SharedPtr = std::shared_ptr<Order>;
    
    /* side helper */
    struct Side
    {
        static const char Buy = 'B';
        static const char Sell = 'S';
    };
    
    /* ctor */
    Order(const int price_, const int qty_, const char side_, const int userId_, const int orderId_);
    
    /* non-copyable */
    Order( const Order& ) = delete;
    
    /* non-assignable */
    Order& operator=(const Order&) = delete;
    
    /* Check if this order is executable with other order */
    bool isExecutableWith( const Order::SharedPtr& order ) const;

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

private:
    const Time time;
    const int price;
    
    /* quantity changes during (partial) execution, hence not const */
    int qty;
    
    const char side;
    const int userId;
    const int orderId;
};
