/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

#ifndef _MUDLIB_CORE_PRIVATE_LOG_H_
#define _MUDLIB_CORE_PRIVATE_LOG_H_

#include <iostream>
#include <typeinfo>
#include <mud/core/log.h>

BEGIN_MUDLIB_CORE_NS

namespace internal {

/**
 * @brief Log information for mudlib library internally.
 *
 * Logging information specifically for internal logging that is controlled
 * by the MUDLIB_LOGxxx environments and log::mudlib_severity and
 * log::mudlib_targets functions.
 */
class MUDLIB_CORE_API log
{
public:
    /**
     * @brief Construct a new log stream for internal logging.
     *
     * Create a new log stream. The stream will be assigned its own private log
     * buffer and will only output to the targets when a log message has been
     * constructed (generally after a std::endl or std::flush).
     */
    log();

    /**
     *@ brief Destruct a log stream.
     *
     * Any outstanding message will be flushed to the targets.
     */
    ~log();

    /**
     * @brief Return a stream for trace messages.
     */
    std::ostream& trace();

    /**
     * @brief Return a stream for debug messages.
     */
    std::ostream& debug();

    /**
     * @brief Return a stream for informational messages.
     */
    std::ostream& info();

    /**
     * @brief Return a stream for warning messages.
     */
    std::ostream& warn();

    /**
     * @brief Return a stream for error messages.
     */
    std::ostream& error();

    /**
     * @brief Return a stream for fatal error messages.
     */
    std::ostream& fatal();

    /**
     * @brief Return a demangled representation of a type name, if possible.
     * @param tp The type to demangle.
     */
    static std::string demangle(const std::type_info& tp);

private:
    /**
     * Implementation
     */
    class impl;
    struct impl_deleter
    {
        void operator()(impl*) const;
    };
    std::unique_ptr<impl, impl_deleter> _impl;
};

} /* internal */

END_MUDLIB_CORE_NS

/** 
 * @brief Create an internal log object.
 * @param obj The object variable.
 */
#define LOG(obj)                                                               \
    mud::core::internal::log obj

/** 
 * @brief Return an internal trace stream.
 * @param obj The internal log object variable.
 */
#define TRACE(obj)                                                             \
    if (mud::core::log::internal_severity()                                    \
        > mud::core::log::severity_t::trace)                                   \
    {} else obj.trace()

/** 
 * @brief Return an internal debug stream.
 * @param obj The internal log object variable.
 */
#define DEBUG(obj)                                                             \
    if (mud::core::log::internal_severity()                                    \
        > mud::core::log::severity_t::debug)                                   \
    {} else obj.debug()

/** 
 * @brief Return an internal info stream.
 * @param obj The internal log object variable.
 */
#define INFO(obj)                                                             \
    if (mud::core::log::internal_severity()                                    \
        > mud::core::log::severity_t::info)                                   \
    {} else obj.info()

/** 
 * @brief Return an internal error stream.
 * @param obj The internal log object variable.
 */
#define WARN(obj)                                                             \
    if (mud::core::log::internal_severity()                                    \
        > mud::core::log::severity_t::warn)                                   \
    {} else obj.warn()

/** 
 * @brief Return an internal fatal stream.
 * @param obj The internal log object variable.
 */
#define ERROR(obj)                                                             \
    if (mud::core::log::internal_severity()                                    \
        > mud::core::log::severity_t::error)                                   \
    {} else obj.error()

/** 
 * @brief Return an internal trace stream.
 * @param obj The internal log object variable.
 */
#define FATAL(obj)                                                             \
    if (mud::core::log::internal_severity()                                    \
        > mud::core::log::severity_t::fatal)                                   \
    {} else obj.fatal()

/**
 * @brief Return the severity log level
 * @param obj The internal log object variable.
 */
#define SEVERITY()                                                             \
    mud::core::log::internal_severity()

/**
 * @brief Return a name for the type.
 * @param tp The type to query.
 */
#define TYPEINFO(tp)                                                           \
    mud::core::internal::log::demangle(typeid(tp))

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_PRIVATE_LOG_H_ */
