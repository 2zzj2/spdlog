#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <map>
#include <string>
#include <memory>

namespace business_log {

enum class BusinessType {
    ScreenRecording,
    DesktopOpen,
    KeyboardRecording,
    AudioRecording
};

class BusinessLogger {
public:
    static BusinessLogger& instance() {
        static BusinessLogger logger;
        return logger;
    }

    void init(const std::string& log_dir = "logs") {
        log_dir_ = log_dir;
        spdlog::drop("screen_recording");
        spdlog::drop("desktop_open");
        spdlog::drop("keyboard_recording");
        spdlog::drop("audio_recording");
        setup_logger(BusinessType::ScreenRecording, "screen_recording");
        setup_logger(BusinessType::DesktopOpen, "desktop_open");
        setup_logger(BusinessType::KeyboardRecording, "keyboard_recording");
        setup_logger(BusinessType::AudioRecording, "audio_recording");
    }

    template<typename... Args>
    void log(BusinessType type, spdlog::level::level_enum level, spdlog::format_string_t<Args...> fmt, Args&&... args) {
        auto it = loggers_.find(type);
        if (it != loggers_.end()) {
            it->second->log(level, fmt, std::forward<Args>(args)...);
        }
    }

    template<typename... Args>
    void info(BusinessType type, spdlog::format_string_t<Args...> fmt, Args&&... args) {
        log(type, spdlog::level::info, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warn(BusinessType type, spdlog::format_string_t<Args...> fmt, Args&&... args) {
        log(type, spdlog::level::warn, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(BusinessType type, spdlog::format_string_t<Args...> fmt, Args&&... args) {
        log(type, spdlog::level::err, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void debug(BusinessType type, spdlog::format_string_t<Args...> fmt, Args&&... args) {
        log(type, spdlog::level::debug, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void critical(BusinessType type, spdlog::format_string_t<Args...> fmt, Args&&... args) {
        log(type, spdlog::level::critical, fmt, std::forward<Args>(args)...);
    }

private:
    BusinessLogger() = default;
    ~BusinessLogger() = default;
    BusinessLogger(const BusinessLogger&) = delete;
    BusinessLogger& operator=(const BusinessLogger&) = delete;

    void setup_logger(BusinessType type, const std::string& name) {
        std::string log_path = log_dir_ + "/" + name + ".log";
        size_t max_size = 30 * 1024 * 1024;
        size_t max_files = 3;
        
        auto existing_logger = spdlog::get(name);
        if (existing_logger) {
            loggers_[type] = existing_logger;
            return;
        }
        
        auto logger = spdlog::rotating_logger_mt(name, log_path, max_size, max_files);
        logger->set_level(spdlog::level::debug);
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");
        logger->flush_on(spdlog::level::info);
        loggers_[type] = logger;
    }

    std::string log_dir_;
    std::map<BusinessType, std::shared_ptr<spdlog::logger>> loggers_;
};

}
