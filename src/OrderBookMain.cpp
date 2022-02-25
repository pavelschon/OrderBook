/**
 * @brief OrderBook python bindings
 *
 */


#include "OrderBook.hpp"

#include <boost/python.hpp>


BOOST_PYTHON_MODULE( orderbook )
{
    namespace py = ::boost::python;

    /* initialize random number generator */
    std::srand(static_cast<const unsigned int>(std::time( nullptr)));

    py::class_<OrderBook, boost::noncopyable>( "OrderBook" )
        .def("newOrder", &OrderBook::newOrder, py::args("userId", "price", "qty", "side", "orderId"))
        .def("cancelOrder", &OrderBook::cancelOrder, py::args("userId", "orderId"))
        .def("flush", &OrderBook::flush)
    ;
}



