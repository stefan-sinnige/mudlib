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

#ifndef _MUDLIB_CORE_LOG_H_
#define _MUDLIB_CORE_LOG_H_

#include <mud/core/ns.h>
#include <filesystem>
#include <iostream>
#include <memory>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief Log information.
 *
 * Logging information to any targets. Information is categorised based on
 * severity as follows:
 *
 *   - TRACE: most detailed level of logging.
 *   - DEBUG: information for debugging purposes.
 *   - INFO: Informational messages.
 *   - WARN: Warning messages that would not usually impact functionality.
 *   - ERROR: Error conditions that impacts functionality.
 *   - FATAL: Severe error condtion that may lead to termination.
 *
 * The application may be instructed to only log messages of a certain minimum
 * severity. If the minimum severuty is set to @c WARN, only the warning, error
 * and fatal messages are logged. Any informational, debug and trace messages
 * are discarded. The minimum severity level can be set programmatically and
 * will be applied to any message processing in any thread.
 *
 * The logging facility allows multiple targets to be used. These include
 *
 *   - NULL: Disable logging altogether.
 *   - CONSOLE: Console logging (@c std::cout for trace, debug and info
 *     messages, and @c std::cerr for warning, error and fatal messages).
 *   - FILE: Logging of all severities to a file.
 *   - SYSTEM: Logging to the system's logging facility, mapping the severity
 *     levels to the appropriate level provided by the system.
 *
 * Multiple targets can be provided, in which case the messages will be written
 * to any of those targets simuktaneously.
 *
 * @section Formatting options
 *
 * The message being logged can be formatted with additional information. This
 * includes the following:
 *
 *    - SEVERITY: Add the severity of the message
 *    - TIMESTAMP: Add a time-stamp when the message is logged
 *    - THREAD: Add a thread-id of the thread logging the message
 *
 * Only threads that have been logged on will be issued a thread ID. The thread
 * ID is a sequential ID that is issued on the first log on that thread and they
 * are recycled after 1000 different threads have been logged from.
 *
 * @section Performance Considerations
 *
 * Console output logging will be synchronised between threads to ensure that
 * there is no interleaving of log messages. With a high amount of logging, this
 * may delay thread execution while waiting for other threads to finish their
 * logging to the console. Only the final output to the console is subject to
 * the synchronisation, the streaming operations themselves are not.
 * Note that interleaving may still be present between log messages and non-log
 * messages from different threads.
 *
 * Thread formatting will be synchronised when acquiring the unique mapping of
 * the thread-id. The number of active threads that log messages is proportional
 * to the performance degradation due to the protection of the container that
 * holds the mapping.
 *
 * Too many simultaneous logging to a log file may result in access permission
 * restrictions imposed by the underlying operating system or file system. If
 * a log instance is unable to open the log-file, the logging will be silently
 * ignored.
 *
 * @section MUDLIB internal logging
 *
 * Logging of internal mudlib commands can be enabled programmatically with
 * the log::internal_severity and  log::internal_targets functions, or through
 * environment variables (programmattic takes precedence):
 *
 *    - @c MUDLIB_LOGLEVEL=[TRACE|DEBUG|INFO|WARN|ERROR|FATAL]
 *      A single entry defining the minimum mudlib logging severity.
 *    - @c MUDLIB_LOGTARGETS=[CONSOLE,FILE,SYSTEM]
 *      Comma-separated list of targets.
 *    - @c MUDLIB_LOGFILE=[PATH]
 *      The path to a log-file if the FILE target is chosen.
 *    - @c MUDLIB_LOGFORMATS=[SEVERITY,TIMESTAMP,THREAD]
 *      Comma-separated list of formatting options.
 *
 * Only internal mudlib log messages will be reported with these settings and
 * does not impact overall application level log settings. By default, internal
 * logging is disabled.
 */
class MUDLIB_CORE_API log
{
public:
    /**
     * The supported log severity levels.
     */
    enum class severity_t
    {
        trace,   /**< Most detailed logging */
        debug,   /**< Diagnostics debug level logging */
        info,    /**< Informational level logging */
        warn,    /**< Warning level logging */
        error,   /**< Error condition level logging */
        fatal    /**< Fatal condition level logging */
    };

    /**
     * The supported log target bit-mask. Multiple targets can be combined
     * by a bit-wise OR of their individual target.
     */
    enum class target_t: uint8_t
    {
        none    = 0,      /** No logging target */
        console = 1 << 0, /** Console logging target */
        file    = 1 << 1, /** File logging target */
        system  = 1 << 2  /** System logging target */
    };

    /**
     * The supported log formatting bit-mask. Multiple options can be combined
     * by a bit-wise OR of their individual option.
     */
    enum class format_t: uint8_t
    {
        none      = 0,      /** No formatting options */
        severity  = 1 << 1, /** Severity format option */
        timestamp = 1 << 2, /** Timestamp format option */
        thread    = 1 << 3  /** Thread ID format option */
    };

