#include "mud/core/log.h"
#include "mud/core/internal/log.h"
#include <chrono>
#include <ctime>
#include <cxxabi.h>
#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

BEGIN_MUDLIB_CORE_NS

/* ======================================================================
 * Application wide settings
 * ====================================================================== */

/* The application log settings. A mutex is used to acquire exclusive access. */
log::severity_t       g_severity = log::severity_t::info;
log::target_t         g_targets = log::target_t::none;
std::filesystem::path g_file_path;
log::format_t         g_formats = log::format_t::none;

/* The internal log settings. A mutex is used to acquire exclusive access. */
log::severity_t       g_internal_severity = log::severity_t::info;
log::target_t         g_internal_targets = log::target_t::none;
std::filesystem::path g_internal_file_path;
log::format_t         g_internal_formats = log::format_t::none;

/* A thread-hash vector, initialised with the main thread (0). */
std::vector<size_t> g_thread_ids = {
    std::hash<std::thread::id>{}(std::this_thread::get_id())
};

/* The mutex for exclusive access */
std::mutex            g_mutex;

/* static */ void
log::severity(severity_t level)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    g_severity = level;
}

/* static */ log::severity_t
log::severity()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    auto value = g_severity;
    return value;
}

/* static */ void
log::targets(log::target_t target_mask, const std::filesystem::path& path)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    g_targets = target_mask;
    g_file_path = path;
}

/* static */ log::target_t
log::targets()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    auto value = g_targets;
    return value;
}

/* static */ std::filesystem::path
log::file_path()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    auto value = g_file_path;
    return value;
}

/* static */ void
log::formats(log::format_t format_mask)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    g_formats = format_mask;
}

/* static */ log::format_t
log::formats()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    auto value = g_formats;
    return value;
}

/* static */ void
log::internal_severity(log::severity_t level)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    g_internal_severity = level;
}

/* static */ log::severity_t
log::internal_severity()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    auto value = g_internal_severity;
    return value;
}

/* static */ void
log::internal_targets(
        log::target_t target_mask,
        const std::filesystem::path& path)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    g_internal_targets = target_mask;
    g_internal_file_path = path;
}

/* static */ log::target_t
log::internal_targets()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    auto value = g_internal_targets;
    return value;
}

/* static */ std::filesystem::path
log::internal_file_path()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    auto value = g_internal_file_path;
    return value;
}

/* static */ void
log::internal_formats(log::format_t format_mask)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    g_internal_formats = format_mask;
}

/* static */ log::format_t
log::internal_formats()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    auto value = g_internal_formats;
    return value;
}

size_t
get_tread_id(std::thread::id id)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    auto hash = std::hash<std::thread::id>{}(id);
    auto found = std::find(g_thread_ids.begin(), g_thread_ids.end(), hash);
    if (found == g_thread_ids.end()) {
        // Only keep a record of 1000 threads
        if (g_thread_ids.size() > 1000) {
            auto first = g_thread_ids[0];
            g_thread_ids.clear();
            g_thread_ids.push_back(first);
        }
        g_thread_ids.push_back(hash);
        found = g_thread_ids.end();
    }
    return std::distance(g_thread_ids.begin(), found);
}

/**
 * Initialiser to examine the environment for MUDLIB_LOG* settings.
 */
