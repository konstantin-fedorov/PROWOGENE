#include "logger.h"

#include <chrono>
#include <iostream>

namespace prowogene {

using std::cout;
using std::list;
using std::string;

static const int kDrawWidth = 40;
static const char kBorder = '#';
static const string kBorderStr = string(1, kBorder);

Logger::Logger() { }

Logger::Logger(LogLevel lvl_stdout) {
    log_level_stdout_ = lvl_stdout;
}

Logger::Logger(LogLevel lvl_file, const std::string& file)
        : log_level_file_(lvl_file) {
    log_file_.open(file);
    if (!log_file_.is_open()) {
        log_level_file_ = LogLevel::Silent;
    }
}

Logger::Logger(LogLevel lvl_stdout, LogLevel lvl_file, const std::string& file)
        : Logger(lvl_file, file) {
    log_level_stdout_ = lvl_stdout;
}

Logger::~Logger() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

void Logger::ModuleStarted(const IModule* module) {
    const auto time_now = std::chrono::high_resolution_clock::now();
    start_time_ns_ = time_now.time_since_epoch().count();
    const string module_name = module ? module->GetName() : "???";
    const string msg = "Module started: " + module_name;
    Log({LogLevel::Standard, LogLevel::Full}, msg);
}

void Logger::ModuleEnded(const IModule* module) {
    const auto time_now = std::chrono::high_resolution_clock::now();
    const auto ns_now = time_now.time_since_epoch().count();
    const long long duration_ns = ns_now - start_time_ns_;
    const double seconds = static_cast<double>(duration_ns) / 1000000000.0f;
    const string module_name = module ? module->GetName() : "???";
    string msg = "Module ended:   " + module_name + "\n";
    msg += "    Time elapsed: " + std::to_string(seconds) + " sec.";
    Log({ LogLevel::Standard, LogLevel::Full }, msg);
}

void Logger::LogError(const IModule* module, const string& msg) {
    const list<LogLevel> levels = {
        LogLevel::Critical,
        LogLevel::Standard,
        LogLevel::Full
    };
    const string module_name = module ? module->GetName() : "???";
    const string module_status = module ? module->GetStatus() : "???";
    const string error = module_name + " module error: ";
    if (!msg.size()) {
        Log(levels, error + module_status);
    } else {
        Log(levels, error + msg);
    }
}

void Logger::LogMessage(const std::string& msg) {
    const list<LogLevel> levels = {
        LogLevel::Standard,
        LogLevel::Full
    };
    Log(levels, msg);
}

void Logger::DrawPipeline(const list<IModule*>& modules) {
    Log({ LogLevel::Full }, "Pipeline:");
    const string hor_line = string(kDrawWidth, kBorder) + "\n";
    const string empty_line =    CreateMessageLine(string(kDrawWidth - 3, ' '));
    const string settings_line = CreateMessageLine("Settings:");
    const string data_line =     CreateMessageLine("Data:");
    const string arrow_tail =    CreateTitle("||");
    const string arrow_pointer = CreateTitle("\\/");
    for (auto& module : modules) {
        if (module) {
            const auto settings = module->GetNeededSettings();

            string message = "";
            message.reserve((settings.size() + 10) * (kDrawWidth + 1));
            message += hor_line;
            message += empty_line;
            message += CreateMessageLine("Module: " + module->GetName());
            message += empty_line;
            message += settings_line;
            for (const auto& item : settings) {
                message += CreateMessageLine("+ " + item);
            }
            message += empty_line;
            message += data_line;
            message += empty_line;
            message += hor_line;
            if (module != modules.back()) {
                message += arrow_tail;
                message += arrow_pointer;
            }
            message.pop_back();
            Log({ LogLevel::Full }, message);
        }
    }
    Log({ LogLevel::Full }, "\n");
}

void Logger::Log(const list<LogLevel>& levels, const string& msg) {
    for (const auto level : levels) {
        if (level == log_level_stdout_) {
            cout << msg << "\n";
            break;
        }
    }
    for (const auto level : levels) {
        if (level == log_level_file_) {
            if (log_file_.is_open()) {
                log_file_ << msg << "\n";
                log_file_.flush();
            }
            break;
        }
    }
}

string Logger::CreateMessageLine(const string& msg) {
    const int msg_length = static_cast<int>(msg.length());
    const int right_indent = kDrawWidth - msg_length - 3;
    return kBorderStr + " " + msg + string(right_indent, ' ') +
           kBorderStr + "\n";
}

string Logger::CreateTitle(const string& msg) {
    const int msg_length = static_cast<int>(msg.length());
    const int left_indent = kDrawWidth / 2 - msg_length / 2;
    return string(left_indent, ' ') + msg + "\n";
}

} // namespace prowogene
