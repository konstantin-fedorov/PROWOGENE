#include "generator.h"

#include <chrono>
#include <fstream>

#include "utils/array2d.h"

namespace prowogene {

using std::list;
using std::string;
using utils::Array2D;
using utils::JsonValue;
using utils::JsonObject;
using utils::JsonType;
using utils::InputString;


// Singleton dummy logger which don't do anything.
class DummyLogger : public Logger {
public:
    DummyLogger(DummyLogger& other) = delete;
    void operator=(const DummyLogger&) = delete;

    static DummyLogger* GetInstance() {
        if (!instance_) {
            instance_ = new DummyLogger();
        }
        return instance_;
    }

protected:
    static DummyLogger* instance_;

    DummyLogger() { };
    void Log(const std::list<LogLevel>& levels, const std::string& msg) override { }
};
DummyLogger* DummyLogger::instance_ = nullptr;


prowogene::Generator::Generator() {
    logger_ = DummyLogger::GetInstance();
}

void Generator::Clear() {
    for (auto& settings : settings_) {
        if (settings.second.settings) {
            delete settings.second.settings;
        }
    }
    settings_.clear();

    for (auto& module : modules_) {
        if (module) {
            delete module;
        }
    }
    modules_.clear();

    if (logger_) {
        delete logger_;
        logger_ = nullptr;
    }
}

void Generator::SetLogger(Logger* logger) {
    if (logger)
        logger_ = logger;
}

void Generator::PushBackModule(IModule* module) {
    modules_.push_back(module);
}

void Generator::AddSettings(ISettings* settings) {
    settings_[settings->GetName()] = { settings, false };

}

void Generator::LoadSettings(const string& filename) {
    JsonValue json_file;
    json_file.Parse(filename, InputString::FILENAME);
    if (json_file.GetType() != JsonType::OBJECT) {
        return;
    }

    JsonObject config = json_file;
    for (auto& sub_config : config) {
        auto founded = settings_.find(sub_config.first);
        if (founded != settings_.end()) {
            founded->second.settings->Deserialize(sub_config.second);
        }
    }
}

void Generator::SaveSettings(const string& filename, bool pretty) const {
    JsonObject config;
    for (auto& settings : settings_) {
        config[settings.first] = settings.second.settings->Serialize();
    }
    JsonValue json_file = config;
    string str = json_file.ToString();
    std::ofstream out_file(filename);
    if (out_file.is_open()) {
        out_file.write(str.c_str(), str.size());
    }
}

bool Generator::Generate() {
    logger_->DrawPipeline(modules_);

    logger_->LogMessage("Generation started.");
    auto time_beg = std::chrono::high_resolution_clock::now();

    for (auto& module : modules_) {
        if (!module) {
            logger_->LogError(module, "Module didn't found.");
            return false;
        }

        logger_->ModuleStarted(module);

        if (!ApplySettings(module)) {
            return false;
        }
        module->Init();
        bool success = module->Process();
        if (!success) {
            logger_->LogError(module);
            module->Deinit();
            return false;
        }
        module->Deinit();

        logger_->ModuleEnded(module);
    }

    logger_->LogMessage("Generation ended.");
    auto time_end = std::chrono::high_resolution_clock::now();
    double seconds = (time_end - time_beg).count() / 1000000000.0;
    logger_->LogMessage("Time elapsed: " + std::to_string(seconds) + " sec.");
    return true;
}

bool Generator::ApplySettings(IModule* module) {
    const list<string> settings_keys = module->GetNeededSettings();
    for (const auto& i : settings_keys) {
        const auto needed = settings_.find(i);
        if (needed == settings_.end()) {
            const string msg = "No settings with key '" + i + "'.";
            logger_->LogError(module, msg);
            return false;
        }
        LazySettingsCheck& settings_to_check = needed->second;
        ISettings* set = settings_to_check.settings;
        if (!settings_to_check.was_checked) {
            const bool success = settings_to_check.settings->IsCorrect();
            if (!success) {
                const string msg = "Settings '" + set->GetName() + "' are incorrect.";
                logger_->LogError(module, msg);
                return false;
            }
        }
        settings_to_check.was_checked = true;
        module->ApplySettings(set);
    }
    return true;
}

} // namespace prowogene
