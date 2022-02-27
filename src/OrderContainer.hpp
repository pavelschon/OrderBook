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


namespace Comparator
{

/* higher price first - bid orders */
using HigherPrice = std::greater<const int>;

/* lesser price first - ask orders */
using LesserPrice = std::less<const int>;

/* lesser timestamp first */
using LesserTime = std::less<const Time>;

using HigherPriceLesserTime = boost::multi_index::composite_key_compare<HigherPrice, LesserTime>;   /* bid orders */
using LesserPriceLesserTime = boost::multi_index::composite_key_compare<LesserPrice,  LesserTime>;  /* ask orders */

}

namespace Extractor
{

using Price = BOOST_MULTI_INDEX_CONST_MEM_FUN( Order, int, Order::getPrice );
using Time = BOOST_MULTI_INDEX_CONST_MEM_FUN( Order, ::Time,  Order::getTime );
using UniqueId = BOOST_MULTI_INDEX_CONST_MEM_FUN( Order, ::UniqueId, Order::getUniqueId );
using PriceTime = boost::multi_index::composite_key<Order, Price, Time>;

}


namespace Tag
{

struct Price {};
struct PriceTime {};
struct UniqueId {};

}


template<typename ComparePriceTime>
using ComparePrice = typename boost::tuples::element<0,
    typename ComparePriceTime::key_comp_tuple>::type;


template<typename ComparePriceTime>
using OrderContainerImpl = boost::multi_index::multi_index_container<
    OrderPtr, boost::multi_index::indexed_by<

        /* iterate orders in a match event */
        boost::multi_index::ordered_unique<
            boost::multi_index::tag<Tag::PriceTime>,
                Extractor::PriceTime, ComparePriceTime>,

        /* iterate orders by price levels */
        boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<Tag::Price>,
                Extractor::Price, ComparePrice<ComparePriceTime> >,
                
        /* find order by unique id */
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<Tag::UniqueId>,
                Extractor::UniqueId > >
>;

using BidOrderContainer = OrderContainerImpl<Comparator::HigherPriceLesserTime>; /* container type for bid orders */
using AskOrderContainer = OrderContainerImpl<Comparator::LesserPriceLesserTime>;  /* container type for ask orders */


