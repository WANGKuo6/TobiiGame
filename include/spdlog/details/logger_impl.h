//
// Copyright(c) 2015 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#pragma once

#include "spdlog/details/fmt_helper.h"

#include <memory>
#include <string>

// create logger with given name, sinks and the default pattern formatter
// all other ctors will call this one
template<typename It>
inline spdlog::logger::logger(std::string logger_name, It begin, It end)
    : name_(std::move(logger_name))
    , sinks_(begin, end)
    , level_(level::info)
    , flush_level_(level::off)
    , last_err_time_(0)
    , msg_counter_(1) // message counter will start from 1. 0-message id will be
                      // reserved for controll messages
{
    err_handler_ = [this](const std::string &msg) { this->default_err_handler_(msg); };
}

// ctor with sinks as init list
inline spdlog::logger::logger(std::string logger_name, sinks_init_list sinks_list)
    : logger(std::move(logger_name), sinks_list.begin(), sinks_list.end())
{
}

// ctor with single sink
inline spdlog::logger::logger(std::string logger_name, spdlog::sink_ptr single_sink)
    : logger(std::move(logger_name), {std::move(single_sink)})
{
}

inline spdlog::logger::~logger() = default;

inline void spdlog::logger::set_formatter(std::unique_ptr<spdlog::formatter> f)
{
    for (auto &sink : sinks_)
    {
        sink->set_formatter(f->clone());
    }
}

inline void spdlog::logger::set_pattern(std::string pattern, pattern_time_type time_type)
{
    auto new_formatter = details::make_unique<spdlog::pattern_formatter>(std::move(pattern), time_type);
    set_formatter(std::move(new_formatter));
}

template<typename... Args>
inline void spdlog::logger::log(level::level_enum lvl, const char *fmt, const Args &... args)
{
    if (!should_log(lvl))
    {
        return;
    }

    try
    {
        details::log_msg log_msg(&name_, lvl);
        fmt::format_to(log_msg.raw, fmt, args...);
        sink_it_(log_msg);
    }
    SPDLOG_CATCH_AND_HANDLE
}

template<typename... Args>
inline void spdlog::logger::log(level::level_enum lvl, const char *msg)
{
    if (!should_log(lvl))
    {
        return;
    }
    try
    {
        details::log_msg log_msg(&name_, lvl);
        details::fmt_helper::append_c_str(msg, log_msg.raw);
        sink_it_(log_msg);
    }
    SPDLOG_CATCH_AND_HANDLE
}

template<typename T>
inline void spdlog::logger::log(level::level_enum lvl, const T &msg)
{
    if (!should_log(lvl))
    {
        return;
    }
    try
    {
        details::log_msg log_msg(&name_, lvl);
        fmt::format_to(log_msg.raw, "{}", msg);
        sink_it_(log_msg);
    }
    SPDLOG_CATCH_AND_HANDLE
}

template<typename... Args>
inline void spdlog::logger::trace(const char *fmt, const Args &... args)
{
    log(level::trace, fmt, args...);
}

template<typename... Args>
inline void spdlog::logger::debug(const char *fmt, const Args &... args)
{
    log(level::debug, fmt, args...);
}

template<typename... Args>
inline void spdlog::logger::info(const char *fmt, const Args &... args)
{
    log(level::info, fmt, args...);
}

template<typename... Args>
inline void spdlog::logger::warn(const char *fmt, const Args &... args)
{
    log(level::warn, fmt, args...);
}

template<typename... Args>
inline void spdlog::logger::error(const char *fmt, const Args &... args)
{
    log(level::err, fmt, args...);
}

template<typename... Args>
inline void spdlog::logger::critical(const char *fmt, const Args &... args)
{
    log(level::critical, fmt, args...);
}

template<typename T>
inline void spdlog::logger::trace(const T &msg)
{
    log(level::trace, msg);
}

template<typename T>
inline void spdlog::logger::debug(const T &msg)
{
    log(level::debug, msg);
}

template<typename T>
inline void spdlog::logger::info(const T &msg)
{
    log(level::info, msg);
}

template<typename T>
inline void spdlog::logger::warn(const T &msg)
{
    log(level::warn, msg);
}

template<typename T>
inline void spdlog::logger::error(const T &msg)
{
    log(level::err, msg);
}

template<typename T>
inline void spdlog::logger::critical(const T &msg)
{
    log(level::critical, msg);
}

#ifdef SPDLOG_WCHAR_TO_UTF8_SUPPORT

inline void wbuf_to_utf8buf(const fmt::wmemory_buffer &wbuf, fmt::memory_buffer &target)
{
    int wbuf_size = static_cast<int>(wbuf.size());
    if (wbuf_size == 0)
    {
        return;
    }

    auto result_size = ::WideCharToMultiByte(CP_UTF8, 0, wbuf.data(), wbuf_size, NULL, 0, NULL, NULL);

    if (result_size > 0)
    {
        target.resize(result_size);
        ::WideCharToMultiByte(CP_UTF8, 0, wbuf.data(), wbuf_size, &target.data()[0], result_size, NULL, NULL);
    }
    else
    {
        throw spdlog::spdlog_ex(fmt::format("WideCharToMultiByte failed. Last error: {}", ::GetLastError()));
    }
}

