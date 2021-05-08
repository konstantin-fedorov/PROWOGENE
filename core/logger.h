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


/** @brief Base instance for writing logs to some output. */
class ILogWriter {
 public:
    /** Log message.
    @param [in] msg - Message text. */
    virtual void LogMessage(const std::string& msg) = 0;

    /** Log level for stdout. */
    LogLevel log_level_ = LogLevel::Silent;
};


/** @brief stdout-oriented log writer. */
class StdoutLogWriter : public ILogWriter {
 public:
    /** @copydoc ILogWriter::Deserialize */
    void LogMessage(const std::string& msg) override;
};


/** @brief File-oriented log writer. */
class FileLogWriter : public ILogWriter {
 public:
    /** Constructor.
    @param [in] file - Log filename. */
    FileLogWriter(const std::string& filename);

    /** Destructor. */
    ~FileLogWriter();

    /** @copydoc ILogWriter::Deserialize */
    void LogMessage(const std::string& msg) override;

 private:
     /** File for log. */
     std::ofstream log_file_;
};


/** @brief Class for logging messages.

Provides logging messages to stdout and/or to file. */
class Logger {
 public:
    /** Add onemore log writter to process all logs. */
    void AddLogWriter(ILogWriter* writer);

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
    virtual void Log(const std::list<LogLevel>& levels, const std::string& msg);

    /** Create text line inside pipeline node.
    @param [in] msg - Message text. */
    static std::string CreateMessageLine(const std::string& msg);

    /** Create text line aliged to center.
    @param [in] msg - Message text. */
    static std::string CreateTitle(const std::string& msg);

    /** Log writers storage. */
    std::list<ILogWriter*> log_writers_;
    /** Last started module's time since Jan 1, 1970 in nanoseconds. */
    long long start_time_ns_ = 0;
};

} // namespace prowogene

#endif // PROWOGENE_CORE_LOGGER_H_
