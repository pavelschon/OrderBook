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
int OrderBook::trade(Response& response, OrderContainer& container, Order& order)
{
    auto& idx = container.template get<Tag::PriceTime>();
    auto it   = idx.begin();

    int tradedQty = 0;
    
    while(it != idx.end() && order.isTradeableWith(*it) && order.qty > 0)
    {
        const auto matchQty = std::min(order.qty, it->qty);

        order.qty -= matchQty;
        
        container.modify(it, [matchQty](Order& other) {
            other.qty -= matchQty;
        });
        
        tradedQty += matchQty;
        
        /* now prepare the trade message */
        switch(order.side)
        {
            case Side::Buy:
                response.trade(order, *it, it->price, matchQty);
                break;
                
            case Side::Sell:
                response.trade(*it, order, it->price, matchQty);
                break;
        }

        /* other order fully matched, remove it */
        if( it->qty < 1 )
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
template int OrderBook::trade(Response&, BidOrderContainer&, Order&);
template int OrderBook::trade(Response&, AskOrderContainer&, Order&);

