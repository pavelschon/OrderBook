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
PyList OrderBook::cancelOrder(const unsigned int ip, const unsigned short port, const int orderId)
{
    Response response;
    
    cancelOrderImpl(response, bidOrders, ip, port, orderId);
    cancelOrderImpl(response, askOrders, ip, port, orderId);
    
    return response.get();
}


/**
 * @brief Cancel existing order
 * 
 */
template<class OrderContainer>
void OrderBook::cancelOrderImpl(Response& response, OrderContainer& container, const unsigned int ip, const unsigned short port, const int orderId)
{
    auto& idx = container.template get<Tag::UniqueId>();
    const auto& it = idx.find(std::make_tuple(ip, port, orderId));

    if(it != idx.end())
    {
        /* calculate top-of-book */
        const auto& prevTopOfBook = OrderBook::getTopOfBook(container);
        const auto order = *it; /* no reference, but copy! */
    
        idx.erase(it);
        
        /* create acknowledge message */
        response.acknowledge(order);
        
        /* create top-of-book message */
        response.topOfBook(container, prevTopOfBook, order.side);
    }
}


/* explicitly instantiate template functions */
template void OrderBook::cancelOrderImpl(Response&, BidOrderContainer&, const unsigned int, const unsigned short, const int);
template void OrderBook::cancelOrderImpl(Response&, AskOrderContainer&, const unsigned int, const unsigned short, const int);

