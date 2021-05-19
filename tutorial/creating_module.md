# Creating module tutorial

That tutorial will show how can be created custom module for PROWOGENE.

*Note: that tutorial includes creation of real module (integrated to core), so file paths and namespaces will be a little bit different from customers code. Real version of that module can be found in core/modules/cliff.h*

## Content
There are several steps of creating module. That tutorial shows how can be created module that creates cliff.

## Create new files
Firstly, create source files for new module. In PROWOGENE core is used next file structure:
1. ```cliff.h``` for module and it's settings interface.
2. ```cliff_module.cpp``` for module implementation.
3. ```cliff_settings.cpp``` for module's settings implementation.

## Creating interface

To create module, we need to create class inherited from ```IModule```. In file ```cliff.h``` we include ```module_interface.h```.

```
#include "module_interface.h"

namespace prowogene {
namespace modules {

class CliffModule : public IModule {
 public:
    void Process() override;
    std::list<std::string> GetNeededSettings() const override;
    void ApplySettings(ISettings* settings) override;
    std::string GetName() const override;
};

} // namespace modules
} // namespace prowogene
```

## Implement base module logic

Let's implement base logic of that module in ```cliff_module.cpp```. Now that module can be added to pipeline, but does not affect anything.
```
#include "cliff.h"

namespace prowogene {
namespace modules {

using std::list;
using std::string;

void CliffModule::Process() {

}

list<string> CliffModule::GetNeededSettings() const {
    return { };
}

void CliffModule::ApplySettings(ISettings* settings) {

}

string CliffModule::GetName() const {
    return "Cliff";
}

} // namespace modules
} // namespace prowogene
```

Every method description can be found in documentation.

## Add module to pipeline

To add module to generator's pipeline, we need to create instance and add it to pipeline (in ```console.cpp```):

```
CliffModule cliff_module;
generator.PushBackModule(&cliff_module);
```

Note that order of adding is significant, so place module when needed. Cliffs creation must be after basis surface creation but before mountain.

## Improoving module logic

### Manage access height map from storage
Cliff module will modify height map, which was modified by previously processed modules, so we need to have access to it. For quick and readable way to use it, store pointer to height map inside module:
```
public:
    utils::Array2D<float>* height_map_ = nullptr;
```

For link that pointer with pipeline's data we must add following code to the cliff module initialization in application which builds generation pipeline:
```
    Array2D<float>    height_map;
    cliff_module.height_map_ = &height_map;
```

### Process height map

Also we need access to general settings. Include headers with needed settings declarations (like ```general_settings.h```) in header file and add entity for settings storage in module declaration. Good idea to group all settings in protected unnamed struct:
```
 protected:
    struct {
        BasisSettings   basis;
        GeneralSettings general;
    } settings_;
```

To copy settings there, improove methods ```GetNeededSettings``` and ```ApplySettings```:
```
list<string> CliffModule::GetNeededSettings() const {
    return {
        settings_.basis.GetName(),
        settings_.general.GetName(),
        settings_.system.GetName()
    };
}

void CliffModule::ApplySettings(ISettings* settings) {
    CopySettings(settings, settings_.basis);
    CopySettings(settings, settings_.general);
    CopySettings(settings, settings_.system);
}
```

Now we are ready to start improoving ```Process``` method. There is no expected runtime problems or possible conflicts in this example. But in other modules there can be some troubles on the fly. To inform generator about problems, throw ```LogicException``` from this method:
```
#include <cmath>
#include "utils/array2d_tools.h"

using utils::Array2D;
using AT = utils::Array2DTools;

void CliffModule::Process() {
    const int size = settings_.general.size;
    const int octaves_count = 7;
    const int levels_count = 6;
    const int seed = 123;
    const float step = 1.0f / (levels_count - 1);
    const int threads = settings_.system.thread_count;

    if (!settings_.cliff.enabled) {
        return;
    }

    if (height_map_->Width() != size ||
            height_map_->Height() != size) {
        height_map_->Resize(size, size);
    }

    Array2D<float> cliff_map(size, size);
    AT::DiamondSquare(cliff_map, size, seed, octaves_count, 0.0f, 1.0f);

    Array2D<float> noise(size, size);
    AT::WhiteNoise(noise, size, seed);
    AT::ToRange(noise, 0.0f, step * settings_.cliff.grain, threads);

    for (auto& elem : cliff_map) {
        const float scaled = elem * levels_count - step / 2.0f;
        const int level_id = static_cast<int>(std::round(scaled));
        elem = level_id * step * settings_.basis.height;
    }
    AT::ApplyFilter(cliff_map, Operation::Add, cliff_map, noise, threads);
    AT::Smooth(cliff_map, 2, threads);

    AT::ApplyFilter(*height_map_, Operation::Min, *height_map_, cliff_map,
        threads);
}
```
Now that module modifies height map and can be easily integrated with generator. But that module can't be modified yet - ```octaves_count```, ```levels_count``` and ```seed``` values are hardcoded inside ```Process``` method. Also there is no opportunity to disable this module.

