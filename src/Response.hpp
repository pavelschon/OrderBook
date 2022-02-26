/**
 * @brief Response definition
 *
 */

#pragma once

#include "OrderContainer.hpp"

#include <boost/python/list.hpp>


class Response
{
public:
    using Type = boost::python::list;
    
    Response() = default;
    
    /* Get payload of this response */
    Type get() const;
    
    /* Create acknowledge message */
    void acknowledge(const int userId, const int orderId);
    
    /* Create top-of-book message */
    template<class OrderContainer>
    void topOfBook(const OrderContainer& container, const char side);
    
private:
    Type payload;
};

