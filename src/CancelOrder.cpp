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
PyList OrderBook::cancelOrder(const int userId, const int orderId)
{
    Response response;
    
    cancelOrderImpl(response, bidOrders, userId, orderId);
    cancelOrderImpl(response, askOrders, userId, orderId);
    
    return response.get();
}


/**
 * @brief Cancel existing order
 * 
 */
template<class OrderContainer>
void OrderBook::cancelOrderImpl(Response& response, OrderContainer& container, const int userId, const int orderId )
{
    auto& idx = container.template get<Tag::UniqueId>();
    const auto& it = idx.find(std::make_tuple(userId, orderId));

    if(it != idx.end())
    {
        
        idx.erase(it);
    }
}


/* explicitly instantiate template functions */
template void OrderBook::cancelOrderImpl(Response&, BidOrderContainer&, const int, const int );
template void OrderBook::cancelOrderImpl(Response&, AskOrderContainer&, const int, const int );

