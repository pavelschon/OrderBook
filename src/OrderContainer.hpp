/**
 * @brief Order container
 *
 */

#pragma once

#include "Order.hpp"

#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/tag.hpp>


namespace comparator
{

/* higher price first - bid orders */
using HigherPrice = std::greater<const int>;

/* lesser price first - ask orders */
using LesserPrice = std::less<const int>;

/* lesser timestamp first */
using LesserTime = std::less<const Order::Time>;

using HigherPriceLesserTime = boost::multi_index::composite_key_compare<HigherPrice, LesserTime>;   /* bid orders */
using LesserPriceLesserTime = boost::multi_index::composite_key_compare<LesserPrice,  LesserTime>;  /* ask orders */

}

namespace extractor
{

using Price = BOOST_MULTI_INDEX_CONST_MEM_FUN( Order, int, Order::getPrice );
using Time = BOOST_MULTI_INDEX_CONST_MEM_FUN( Order, Order::Time,  Order::getTime );
using UserId = BOOST_MULTI_INDEX_CONST_MEM_FUN( Order, int, Order::getUserId );
using UniqueId = BOOST_MULTI_INDEX_CONST_MEM_FUN( Order, Order::UniqueId, Order::getUniqueId );
using PriceTime = boost::multi_index::composite_key<Order, Price, Time>;

}


namespace tag
{

struct Price {};
struct PriceTime {};
struct UserId {};
struct UniqueId {};

}


template<typename ComparePriceTime>
using ComparePrice = typename boost::tuples::element<0,
    typename ComparePriceTime::key_comp_tuple>::type;


template<typename ComparePriceTime>
using OrderContainerImpl = boost::multi_index::multi_index_container<
    Order::SharedPtr, boost::multi_index::indexed_by<

        /* iterate orders in a match event */
        boost::multi_index::ordered_unique<
            boost::multi_index::tag<tag::PriceTime>,
                extractor::PriceTime, ComparePriceTime>,

        /* iterate orders by price levels */
        boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<tag::Price>,
                extractor::Price, ComparePrice<ComparePriceTime> >,

        /* find orders of the user */
        boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<tag::UserId>,
                extractor::UserId >,

                
        /* find order by unique id */
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<tag::UniqueId>,
                extractor::UniqueId > >
>;

using BidOrderContainer = OrderContainerImpl<comparator::HigherPriceLesserTime>; /* container type for bid orders */
using AskOrderContainer = OrderContainerImpl<comparator::LesserPriceLesserTime>;  /* container type for ask orders */