template<typename... Args>
inline void spdlog::logger::log(level::level_enum lvl, const wchar_t *fmt, const Args &... args)
{
    if (!should_log(lvl))
    {
        return;
    }

    try
    {
        // format to wmemory_buffer and convert to utf8
        details::log_msg log_msg(&name_, lvl);
        fmt::wmemory_buffer wbuf;
        fmt::format_to(wbuf, fmt, args...);
        wbuf_to_utf8buf(wbuf, log_msg.raw);
        sink_it_(log_msg);
    }
    SPDLOG_CATCH_AND_HANDLE
}

template<typename... Args>
inline void spdlog::logger::trace(const wchar_t *fmt, const Args &... args)
{
    log(level::trace, fmt, args...);
}

template<typename... Args>
inline void spdlog::logger::debug(const wchar_t *fmt, const Args &... args)
{
    log(level::debug, fmt, args...);
}

template<typename... Args>
inline void spdlog::logger::info(const wchar_t *fmt, const Args &... args)
{
    log(level::info, fmt, args...);
}

template<typename... Args>
inline void spdlog::logger::warn(const wchar_t *fmt, const Args &... args)
{
    log(level::warn, fmt, args...);
}

template<typename... Args>
inline void spdlog::logger::error(const wchar_t *fmt, const Args &... args)
{
    log(level::err, fmt, args...);
}

template<typename... Args>
inline void spdlog::logger::critical(const wchar_t *fmt, const Args &... args)
{
    log(level::critical, fmt, args...);
}

#endif // SPDLOG_WCHAR_TO_UTF8_SUPPORT

//
// name and level
//
inline const std::string &spdlog::logger::name() const
{
    return name_;
}

inline void spdlog::logger::set_level(spdlog::level::level_enum log_level)
{
    level_.store(log_level);
}

inline void spdlog::logger::set_error_handler(spdlog::log_err_handler err_handler)
{
    err_handler_ = std::move(err_handler);
}

inline spdlog::log_err_handler spdlog::logger::error_handler()
{
    return err_handler_;
}

inline void spdlog::logger::flush()
{
    try
    {
        flush_();
    }
    SPDLOG_CATCH_AND_HANDLE
}

inline void spdlog::logger::flush_on(level::level_enum log_level)
{
    flush_level_.store(log_level);
}

inline spdlog::level::level_enum spdlog::logger::flush_level() const
{
    return static_cast<spdlog::level::level_enum>(flush_level_.load(std::memory_order_relaxed));
}

inline bool spdlog::logger::should_flush_(const details::log_msg &msg)
{
    auto flush_level = flush_level_.load(std::memory_order_relaxed);
    return (msg.level >= flush_level) && (msg.level != level::off);
}

inline spdlog::level::level_enum spdlog::logger::level() const
{
    return static_cast<spdlog::level::level_enum>(level_.load(std::memory_order_relaxed));
}

inline bool spdlog::logger::should_log(spdlog::level::level_enum msg_level) const
{
    return msg_level >= level_.load(std::memory_order_relaxed);
}

//
// protected virtual called at end of each user log call (if enabled) by the
// line_logger
//
inline void spdlog::logger::sink_it_(details::log_msg &msg)
{
#if defined(SPDLOG_ENABLE_MESSAGE_COUNTER)
    incr_msg_counter_(msg);
#endif
    for (auto &sink : sinks_)
    {
        if (sink->should_log(msg.level))
        {
            sink->log(msg);
        }
    }

    if (should_flush_(msg))
    {
        flush();
    }
}

inline void spdlog::logger::flush_()
{
    for (auto &sink : sinks_)
    {
        sink->flush();
    }
}

inline void spdlog::logger::default_err_handler_(const std::string &msg)
{
    auto now = time(nullptr);
    if (now - last_err_time_ < 60)
    {
        return;
    }
    last_err_time_ = now;
    auto tm_time = details::os::localtime(now);
    char date_buf[100];
    std::strftime(date_buf, sizeof(date_buf), "%Y-%m-%d %H:%M:%S", &tm_time);
    fmt::print(stderr, "[*** LOG ERROR ***] [{}] [{}] {}\n", date_buf, name(), msg);
}

inline void spdlog::logger::incr_msg_counter_(details::log_msg &msg)
{
    msg.msg_id = msg_counter_.fetch_add(1, std::memory_order_relaxed);
}

inline const std::vector<spdlog::sink_ptr> &spdlog::logger::sinks() const
{
    return sinks_;
}

inline std::vector<spdlog::sink_ptr> &spdlog::logger::sinks()
{
    return sinks_;
}

inline std::shared_ptr<spdlog::logger> spdlog::logger::clone(std::string logger_name)
{
    auto cloned = std::make_shared<spdlog::logger>(std::move(logger_name), sinks_.begin(), sinks_.end());
    cloned->set_level(this->level());
    cloned->flush_on(this->flush_level());
    cloned->set_error_handler(this->error_handler());
    return cloned;
}
