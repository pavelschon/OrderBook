cmake_minimum_required( VERSION 3.7 )

project( orderbook CXX )

set( CMAKE_VERBOSE_MAKEFILE OFF )

add_compile_options( -Wall )
add_compile_options( -pedantic )
add_compile_options( -fPIC )
add_compile_options( -std=c++17 )

find_package( PythonLibs   "3.5" REQUIRED )
find_package( PythonInterp "3.5" REQUIRED )
find_package( Boost COMPONENTS python3 REQUIRED )

set( PythonPkgPathCmd "import distutils.sysconfig as cfg; print(cfg.get_python_lib())" )

execute_process (
    COMMAND ${PYTHON_EXECUTABLE} -c "${PythonPkgPathCmd}"
    OUTPUT_VARIABLE PythonPkgPath OUTPUT_STRIP_TRAILING_WHITESPACE
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PYTHON_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

add_library( orderbook SHARED
    src/OrderBookMain.cpp
    src/OrderBook.cpp
    src/Order.cpp
    src/NewOrder.cpp
    src/CancelOrder.cpp
    src/Trade.cpp
    src/Response.cpp
)

set_target_properties( orderbook PROPERTIES PREFIX "" )
target_link_libraries ( orderbook ${Boost_LIBRARIES} )

install(
    TARGETS orderbook
    DESTINATION ${PythonPkgPath}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)

install(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    DESTINATION /usr/share/orderbook
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
    DESTINATION /usr/share/orderbook
    FILES_MATCHING PATTERN "*.py"
    PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
    DESTINATION /usr
    FILES_MATCHING PATTERN "*.py"
    PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

set( CPACK_GENERATOR "DEB" )

set( CPACK_PACKAGE_VERSION_MAJOR "1" )
set( CPACK_PACKAGE_VERSION_MINOR "0" )
set( CPACK_PACKAGE_VERSION_PATCH "0" )

set( CPACK_DEBIAN_PACKAGE_DESCRIPTION "Orderbook engine" )
set( CPACK_DEBIAN_PACKAGE_MAINTAINER "Pavel Schön <pavel@schon.cz>" )
set( CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/pavelschon/Orderbook.git")
set( CPACK_DEBIAN_PACKAGE_DEPENDS "python3 (>= 3.5), python3-twisted (>= 16.6), libboost-python${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}" )
set( CPACK_DEBIAN_PACKAGE_BUILD_DEPENDS "g++, python3-dev (>= 3.5), libboost-all-dev" )

include(CPack)
