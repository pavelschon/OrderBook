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
int OrderBook::trade(Response& response, OrderContainer& container, const OrderPtr& order)
{
    auto& idx = container.template get<Tag::PriceTime>();
    auto it   = idx.begin();

    int tradedQty = 0;
    
    while(it != idx.end() && order->isTradeableWith(*it) && order->qty > 0)
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
                response.trade(order, otherOrder, otherOrder->price, matchQty);
                break;
                
            case Side::Sell:
                response.trade(otherOrder, order, otherOrder->price, matchQty);
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
template int OrderBook::trade(Response&, BidOrderContainer&, const OrderPtr&);
template int OrderBook::trade(Response&, AskOrderContainer&, const OrderPtr&);

