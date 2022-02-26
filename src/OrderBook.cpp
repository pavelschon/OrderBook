/**
 * @brief OrderBook implementation
 *
 */

#include "OrderBook.hpp"
#include <iostream>


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
    
    /* full or partial execution */
    execution(otherContainer, order);
    
    /* if has resting qty... */
    if(order->qty > 0)
    {
        /* ...insert into orderbook */
        const auto& result = container.insert(order);

        /* if inserted */
        if(result.second)
        {
            if(order->qty > 0)
            {
                response.acknowledge(userId, orderId);
                
                //aggregatePriceLevel( orders, order->price, order->side );
                
                //logger.debug( format::f2::logTraderAddedOrder, trader->toString(), order->toString() );
            }
        }
        else
        {
            //logger.debug( format::f2::logOrderAlreadyExist, trader->toString(), order->orderId );

            //trader->notifyError( format::f0::orderAlreadyExist.str() );
        }
    }
    
    response.topOfBook(container, side);
}


/**
 * @brief Cancel existing order
 * 
 */
template<class OrderContainer>
void OrderBook::cancelOrderImpl( OrderContainer& container, const int userId, const int orderId )
{
    auto& idx = container.template get<tag::UniqueId>();
    const auto it = idx.find(std::make_tuple(userId, orderId));

    if(it != idx.end())
    {
        const auto order = *it;

        idx.erase( it );

        //trader->notifyCancelOrderSuccess( order->orderId, order->quantity );

        //logger.debug( format::f2::logTraderCanceledOne, trader->toString(), order->toString() );

        //aggregatePriceLevel<OrderContainer>( orders, order->price, order->side );
    }
}


/**
 * @brief Handle order execution (trade)
 * 
 */
template<class OrderContainer>
void OrderBook::execution( OrderContainer& container, const Order::SharedPtr& order )
{
    //typename OrderContainer::element_type::price_set priceLevels;

    auto &idx = container.template get<tag::PriceTime>();
    auto it   = idx.begin();

    int totalMatchQuantity = 0;
    
    while( it != idx.end() && order->isExecutableWith( *it ) && order->qty > 0 )
    {
        const auto& otherOrder = *it;
        //const auto matchPrice = otherOrder->price;
        const auto matchQty = std::min( order->qty, otherOrder->qty );

        order->qty -= matchQty;
        otherOrder->qty -= matchQty;
        totalMatchQuantity += matchQty;

        //priceLevels.insert( matchPrice );

        // other order fully matched, remove it
        if( otherOrder->qty < 1 )
        {
            idx.erase( it++ );
        }
        else
        {
            ++it;
        }

//        notifyExecution( order, otherOrder, matchQty );
    }

    if( totalMatchQuantity > 0 )
    {
        std::cout << "trade: " << totalMatchQuantity << "\n";
    //    aggregateSetPriceLevels( orders, priceLevels, side::opposite( order->side ) );
    }
}



/**
 * @brief Flush orderbook
 * 
 */
void OrderBook::flush()
{
    if(bidOrders.size())
    {
        std::cout << "Bid: " << (*bidOrders.get<tag::Price>().begin())->getPrice() << "\n";
    }
    
    if(askOrders.size())
    {
        std::cout << "Ask: " << (*askOrders.get<tag::Price>().begin())->getPrice() << "\n";
    }
}


/**
 * @brief Create top-of-book message
 *
 */
template<class OrderContainer>
void Response::topOfBook(const OrderContainer& container, const char side)
{
    Type topOfBookMessage;
    
    topOfBookMessage.append('B');
    topOfBookMessage.append(side);
    
    if(container.size())
    {
        const auto& idx = container.template get<tag::Price>();
        
        /* best price is the price of the first order */
        const auto bestPrice = (*idx.begin())->getPrice();
        
        /* now get the range of order of this price */
        const auto end = idx.upper_bound(bestPrice);
        auto it = idx.lower_bound(bestPrice);

        int qty = 0;

        /* iterate over orders in the price-level range */
        for(; it != end; ++it )
        {
            /* sum-up the quantity */
            qty += (*it)->getQty();
        }
        
        topOfBookMessage.append(bestPrice);
        topOfBookMessage.append(qty);
    }
    else
    {
        topOfBookMessage.append('-');
        topOfBookMessage.append('-');
    }
    
    payload.append(topOfBookMessage);
}


