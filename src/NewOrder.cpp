/**
 * @brief OrderBook new order implementation
 *
 */

#include "OrderBook.hpp"
#include "Response.hpp"


/**
 * @brief Create new order
 * 
 */
PyList OrderBook::newOrder(const unsigned int ip, const unsigned short port, const int orderId, const char side, const int price, const int qty)
{
    Response response;
    
    switch(side)
    {
        case Side::Buy:
            newOrderImpl(bidOrders, askOrders, response, ip, port, orderId, side, price, qty);
            break;
            
        case Side::Sell:
            newOrderImpl(askOrders, bidOrders, response, ip, port, orderId, side, price, qty);
            break;
    }
    
    return response.get();
}


/**
 * @brief Create new order
 *
 */
template<class OrderContainer, class OtherContainer>
void OrderBook::newOrderImpl(OrderContainer& container, OtherContainer& otherContainer, Response& response,
                             const unsigned int ip, const unsigned short port, const int orderId, const char side, const int price, const int qty)
{
    /* calculate top-of-book */
    const auto& prevTopOfBook = OrderBook::getTopOfBook(container);
    const auto& prevTopOfBookOther = OrderBook::getTopOfBook(otherContainer);
    
    /* create new order */
    Order order{ ip, port, orderId, side, price, qty };
    
    /* create acknowledge message */
    response.acknowledge(order);
    
    /* full or partial execution */
    const auto tradedQty = trade(response, otherContainer, order);
    
    /* if has resting qty... */
    if(order.qty > 0)
    {
        /* ...insert passive order into orderbook */
        if(container.insert(order).second)
        {
            /* if inserted, then maybe create top-of-book message */
            response.topOfBook(container, prevTopOfBook, order.side);
        }
        /*
        else
        {
            // order with this userId&orderId exists in orderbook
            // no action
        }
        */
    }
    
    if(tradedQty > 0)
    {
        response.topOfBook(otherContainer, prevTopOfBookOther, order.getOtherSide());
    }
}


/* explicitly instantiate template functions */
template void OrderBook::newOrderImpl(BidOrderContainer&, AskOrderContainer&, Response&,
                                      const unsigned int, const unsigned short, const int, const char, const int, const int);

template void OrderBook::newOrderImpl(AskOrderContainer&, BidOrderContainer&, Response&,
                                      const unsigned int, const unsigned short, const int, const char, const int, const int);

