#ifndef PROWOGENE_CORE_LOGGER_H_
#define PROWOGENE_CORE_LOGGER_H_

#include <fstream>

#include "module_interface.h"

namespace prowogene {

/** @brief Mode of logging. */
enum class LogLevel {
    /** No logs. */
    Silent,
    /** Log only errors. */
    Critical,
    /** Log all messages. */
    Standard,
    /** Draw pipeline and log all messages. */
    Full
};


/** @brief Class for logging messages.

Provides logging messages to stdout and/or to file. */
class Logger {
 public:
    /** Default constructor. */
    Logger();

    /** Constructor.
    @param [in] lvl_stdout - Log level for stdout. */
    Logger(LogLevel lvl_stdout);

    /** Constructor.
    @param [in] lvl_file - Log level for file.
    @param [in] file     - Log filename. */
    Logger(LogLevel lvl_file, const std::string& file);

    /** Constructor.
    @param [in] lvl_stdout - Log level for stdout.
    @param [in] lvl_file   - Log level for file.
    @param [in] file       - Log filename. */
    Logger(LogLevel lvl_stdout, LogLevel lvl_file, const std::string& file);

    /** Destructor. */
    ~Logger();

    /** Log that module started processing.
    @param [in] module - Module that started processing. */
    void ModuleStarted(const IModule* module);
    
    /** Log that module ended processing.
    @param [in] module - Module that ended processing. */
    void ModuleEnded(const IModule* module);

    /** Log error from module.
    @param [in] module - Module there error occured.
    @param [in] msg    - Error message text. */
    void LogError(const IModule* module, const std::string& msg = "");

    /** Log message.
    @param [in] msg - Message text. */
    void LogMessage(const std::string& msg);

    /** Draw modules pipeline.
    @param [in] modules - Modules pipeline. */
    void DrawPipeline(const std::list<IModule*>& modules);

 protected:
    /** Log message to specified outputs that supports selected levels.
    @param [in] levels - Levels in which message must be logged.
    @param [in] msg    - Message text. */
    void Log(const std::list<LogLevel>& levels, const std::string& msg);

    /** Create text line inside pipeline node.
    @param [in] msg - Message text. */
    static std::string CreateMessageLine(const std::string& msg);

    /** Create text line aliged to center.
    @param [in] msg - Message text. */
    static std::string CreateTitle(const std::string& msg);

    /** Log level for stdout. */
    LogLevel log_level_stdout_ = LogLevel::Standard;
    /** Log level for file. */
    LogLevel log_level_file_ = LogLevel::Silent;
    /** File for log. */
    std::ofstream log_file_;
    /** Last started module's time since Jan 1, 1970 in nanoseconds. */
    long long start_time_ns_ = 0;
};

} // namespace prowogene

#endif // PROWOGENE_CORE_LOGGER_H_
