/**
 * @brief Response implementation
 *
 */

#include "Response.hpp"


namespace py = boost::python;



/**
 * @brief Get payload of this response
 * 
 */
Response::Type Response::get() const
{
    return payload;
}


/**
 * @brief Create acknowledge message
 * 
 */
void Response::acknowledge(const int userId, const int orderId)
{
    py::list ackMessage;
    
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
    py::list tradeMessage;
    
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
 * @brief Compare top-of-book objects
 *
 */
bool Response::TopOfBook::operator!=( const TopOfBook& other) const
{
    return price != other.price || qty != other.qty;
}