## Creating custom settings

For modules settings must be created own struct, inherited from ```ISettings``` in ```cliff.h```:
```
const std::string kConfigCliff = "cliff";

struct CliffSettings : ISettings {
    void Deserialize(utils::JsonObject config) override;
    utils::JsonObject Serialize() const override;
    void Check() const override;
    std::string GetName() const override;

    bool enabled = false;
    int  octaves = 3;
    int  levels = 8;
    int  seed = 0;
    float grain = 0.0325f;
};
```

After that, implement logic in ```cliff_settings.cpp```:
```
#include "cliff.h"

namespace prowogene {
namespace modules {

using std::string;
using utils::JsonObject;

static const string kEnabled = "enabled";
static const string kOctaves = "octaves";
static const string kLevels =  "levels";
static const string kSeed =    "seed";
static const string kGrain =   "grain";

void CliffSettings::Deserialize(JsonObject config) {
    enabled = config[kEnabled];
    octaves = config[kOctaves];
    levels =  config[kLevels];
    seed =    config[kSeed];
    grain =   config[kGrain];
}

JsonObject CliffSettings::Serialize() const {
    JsonObject config;
    config[kEnabled] = enabled;
    config[kOctaves] = octaves;
    config[kLevels] =  levels;
    config[kSeed] =    seed;
    config[kGrain] =   grain;
    return config;
}

string CliffSettings::GetName() const {
    return kConfigCliff;
}

void CliffSettings::Check() const {
    CheckCondition(octaves > 0, "octaves is less than 1");
    CheckCondition(levels > 1,  "levels is less than 2");
    CheckCondition(seed >= 0,   "seed is less than 1");
    CheckInRange(grain, 0.f, 1.f, "grain");
}

} // namespace modules
} // namespace prowogene
```

After that, add ```CliffSettings``` to ```CliffModule```:
```
struct {
    BasisSettings   basis;
    CliffSettings   cliff;
    GeneralSettings general;
} settings_;
```
Copy it from generator:
```

list<string> CliffModule::GetNeededSettings() const {
    return {
        settings_.basis.GetName(),
        settings_.cliff.GetName(),
        settings_.general.GetName(),
        settings_.system.GetName()
    };
}

void CliffModule::ApplySettings(ISettings* settings) {
    CopySettings(settings, settings_.basis);
    CopySettings(settings, settings_.cliff);
    CopySettings(settings, settings_.general);
    CopySettings(settings, settings_.system);
}
```
Add settings to pipeline (in ```console.cpp```):
```
CliffSettings cliff_settings;
generator.AddSettings(&cliff_settings);
```

And use it inside module:
```
void CliffModule::Process() {
    const int size = settings_.general.size;
    const int octaves_count = settings_.cliff.octaves;
    const int levels_count = settings_.cliff.levels;
    const int seed = settings_.cliff.seed;
    const float step = 1.0f / (levels_count - 1);
    const int threads = settings_.system.thread_count;

    if (!settings_.cliff.enabled) {
        return;
    }

    if (height_map_->Width() != size ||
            height_map_->Height() != size) {
        height_map_->Resize(size, size);
    }

    Array2D<float> cliff_map(size, size);
    AT::DiamondSquare(cliff_map, size, seed, octaves_count, 0.0f, 1.0f);

    Array2D<float> noise(size, size);
    AT::WhiteNoise(noise, size, seed);
    AT::ToRange(noise, 0.0f, step * settings_.cliff.grain, threads);

    for (auto& elem : cliff_map) {
        const float scaled = elem * levels_count - step / 2.0f;
        const int level_id = static_cast<int>(std::round(scaled));
        elem = level_id * step * settings_.basis.height;
    }
    AT::ApplyFilter(cliff_map, Operation::Add, cliff_map, noise, threads);
    AT::Smooth(cliff_map, 2, threads);

    AT::ApplyFilter(*height_map_, Operation::Min, *height_map_, cliff_map,
        threads);
}
```

### Adding config to JSON file
For set module preferences, you can simply add that JSON object to settings file. It must be placed in root object and have key value that returned from ```CliffSettings::GetName```. Example of that config: 

```
"cliff": {
    "enabled": true,
    "octaves": 3,
    "levels": 6,
    "seed": 4321,
    "grain": 0.0325
}
```
