/**
 * @brief Response definition
 *
 */

#pragma once

#include "OrderBookFwd.hpp"
#include "OrderContainer.hpp"

#include <boost/python/list.hpp>
#include <boost/optional.hpp>


class Response
{
public:
    Response() = default;
    
    /* Get payload of this response */
    PyList get() const;
    
    /* Create acknowledge message */
    void acknowledge(const int userId, const int orderId);
    
    
    /* Create trade message */
    void trade(const int userIdBuy, const int orderIdBuy,
               const int userIdSell, const int orderIdSell,
               const int price, const int quantity );
    
    /* Create top-of-book message */
    template<class OrderContainer>
    void topOfBook(const OrderContainer& container,
                   const TopOfBook::Optional& prevTopOfBook, char side);
    
private:
    PyList payload;
};