struct __initialiser__
{
    __initialiser__ () {
        /* Override mudlib severity log level */
        const char* level = getenv("MUDLIB_LOGLEVEL");
        if (level != nullptr) {
            if (strcmp(level, "TRACE") == 0) {
                g_internal_severity = log::severity_t::trace;
            }
            else
            if (strcmp(level, "DEBUG") == 0) {
                g_internal_severity = log::severity_t::debug;
            }
            else
            if (strcmp(level, "INFO") == 0) {
                g_internal_severity = log::severity_t::info;
            }
            else
            if (strcmp(level, "WARN") == 0) {
                g_internal_severity = log::severity_t::warn;
            }
            else
            if (strcmp(level, "ERROR") == 0) {
                g_internal_severity = log::severity_t::error;
            }
            else
            if (strcmp(level, "FATAL") == 0) {
                g_internal_severity = log::severity_t::fatal;
            }
            else {
                // Silently ignore
            }
        }

        /* Override mudlib targets */
        const char* targets = getenv("MUDLIB_LOGTARGETS");
        if (targets != nullptr) {
            while (*targets != '\0') {
                /* Add the target. The NONE takes precedence over all */
                if (strncmp(targets, "NONE", 4) == 0) {
                    g_internal_targets = log::target_t::none;
                    break;
                }
                else
                if (strncmp(targets, "CONSOLE", 7) == 0) {
                    g_internal_targets = g_internal_targets 
                                     | log::target_t::console;
                    targets += 7;
                }
                else
                if (strncmp(targets, "FILE", 4) == 0) {
                    g_internal_targets = g_internal_targets 
                                     | log::target_t::file;
                    targets += 4;
                }
                else
                if (strncmp(targets, "SYSTEM", 6) == 0) {
                    g_internal_targets = g_internal_targets 
                                     | log::target_t::system;
                    targets += 6;
                }
                else {
                    // Silently ignore
                    break;
                }

                /* Any more ? */
                if (*targets == ',') {
                    ++targets;
                }
                else {
                    // Silently ignore
                    break;
                }
            }
        }

        /* Override mudlib file path */
        const char* logfile = getenv("MUDLIB_LOGFILE");
        if (logfile != nullptr) {
            g_internal_file_path = std::filesystem::path(logfile);
        }

        /* Override mudlib formats */
        const char* formats = getenv("MUDLIB_LOGFORMATS");
        if (formats != nullptr) {
            while (*formats != '\0') {
                /* Add the target. The NONE takes precedence over all */
                if (strncmp(formats, "NONE", 4) == 0) {
                    g_internal_formats = log::format_t::none;
                    break;
                }
                else
                if (strncmp(formats, "TIMESTAMP", 9) == 0) {
                    g_internal_formats = g_internal_formats 
                                     | log::format_t::timestamp;
                    formats += 9;
                }
                else
                if (strncmp(formats, "SEVERITY", 8) == 0) {
                    g_internal_formats = g_internal_formats 
                                     | log::format_t::severity;
                    formats += 8;
                }
                else
                if (strncmp(formats, "THREAD", 6) == 0) {
                    g_internal_formats = g_internal_formats 
                                     | log::format_t::thread;
                    formats += 6;
                }
                else {
                    // Silently ignore
                    break;
                }

                /* Any more ? */
                if (*formats == ',') {
                    ++formats;
                }
                else {
                    // Silently ignore
                    break;
                }
            }
        }

    }
} __init__;

/* ======================================================================
 * Log state
 * ====================================================================== */

/**
 * @brief State information
 *
 * The state information is shared between all the stream buffers that are
 * in use within a certain instance.
 */
struct state
{
    /** Flag to indicate if the logging is internal. */
    bool internal;

    /** The minimum severity level. */
    log::severity_t severity;

    /** The log severity level. */
    log::severity_t log_severity;

    /** The current formatting options. */
    log::format_t format;
};


/* ======================================================================
 * Formatted string buffer
 * ====================================================================== */

/**
 * @brief String stream buffer that handles formatting options.
 */
class formatted_stringbuf: public std::stringbuf
{
public:
    /**
     * @brief Create a string buffer.
     * @param stream_state The shared state of the stream buffers.
     */
    formatted_stringbuf(state& stream_state);

    /**
     * @Destructor.
     */
    ~formatted_stringbuf() = default;

    /**
     * @brief Return the formatted options.
     */
    const char* format();

protected:
    /** The stream state */
    state& _state;

    /** The buffer holding the formatted string. */
    char _buf[56];
};

formatted_stringbuf::formatted_stringbuf(state& stream_state)
    : _state(stream_state)
{
    _buf[0] = '\0';
}

