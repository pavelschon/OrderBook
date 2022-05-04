/**
 * @brief OrderBook cancel order implementation
 *
 */

#include "OrderBook.hpp"
#include "Response.hpp"


/**
 * @brief Cancel existing order
 * 
 */
PyList OrderBook::cancelAll(const unsigned int ip, const unsigned short port)
{
    Response response;
    
    cancelAllImpl(response, bidOrders, ip, port, 'B');
    cancelAllImpl(response, askOrders, ip, port, 'S');
    
    return response.get();
}


/**
 * @brief Cancel existing order
 * 
 */
template<class OrderContainer>
void OrderBook::cancelAllImpl(Response& response, OrderContainer& container, const unsigned int ip, const unsigned short port, const char side)
{
    auto& idx = container.template get<Tag::UserId>();
    const auto& userId = std::make_tuple(ip, port);
    
    auto it = idx.lower_bound(userId);
    const auto& end = idx.upper_bound(userId);
    
    const auto& prevTopOfBook = OrderBook::getTopOfBook(container);
    
    while(it != end)
    {
        const auto order = *it; /* no reference, but copy! */
    
        idx.erase(it++);
        
        /* create acknowledge message */
        response.acknowledge(order);
    }
    
    /* create top-of-book message */
    response.topOfBook(container, prevTopOfBook, side);
}


/* explicitly instantiate template functions */
template void OrderBook::cancelAllImpl(Response&, BidOrderContainer&, const unsigned int, const unsigned short, const char);
template void OrderBook::cancelAllImpl(Response&, AskOrderContainer&, const unsigned int, const unsigned short, const char);

