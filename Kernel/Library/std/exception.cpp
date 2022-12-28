/**
 * @file exception.cpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief C++ standard runtime exception implementations
 * @version 0.1
 * @date 2022-12-28
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */

#include "exception.hpp"

namespace std {

// std::exception

exception::exception() throw()
{
    // Stubbed
}
exception::exception(const exception& e) throw()
{
    // Stubbed
    (void)e;
}

exception::~exception()
{
    // Stubbed
}

exception& exception::operator=(const exception& e) throw()
{
    (void)e;
    return *this;
}

const char* exception::what() const throw()
{
    return "std::exception";
}


// std::bad_alloc

bad_alloc::bad_alloc() throw()
{
    // Stubbed
}

bad_alloc::bad_alloc(const bad_alloc& e) throw()
    : exception()
{
    (void)e;
    // Stubbed
}

bad_alloc::~bad_alloc()
{
    // Stubbed
}

bad_alloc& bad_alloc::operator=(const bad_alloc& e) throw()
{
    (void)e;
    return *this;
}

const char* bad_alloc::what() const throw()
{
    return "cxxrt::bad_alloc";
}


// std::bad_cast

bad_cast::bad_cast() throw()
{
    // Stubbed
}

bad_cast::bad_cast(const bad_cast& e) throw()
    : exception()
{
    // Stubbed
    (void)e;
}

bad_cast::~bad_cast()
{
    // Stubbed
}

bad_cast& bad_cast::operator=(const bad_cast& e) throw()
{
    (void)e;
    return *this;
}

const char* bad_cast::what() const throw()
{
    return "std::bad_cast";
}


// std::bad_typeid

bad_typeid::bad_typeid() throw()
{
    // Stubbed
}

bad_typeid::bad_typeid(const bad_typeid& e) throw()
    : exception()
{
    // Stubbed
    (void)e;
}

bad_typeid::~bad_typeid()
{
    // Stubbed
}

bad_typeid& bad_typeid::operator=(const bad_typeid& e) throw()
{
    (void)e;
    return *this;
}

const char* bad_typeid::what() const throw()
{
    return "std::bad_typeid";
}


// std::bad_array_new_length

bad_array_new_length::bad_array_new_length() throw()
{
    // Stubbed
}

bad_array_new_length::bad_array_new_length(const bad_array_new_length& e) throw()
    : bad_alloc()
{
    // Stubbed
    (void)e;
}

bad_array_new_length::~bad_array_new_length()
{
    // Stubbed
}

bad_array_new_length& bad_array_new_length::operator=(const bad_array_new_length& e) throw()
{
    (void)e;
    return *this;
}

const char* bad_array_new_length::what() const throw()
{
    return "std::bad_array_new_length";
}

} // namespace std
