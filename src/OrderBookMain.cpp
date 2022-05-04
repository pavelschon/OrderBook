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
        .def("newOrder", &OrderBook::newOrder, py::args("ip", "port", "orderId", "side", "price", "qty" ))
        .def("cancelOrder", &OrderBook::cancelOrder, py::args("ip", "port", "orderId"))
        .def("cancelAll", &OrderBook::cancelAll, py::args("ip", "port"))
        .def("topOfBook", &OrderBook::topOfBook)
        .def("flush", &OrderBook::flush)
    ;
}



