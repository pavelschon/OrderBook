/**
 * @brief Response implementation
 *
 */

#include "OrderBook.hpp"
#include "Response.hpp"


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
void Response::acknowledge(const int userId, const int orderId)
{
    PyList ackMessage;
    
    ackMessage.append('A');
    ackMessage.append(userId);
    ackMessage.append(orderId);
    
    payload.append(ackMessage);
}



/**
 * @brief Create trade message
 *
 */
void Response::trade(const int userIdBuy, const int orderIdBuy,
                     const int userIdSell, const int orderIdSell,
                     const int price, const int quantity )
{
    PyList tradeMessage;
    
    tradeMessage.append('T');
    tradeMessage.append(userIdBuy);
    tradeMessage.append(orderIdBuy);
    tradeMessage.append(userIdSell);
    tradeMessage.append(orderIdSell);
    tradeMessage.append(price);
    tradeMessage.append(quantity);
    
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



