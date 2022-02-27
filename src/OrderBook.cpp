/**
 * @brief OrderBook implementation
 *
 */

#include "OrderBook.hpp"
#include "Response.hpp"


/**
 * @brief Get the best price from the order container
 * 
 */
template<class OrderContainer>
TopOfBook::Optional OrderBook::getTopOfBook(const OrderContainer& container)
{
    if(container.size())
    {
        const auto& idx = container.template get<Tag::Price>();
        const auto bestPrice = (*idx.begin())->price;
        const auto& end = idx.upper_bound(bestPrice);
        auto it = idx.lower_bound(bestPrice);

        int qty = 0;

        /* iterate over orders in the range */
        for(; it != end; ++it )
        {
            /* sum-up the quantity */
            qty += (*it)->getQty();
        }
        
        return TopOfBook{ bestPrice, qty };
    }
    else
    {
        return boost::none;
    }
}


/* explicitly instantiate template functions */
template TopOfBook::Optional OrderBook::getTopOfBook(const BidOrderContainer&);
template TopOfBook::Optional OrderBook::getTopOfBook(const AskOrderContainer&);


/**
 * @brief Flush orderbook
 * 
 */
void OrderBook::flush()
{
    bidOrders.clear();
    askOrders.clear();
}

