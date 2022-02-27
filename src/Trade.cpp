/**
 * @brief OrderBook trade implementation
 *
 */

#include "OrderBook.hpp"
#include "Response.hpp"


/**
 * @brief Handle order execution (trade)
 * 
 */
template<class OrderContainer>
int OrderBook::trade(Response& response, OrderContainer& container, const Order::SharedPtr& order)
{
    auto& idx = container.template get<Tag::PriceTime>();
    auto it   = idx.begin();

    int tradedQty = 0;
    
    while(it != idx.end() && order->isExecutableWith(*it) && order->qty > 0)
    {
        const auto& otherOrder = *it;
        const auto matchQty = std::min(order->qty, otherOrder->qty);

        order->qty -= matchQty;
        otherOrder->qty -= matchQty;
        tradedQty += matchQty;
        
        /* now prepare the trade message */
        switch(order->side)
        {
            case Side::Buy:
                response.trade(
                    order->userId, order->orderId,
                    otherOrder->userId, otherOrder->orderId,
                    otherOrder->price, matchQty);
                
                break;
                
            case Side::Sell:
                response.trade(
                    otherOrder->userId, otherOrder->orderId,
                    order->userId, order->orderId,
                    otherOrder->price, matchQty);
                
                break;
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


/* explicitly instantiate template functions */
template int OrderBook::trade(Response&, BidOrderContainer&, const Order::SharedPtr&);
template int OrderBook::trade(Response&, AskOrderContainer&, const Order::SharedPtr&);

