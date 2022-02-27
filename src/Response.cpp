/**
 * @brief Response implementation
 *
 */

#include "Response.hpp"
#include "OrderBook.hpp"
#include "OrderContainer.hpp"
#include "Order.hpp"


/**
 * @brief Get payload of this response
 * 
 */
PyList Response::get() const
{
    return payload;
}


/**
 * @brief Create acknowledge message
 * 
 */
void Response::acknowledge(const OrderConstPtr& order)
{
    PyList ackMessage;
    
    ackMessage.append('A');
    ackMessage.append(order->getUserId());
    ackMessage.append(order->getOrderId());
    
    payload.append(ackMessage);
}



/**
 * @brief Create trade message
 *
 */
void Response::trade(const OrderConstPtr& bidOrder, const OrderConstPtr& askOrder, const int matchPrice, const int matchQty)
{
    PyList tradeMessage;
    
    tradeMessage.append('T');
    tradeMessage.append(bidOrder->getUserId());
    tradeMessage.append(bidOrder->getOrderId());
    tradeMessage.append(askOrder->getUserId());
    tradeMessage.append(askOrder->getOrderId());
    tradeMessage.append(matchPrice);
    tradeMessage.append(matchQty);
    
    payload.append(tradeMessage);
}


/**
 * @brief Create top-of-book message
 *
 */
template<class OrderContainer>
void Response::topOfBook(const OrderContainer& container, const TopOfBook::Optional& prevTopOfBook, char side)
{
    const auto& topOfBook = OrderBook::getTopOfBook(container);
    
    PyList topOfBookMessage;
    
    topOfBookMessage.append('B');
    topOfBookMessage.append(side);
    
    if(!topOfBook)
    {
        topOfBookMessage.append('-');
        topOfBookMessage.append('-');
        
        payload.append(topOfBookMessage);
    }
    else if((!prevTopOfBook) || (prevTopOfBook.value() != topOfBook.value()))
    {
        topOfBookMessage.append(topOfBook.value().price);
        topOfBookMessage.append(topOfBook.value().qty);
        
        payload.append(topOfBookMessage);
    }
}


/* explicitly instantiate template functions */
template void Response::topOfBook(const BidOrderContainer&, const TopOfBook::Optional&, char);
template void Response::topOfBook(const AskOrderContainer&, const TopOfBook::Optional&, char);



