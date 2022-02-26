/**
 * @brief OrderBook implementation
 *
 */

#include "OrderBook.hpp"


/**
 * @brief Create new order
 * 
 */
Response::Type OrderBook::newOrder(const int userId, const int price, const int qty, const char side, const int orderId)
{
    Response response;
    
    switch(side)
    {
        case Order::Side::Buy:
            newOrderImpl(bidOrders, askOrders, response, price, qty, side, userId, orderId);
            break;
            
        case Order::Side::Sell:
            newOrderImpl(askOrders, bidOrders, response, price, qty, side, userId, orderId);
            break;
    }
    
    return response.get();
}


/**
 * @brief Cancel existing order
 * 
 */
void OrderBook::cancelOrder(const int userId, const int orderId)
{
    cancelOrderImpl( bidOrders, userId, orderId );
    cancelOrderImpl( askOrders, userId, orderId );
}


/**
 * @brief Create new order
 *
 */
template<class OrderContainer, class OtherContainer>
void OrderBook::newOrderImpl(OrderContainer& container, OtherContainer& otherContainer, Response& response,
                             const int price, const int qty, const char side, const int userId, const int orderId)
{
    const auto order = std::make_shared<Order>(price, qty, side, userId, orderId);
    const auto& idx = container.template get<tag::Price>();
    
    const auto& prevBestPrice = OrderBook::getBestPrice(container);
    const auto& prevBestPriceOther = OrderBook::getBestPrice(otherContainer);
    
    response.acknowledge(userId, orderId);
    
    /* full or partial execution */
    const int tradedQty = trade(response, otherContainer, order);
    
    /* if has resting qty... */
    if(order->qty > 0)
    {
        /* ...insert into orderbook */
        container.insert(order);
        
        const auto& bestPrice = OrderBook::getBestPrice(container);
        if(bestPrice && ((!prevBestPrice) || prevBestPrice.value() != bestPrice.value()))
        {
            response.topOfBook(container, order->side);
        }
    }
    
    if(tradedQty > 0)
    {
        const auto& bestPrice = OrderBook::getBestPrice(otherContainer);
        if(bestPrice && ((!prevBestPriceOther) || prevBestPriceOther.value() != bestPrice.value()))
        {
            response.topOfBook(otherContainer, order->getOtherSide());
        }
    }
}


/**
 * @brief Cancel existing order
 * 
 */
template<class OrderContainer>
void OrderBook::cancelOrderImpl(OrderContainer& container, const int userId, const int orderId )
{
    auto& idx = container.template get<tag::UniqueId>();
    const auto it = idx.find(std::make_tuple(userId, orderId));

    if(it != idx.end())
    {
        const auto order = *it;

        idx.erase( it );
    }
}


/**
 * @brief Handle order execution (trade)
 * 
 */
template<class OrderContainer>
int OrderBook::trade(Response& response, OrderContainer& container, const Order::SharedPtr& order)
{
    auto &idx = container.template get<tag::PriceTime>();
    auto it   = idx.begin();

    int tradedQty = 0;
    
    while(it != idx.end() && order->isExecutableWith(*it) && order->qty > 0)
    {
        const auto& otherOrder = *it;
        //const auto matchPrice = otherOrder->price;
        const auto matchQty = std::min( order->qty, otherOrder->qty );

        order->qty -= matchQty;
        otherOrder->qty -= matchQty;
        tradedQty += matchQty;
        
        /* now prepare the trade message */
        switch(order->side)
        {
            case Order::Side::Buy:
                response.trade(order->userId, order->orderId,
                               otherOrder->userId, otherOrder->orderId,
                               otherOrder->price, matchQty);
                break;
                
            case Order::Side::Sell:
                response.trade(otherOrder->userId, otherOrder->orderId,
                               order->userId, order->orderId,
                               otherOrder->price, matchQty);
                break;
        }

        // other order fully matched, remove it
        if( otherOrder->qty < 1 )
        {
            idx.erase( it++ );
        }
        else
        {
            ++it;
        }
    }
    
    return tradedQty;
}


/**
 * @brief Get the best price from the order container
 * 
 */
template<class OrderContainer>
boost::optional<int> OrderBook::getBestPrice(const OrderContainer& container)
{
    if(container.size())
    {
        const auto& idx = container.template get<tag::Price>();
        
        return (*idx.begin())->price;
    }
    else
    {
        return boost::none;
    }
}


/**
 * @brief Create top-of-book message
 *
 */
template<class OrderContainer>
void Response::topOfBook(const OrderContainer& container, char side)
{
    Type topOfBookMessage;
    
    topOfBookMessage.append('B');
    topOfBookMessage.append(side);
    
    const auto& bestPrice = OrderBook::getBestPrice(container);
    
    if(bestPrice)
    {
        const auto& idx = container.template get<tag::Price>();
        
        /* now get the orders-range in this price level */
        const auto end = idx.upper_bound(bestPrice.value());
        auto it = idx.lower_bound(bestPrice.value());

        int qty = 0;

        /* iterate over orders in the range */
        for(; it != end; ++it )
        {
            /* sum-up the quantity */
            qty += (*it)->getQty();
        }
        
        topOfBookMessage.append(bestPrice.value());
        topOfBookMessage.append(qty);
    }
    else
    {
        topOfBookMessage.append('-');
        topOfBookMessage.append('-');
    }
    
    payload.append(topOfBookMessage);
}



/**
 * @brief Flush orderbook
 * 
 */
void OrderBook::flush()
{

}


