#ifndef BOOST_STD_CONFIGURATION_H
#define BOOST_STD_CONFIGURATION_H

#define STD_SHARED_MUTEX_FOUND 1
#define Boost_SHARED_MUTEX_FOUND 0

#if STD_SHARED_MUTEX_FOUND
#include <shared_mutex>
namespace log4cxx {
    typedef std::shared_mutex shared_mutex;
    template <typename T>
    using shared_lock = std::shared_lock<T>;
}
#elif Boost_SHARED_MUTEX_FOUND
#include <boost/thread/shared_mutex.hpp>
namespace log4cxx {
    typedef boost::shared_mutex shared_mutex;
    template <typename T>
    using shared_lock = boost::shared_lock<T>;
}
#endif

#endif /* BOOST_STD_CONFIGURATION_H */
