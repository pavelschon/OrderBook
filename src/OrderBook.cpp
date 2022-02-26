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
    const auto& order = std::make_shared<Order>(price, qty, side, userId, orderId);
    
    /* calculate top-of-book */
    const auto& prevTopOfBook = OrderBook::getTopOfBook(container);
    const auto& prevTopOfBookOther = OrderBook::getTopOfBook(otherContainer);
    
    /* create acknowledge message */
    response.acknowledge(userId, orderId);
    
    /* full or partial execution */
    const int tradedQty = trade(response, otherContainer, order);
    
    /* if has resting qty... */
    if(order->qty > 0)
    {
        /* ...insert into orderbook */
        if(container.insert(order).second)
        {
            /* if inserted, then maybe create top-of-book message */
            response.topOfBook(container, prevTopOfBook, order->side);
        }
    }
    
    if(tradedQty > 0)
    {
        response.topOfBook(otherContainer, prevTopOfBookOther, order->getOtherSide());
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
    const auto& it = idx.find(std::make_tuple(userId, orderId));

    if(it != idx.end())
    {
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
    auto& idx = container.template get<tag::PriceTime>();
    auto it   = idx.begin();

    int tradedQty = 0;
    
    while(it != idx.end() && order->isExecutableWith(*it) && order->qty > 0)
    {
        const auto& otherOrder = *it;
        const auto matchQty = std::min( order->qty, otherOrder->qty );

        order->qty -= matchQty;
        otherOrder->qty -= matchQty;
        tradedQty += matchQty;
        
        /* now prepare the trade message */
        if(order->price > 0)
        {
            response.trade(order->userId, order->orderId,
                           otherOrder->userId, otherOrder->orderId,
                           otherOrder->price, matchQty);
        }
        else
        {
            switch(order->side)
            {
                case Order::Side::Buy:
                    response.trade(order->userId, order->orderId,
                        otherOrder->userId, otherOrder->orderId,
                        otherOrder->price, matchQty);
                    
                    break;
                    
                case Order::Side::Sell:
                    response.trade(
                        otherOrder->userId, otherOrder->orderId,
                        order->userId, order->orderId,
                        otherOrder->price, matchQty);
                    
                    break;
            }
        }

        /* other order fully matched, remove it */
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
boost::optional<Response::TopOfBook> OrderBook::getTopOfBook(const OrderContainer& container)
{
    if(container.size())
    {
        const auto& idx = container.template get<tag::Price>();
        const auto bestPrice = (*idx.begin())->price;
        const auto& end = idx.upper_bound(bestPrice);
        auto it = idx.lower_bound(bestPrice);

        int qty = 0;

        /* iterate over orders in the range */
        for(; it != end; ++it )
        {
            /* sum-up the quantity */
            qty += (*it)->getQty();
        }
        
        return Response::TopOfBook{bestPrice, qty};
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
void Response::topOfBook(const OrderContainer& container, const Response::TopOfBook::Optional& prevTopOfBook, char side)
{
    const auto& topOfBook = OrderBook::getTopOfBook(container);
    
    Type topOfBookMessage;
    
    topOfBookMessage.append('B');
    topOfBookMessage.append(side);
    
    if(!topOfBook)
    {
        topOfBookMessage.append('-');
        topOfBookMessage.append('-');
        
        payload.append(topOfBookMessage);
    }
    else if((!prevTopOfBook) || prevTopOfBook.value() != topOfBook.value())
    {
        topOfBookMessage.append(topOfBook.value().price);
        topOfBookMessage.append(topOfBook.value().qty);
        
        payload.append(topOfBookMessage);
    }
}



/**
 * @brief Flush orderbook
 * 
 */
void OrderBook::flush()
{

}