const char*
formatted_stringbuf::format()
{   
    // The maximum format be like
    //     [YYYY-MM-DD HH:MM:SS.sss][SSSSS][TTTTTTTTTTTTTTTTTTTT]_
    //     0        1         2         3         4         5
    //     1234567890123456789012345678901234567890123456789012345
    char* cptr = _buf;
    if ((_state.format & log::format_t::timestamp) == log::format_t::timestamp)
    {
        // Get the current time in local-time zone and the milliseconds
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()).count() % 1000;
        auto local = *std::localtime(&time);

        // Format the date + time
        *cptr++ = '[';
        cptr += strftime(cptr, 23, "%F %T", &local);

        // format the milliseconds component
        *cptr++ = '.';
        for (int divider = 1000; divider >= 1; divider /= 10) {
            auto digit = ms / divider;
            ms -= digit * divider;
            *cptr++ = '0'  + digit;
        }
        *cptr++ = ']';
    }
    if ((_state.format & log::format_t::severity) == log::format_t::severity)
    {
        *cptr++ = '[';
        const char *desc;
        switch (_state.log_severity) {
            case log::severity_t::trace:
                desc = "TRACE";
                break;
            case log::severity_t::debug:
                desc = "DEBUG";
                break;
            case log::severity_t::info:
                desc = "INFO ";
                break;
            case log::severity_t::warn:
                desc = "WARN ";
                break;
            case log::severity_t::error:
                desc = "ERROR";
                break;
            case log::severity_t::fatal:
                desc = "FATAL";
                break;
            default:
                desc = "UNKNW";
                break;
        }
        for (int i = 0; i < 5; ++i) {
            *cptr++ = *desc++;
        }
        *cptr++ = ']';
    }
    if ((_state.format & log::format_t::thread) == log::format_t::thread)
    {
        // Get the index of the hash
        size_t id = get_tread_id(std::this_thread::get_id());
        *cptr++ = '[';
        for (int divider = 10000; divider >= 1; divider /= 10) {
            auto digit = id / divider;
            id -= digit * divider;
            *cptr++ = '0'  + digit;
        }
        *cptr++ = ']';
    }
    if (_state.format != log::format_t::none) {
        // Add a separator between the format fields and the message.
        *cptr++ = ' ';
    }
    *cptr = '\0';
    return _buf;
}

/* ======================================================================
 * Tee stream buffer
 * ====================================================================== */

/**
 * @brief Tee stream buffer
 *
 * The tee stream buffer outputs everything to all attached buffers.
 */
