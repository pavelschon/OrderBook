/**
 * @brief OrderBook definition
 *
 */

#include "OrderContainer.hpp"

#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/tag.hpp>


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
    void newOrder(const int userId, const int price, const int qty, const char side, const int orderId);
    
    /* Cancel existing order */
    void cancelOrder(const int userId, const int userOrderId);
    
    /* Flush orderbook */
    void flush();
    
private:
    /* Create new order */
    template<class OrderContainer, class OtherContainer>
    static void newOrderImpl(OrderContainer& container, OtherContainer& otherContainer,
                             const int price, const int qty, const char side, const int userId, const int orderId);

    /* Cancel existing order */
    template<typename OrderContainer>
    static void cancelOrderImpl( OrderContainer& container, const int userId, const int orderId );
    
    /* Handle order execution (trade) */
    template<typename OrderContainer>
    static void execution( OrderContainer& container, const Order::SharedPtr& order );
        
    BidOrderContainer bidOrders;
    AskOrderContainer askOrders;
};

