/**
 * @brief Response definition
 *
 */

#pragma once

#include "OrderBookFwd.hpp"

#include <boost/python/list.hpp>
#include <boost/optional.hpp>


class Response
{
public:
    Response() = default;
    
    /* Get payload of this response */
    PyList get() const;
    
    /* Create acknowledge message */
    void acknowledge(const OrderConstPtr& order);
    
    /* Create acknowledge message */
    void cancel(const int userId, const int orderId);
    
    
    /* Create trade message */
    void trade(const OrderConstPtr& bidOrder,
               const OrderConstPtr& askOrder,
               const int matchPrice, const int matchQty);
    
    /* Create top-of-book message */
    template<class OrderContainer>
    void topOfBook(const OrderContainer& container,
                   const TopOfBook::Optional& prevTopOfBook, char side);
    
private:
    PyList payload;
};