class tee_streambuf : public std::streambuf
{
public:
    /**
     * @brief Construct a stream buffer that sends output to multiple buffers.
     * @param stream_state The shared state of the stream buffers.
     * @param streambufs The destination stream buffers.
     */
    template<class... Args>
    tee_streambuf(state& stream_state, Args... streambufs)
        : _state(stream_state)
    {
        _targets = { streambufs... };
        for (auto iter = _targets.begin(); iter != _targets.end(); /* */) {
            if ((*iter) == nullptr) {
                iter = _targets.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    /**
     * @brief Destructor.
     */
    virtual ~tee_streambuf() = default;

    /**
     * @brief Write a character to the buffer,
     * @param ch The character to write
     * @return The character written or EOF on failure.
     */
    virtual int overflow(int ch) override;

    /**
     * @brief Synchronise the buffer to the output target
     * @return Zero upon success, or -1 on failure.
     */
    virtual int sync() override;

private:
    /** The stream state */
    state& _state;

    /** The target stream buffers */
    std::vector<std::shared_ptr<std::streambuf>> _targets;
};

int
tee_streambuf::overflow(int ch)
{
    for (auto target : _targets) {
        target->sputc(ch);
    }
    return ch;
}

int
tee_streambuf::sync()
{
    for (auto target : _targets) {
        target->pubsync();
    }
    return 0;
}

/* ======================================================================
 * Null stream buffer
 * ====================================================================== */

/**
 * @brief Null stream buffer
 *
 * The null stream ignores all output that is issues to it.
 */
class null_streambuf: public std::streambuf
{
public:
    /**
     * @brief Construct a streambuffer that ignores any output.
     */
    null_streambuf();

    /**
     *  @brief Destructor.
     */
    ~null_streambuf();

    /**
     * @brief Write a charcater to the buffer,
     * @param ch The character to write
     * @return The character written or EOF on failure.
     */
    virtual int overflow(int ch) override;

    /**
     * @brief Synchronise the buffer to the output target
     * @return Zero upon success, or -1 on failure.
     */
    virtual int sync() override;
};

null_streambuf::null_streambuf()
{
}

null_streambuf::~null_streambuf()
{
}

int
null_streambuf::overflow(int ch)
{
    return ch;
}

int
null_streambuf::sync()
{
    return 0;
}

/* ======================================================================
 * Console stream buffer
 * ====================================================================== */

/**
 * @brief Console stream buffer
 *
 * The console stream outputs to std::cout and std::cerr.
 */
class console_streambuf: public formatted_stringbuf
{
public:
    /**
     * @brief Construct a streambuffer that outputs to the console.
     * @param stream_state The shared state of the stream buffers.
     */
    console_streambuf(state& stream_state);

    /**
     *  @brief Destructor.
     */
    ~console_streambuf();

    /**
     * @brief Synchronise the buffer to the output target
     * @return Zero upon success, or -1 on failure.
     */
    virtual int sync() override;

private:
    /* The mutex to avoid console interleaving. */
    static std::mutex g_console_mutex;
};

std::mutex console_streambuf::g_console_mutex;

console_streambuf::console_streambuf(state& stream_state)
    : formatted_stringbuf(stream_state)
{
}

console_streambuf::~console_streambuf()
{
    pubsync();
}

int
console_streambuf::sync()
{
    int nr = pptr() - pbase();
    if (nr > 0) {
        std::lock_guard<std::mutex> lock(g_console_mutex);
        *pptr() = '\0';
        if (_state.log_severity >= log::severity_t::error) {
            std::cerr << format() <<  pbase() << std::flush;
        }
        else {
            std::cout <<  format() << pbase() << std::flush;
        }
        pbump(-nr);
    }
    return 0;
}

/* ======================================================================
 * File stream buffer
 * ====================================================================== */

/**
 * @brief File stream buffer
 *
 * The file stream outputs to a file.
 */
class file_streambuf: public formatted_stringbuf
{
public:
    /**
     * @brief Construct a streambuffer that outputs to a file.
     * @param stream_state The shared state of the stream buffers.
     * @param file_path The file to log to.
     */
    file_streambuf(state& stream_state, const std::filesystem::path& file_path);

    /**
     *  @brief Destructor.
     */
    ~file_streambuf();

    /**
     * @brief Synchronise the buffer to the output target
     * @return Zero upon success, or -1 on failure.
     */
    virtual int sync() override;

private:
    /** The file-stream to log to. */
    std::ofstream _fostr;
};

file_streambuf::file_streambuf(state& stream_state,
        const std::filesystem::path& file_path)
    : formatted_stringbuf(stream_state)
{
    _fostr.open(file_path, std::ios::out | std::ios::app);
}

file_streambuf::~file_streambuf()
{
    pubsync();
    _fostr.close();
}

int
file_streambuf::sync()
{
    int nr = pptr() - pbase();
    if (nr > 0) {
        *pptr() = '\0';
        if (_fostr.is_open()) {
            _fostr <<  format() << pbase() << std::flush;
        }
        pbump(-nr);
    }
    return 0;
}

/* ======================================================================
 * Common Log implementation
 * ====================================================================== */

class base_impl
{
public:
    /**
     * Constructor.
     *
     * @param internal Flag to indicate if the logging is mudlib internal.
     */
    base_impl(bool internal);

    /**
     * Destructor.
     */
    ~base_impl() = default;

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
    /** The shared state of the stream buffers. */
    state _state;

    /** The output stream and stream buffers */
    std::ostream _ostr;
    std::shared_ptr<tee_streambuf> _tee_streambuf;
    std::shared_ptr<console_streambuf> _console_streambuf;
    std::shared_ptr<file_streambuf> _file_streambuf;

    /** The null stream */
    null_streambuf _null_streambuf;
    std::ostream _null_ostr;
};

base_impl::base_impl(bool internal)
    : _ostr(nullptr)
    , _null_ostr(&_null_streambuf)
{
    _state.internal = internal;

    // Get the application or internal settings
    log::target_t targets;
    std::filesystem::path file_path;
    if (internal) {
        _state.severity = log::internal_severity();
        _state.format = log::internal_formats();
        targets = log::internal_targets();
        file_path = log::internal_file_path();
    }
    else {
        _state.severity = log::severity();
        _state.format = log::formats();
        targets = log::targets();
        file_path = log::file_path();
    }

    // Allocate the stream buffers
    if ((targets & log::target_t::console) == log::target_t::console) {
        _console_streambuf = std::make_shared<console_streambuf>(_state);
    }
    if ((targets & log::target_t::file) == log::target_t::file) {
        _file_streambuf = std::make_shared<file_streambuf>(_state,
                file_path);
    }
    _tee_streambuf = std::make_shared<tee_streambuf>(_state,
            _console_streambuf, _file_streambuf);

    // Create the output stream and assign the tee stream-buffer.
    _ostr.rdbuf(_tee_streambuf.get());
}

std::ostream&
base_impl::trace()
{
    _state.log_severity = log::severity_t::trace;
    return (_state.severity <= _state.log_severity) ? _ostr : _null_ostr;
}

std::ostream&
base_impl::debug()
{
    _state.log_severity = log::severity_t::debug;
    return (_state.severity <= _state.log_severity) ? _ostr : _null_ostr;
}

std::ostream&
base_impl::info()
{
    _state.log_severity = log::severity_t::info;
    return (_state.severity <= _state.log_severity) ? _ostr : _null_ostr;
}

std::ostream&
base_impl::warn()
{
    _state.log_severity = log::severity_t::warn;
    return (_state.severity <= _state.log_severity) ? _ostr : _null_ostr;
}

std::ostream&
base_impl::error()
{
    _state.log_severity = log::severity_t::error;
    return (_state.severity <= _state.log_severity) ? _ostr : _null_ostr;
}

std::ostream&
base_impl::fatal()
{
    _state.log_severity = log::severity_t::fatal;
    return (_state.severity <= _state.log_severity) ? _ostr : _null_ostr;
}

/* ======================================================================
 * Log implementation
 * ====================================================================== */

class log::impl: public base_impl
{
public:
    /**
     * @brief Constructor
     */
    impl(): base_impl(false) {}

    /**
     * @brief Destructor
     */
    ~impl() {}
};

void
log::impl_deleter::operator()(log::impl* ptr) const
{
    delete ptr;
}

/* ======================================================================
 * Log 
 * ====================================================================== */

log::log()
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl());
}

log::~log()
{}

std::ostream&
log::trace()
{
    return _impl->trace();
}

std::ostream&
log::debug()
{
    return _impl->debug();
}

std::ostream&
log::info()
{
    return _impl->info();
}

std::ostream&
log::warn()
{
    return _impl->warn();
}

std::ostream&
log::error()
{
    return _impl->error();
}

std::ostream&
log::fatal()
{
    return _impl->fatal();
}

/* ======================================================================
 * Internal Log implementation
 * ====================================================================== */

class internal::log::impl: public base_impl
{
public:
    /**
     * @brief Constructor
     */
    impl(): base_impl(true) {}

    /**
     * @brief Destructor
     */
    ~impl() {}
};

void
internal::log::impl_deleter::operator()(internal::log::impl* ptr) const
{
    delete ptr;
}

/* ======================================================================
 * Internal Log 
 * ====================================================================== */

internal::log::log()
{
    _impl = std::unique_ptr<internal::log::impl, internal::log::impl_deleter>(
            new internal::log::impl());
}

internal::log::~log()
{}

std::ostream&
internal::log::trace()
{
    return _impl->trace();
}

std::ostream&
internal::log::debug()
{
    return _impl->debug();
}

std::ostream&
internal::log::info()
{
    return _impl->info();
}

std::ostream&
internal::log::warn()
{
    return _impl->warn();
}

std::ostream&
internal::log::error()
{
    return _impl->error();
}

std::ostream&
internal::log::fatal()
{
    return _impl->fatal();
}

std::string
internal::log::demangle(const std::type_info& tp)
{
    int status = 0;
    const char* mangled = tp.name();
    std::unique_ptr<char, void (*)(void*)> demangled(
        abi::__cxa_demangle(mangled, nullptr, nullptr, &status),
        std::free
    );
    return (status == 0) ? demangled.get() : mangled;
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
