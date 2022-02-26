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

