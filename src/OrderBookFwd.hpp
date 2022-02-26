/**
 * @brief Forward declarations and simple types
 *
 */

#pragma once

#include <chrono>
#include <memory>
#include <tuple>

#include <boost/python/list.hpp>
#include <boost/optional.hpp>

using PyList = boost::python::list;
using UniqueId = std::tuple<int, int>;
using Time = std::chrono::time_point<std::chrono::high_resolution_clock>;


/* side helper */
namespace Side
{
    static const char Buy = 'B';
    static const char Sell = 'S';
}


struct TopOfBook
{
    using Optional = boost::optional<TopOfBook>;
    
    int price;
    int qty;
    
    /* compare top-of-book objects */
    inline bool operator!=( const TopOfBook& other) const
    {
        return price != other.price || qty != other.qty;
    }
};