    /**
     * @brief Construct a new log stream.
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
     * @brief Set the application severity level.
     * @param level The minimum sevrity level.
     *
     * Defines the minimum log-level to output to the targets. Any message that
     * is logged at a lower level will be discarded.
     *
     * This is an application-wide setting.
     */
    static void severity(severity_t level);

    /**
     * @brief Get the application severity level.
     *
     * This is an application-wide setting.
     */
    static severity_t severity();

    /**
     * @brief Set the application log target mask.
     * @param mask The bitmask of the targets to log to. Multiple targets
     * can be chosen by combining them with a bitwise-or operation.
     * @param path The path to a file when a @c file target is used.
     *
     * This is an application-wide setting.
     */
    static void targets(
            target_t mask,
            const std::filesystem::path& path = std::filesystem::path());

    /**
     * @brief Get the application log target mask.
     *
     * This is an application-wide setting.
     */
    static target_t targets();

    /**
     * @brief Get the application log target file path.
     *
     * This is an application-wide setting.
     */
    static std::filesystem::path file_path();

    /**
     * @brief Set the application log format mask.
     * @param mask The bitmask of the format options. Multiple options can be
     * by combining them with a bitwise-or operation.
     *
     * This is an application-wide setting.
     */
    static void formats(format_t mask);

    /**
     * @brief Get the application log format mask.
     *
     * This is an application-wide setting.
     */
    static format_t formats();

    /**
     * @brief Set the mudlib severity level.
     * @param level The minimum sevrity level.
     *
     * Defines the minimum log-level to output to the targets. Any internal
     * mudlib message that is logged at a lower level will be discarded.
     *
     * This setting only applies to mudlib internal logging and is not an
     * application-wide setting.
     */
    static void internal_severity(severity_t level);

    /**
     * @brief Get the mudlib severity level.
     *
     * This setting only applies to mudlib internal logging and is not an
     * an application-wide setting.
     */
    static severity_t internal_severity();

    /**
     * @brief Set the mudlib log target mask.
     * @param mask The bitmask of the targets to log to. Multiple targets
     * can be chosen by combining them with a bitwise-or.
     * @param path The path to a file when a @c file target is used.
     *
     * This setting only applies to mudlib internal logging and is not an
     * application-wide setting.
     */
    static void internal_targets(
            target_t mask,
            const std::filesystem::path& path = std::filesystem::path());

    /**
     * @brief Get the mudlib log target mask.
     *
     * This setting only applies to mudlib internal logging and is not an
     * application-wide setting.
     */
    static target_t internal_targets();

    /**
     * @brief Get the application log target file path.
     *
     * This setting only applies to mudlib internal logging and is not an
     * application-wide setting.
     */
    static std::filesystem::path internal_file_path();

    /**
     * @brief Set the ainternal log format mask.
     * @param mask The bitmask of the format options. Multiple options can be
     * by combining them with a bitwise-or operation.
     *
     * This setting only applies to mudlib internal logging and is not an
     * application-wide setting.
     */
    static void internal_formats(format_t mask);

    /**
     * @brief Get the internal log format mask.
     *
     * This setting only applies to mudlib internal logging and is not an
     * application-wide setting.
     */
    static format_t internal_formats();

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

/**
 * Bitwise-or operator on @c target_t types.
 */
inline constexpr log::target_t operator|(log::target_t lhs, log::target_t rhs) {
    return static_cast<log::target_t>(
        static_cast<std::underlying_type_t<log::target_t>>(lhs) |
        static_cast<std::underlying_type_t<log::target_t>>(rhs));
}

/**
 * Bitwise-and operator on @c target_t types.
 */
inline constexpr log::target_t operator&(log::target_t lhs, log::target_t rhs) {
    return static_cast<log::target_t>(
        static_cast<std::underlying_type_t<log::target_t>>(lhs) &
        static_cast<std::underlying_type_t<log::target_t>>(rhs));
}

/**
 * Bitwise-or operator on @c format_t types.
 */
inline constexpr log::format_t operator|(log::format_t lhs, log::format_t rhs) {
    return static_cast<log::format_t>(
        static_cast<std::underlying_type_t<log::format_t>>(lhs) |
        static_cast<std::underlying_type_t<log::format_t>>(rhs));
}

/**
 * Bitwise-and operator on @c format_t types.
 */
inline constexpr log::format_t operator&(log::format_t lhs, log::format_t rhs) {
    return static_cast<log::format_t>(
        static_cast<std::underlying_type_t<log::format_t>>(lhs) &
        static_cast<std::underlying_type_t<log::format_t>>(rhs));
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_LOG_H_ */
