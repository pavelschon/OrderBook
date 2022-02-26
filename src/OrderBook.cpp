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
void OrderBook::newOrder(const int userId, const int price, const int qty, const char side, const int orderId)
{
    switch(side)
    {
        case Order::Side::Buy:
            newOrderImpl(bidOrders, askOrders, price, qty, side, userId, orderId);
            break;
            
        case Order::Side::Sell:
            newOrderImpl(askOrders, bidOrders, price, qty, side, userId, orderId);
            break;
    }
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
void OrderBook::newOrderImpl(OrderContainer& container, OtherContainer& otherContainer,
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
}


/**
 * @brief Cancel existing order
 * 
 */
template<typename OrderContainer>
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
template<typename OrderContainer>
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

