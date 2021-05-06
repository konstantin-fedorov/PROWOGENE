/*////////////////////////////////////////////////////////////////////////////

  _____   ______  _____  _  _  _  _____   ______ _______ __   _ _______
 |_____] |_____/ |     | |  |  | |     | |  ____ |______ | \  | |______
 |       |    \_ |_____| |__|__| |_____| |_____| |______ |  \_| |______


MIT License

Copyright (c) 2019-2020 Konstantin Fedorov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

////////////////////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <string.h>

#include <generator.h>
#include <modules/basis.h>
#include <modules/cliff.h>
#include <modules/item.h>
#include <modules/location.h>
#include <modules/model.h>
#include <modules/mountain.h>
#include <modules/post_process.h>
#include <modules/river.h>
#include <modules/texture.h>
#include <modules/water.h>

// using namespace is bad practice, but readability is more significant here.
using namespace prowogene;
using namespace prowogene::modules;
using namespace prowogene::utils;


int main(int argc, const char *argv[]) {
    // Pass 
    if (argc != 2) {
        printf("No config file passed to generator.\n");
        return 1;
    }
    std::string config_filename = argv[1];

    // Create modules instances.
    BasisModule       basis_module;
    MountainModule    mountain_module;
    CliffModule       cliff_module;
    WaterModule       water_module;
    RiverModule       river_module;
    PostProcessModule post_process_module;
    LocationModule    locatoin_module;
    ItemModule        item_module;
    TextureModule     texture_module;
    ModelModule       model_module;

    // Create settings instances.
    BasisSettings       basis_settings;
    CliffSettings       cliff_settings;
    MountainSettings    mountain_settings;
    GeneralSettings     general_settings;
    WaterSettings       water_settings;
    RiverSettings       river_settings;
    SystemSettings      system_settings;
    PostprocessSettings post_process_settings;
    TextureSettings     texture_settings;
    NamesSettings       names_settings;
    LocationSettings    location_settings;
    ItemSettings        item_settings;
    ModelSettings       model_settings;

    // Data storage inctance.
    Storage storage;

    // Storage can hold any data type and allows to get it by std::string key.
    storage.AddData(kStorageBeachLevel,   new float(0.0f));
    storage.AddData(kStorageHeightMap,    new Array2D<float>());
    storage.AddData(kStorageLocationMap,  new Array2D<Location>());
    storage.AddData(kStorageMountainMask, new Array2D<float>());
    storage.AddData(kStorageRiverMask,    new Array2D<float>());
    storage.AddData(kStorageSeaLevel,     new float(0.0f));
    storage.AddData(kStorageModelIO,      new ModelIO());
    storage.AddData(kStorageImageIO,      new ImageIO());

    // Generator instance allows to manage modules, settings and storage.
    Generator generator;

    // Now any data can be attached to generator.

    // There is single storage in generator.
    generator.SetStorage(&storage);

    // Logger can be also attached to generator.
    Logger logger(LogLevel::Standard, LogLevel::Full, "log.txt");
    generator.SetLogger(nullptr);// &logger);

    // There is unlimited count of modules that can be attached to generator's
    // pipeline. Order of addition is significant and describes steps order.
    generator.PushBackModule(&basis_module);
    generator.PushBackModule(&cliff_module);
    generator.PushBackModule(&mountain_module);
    generator.PushBackModule(&water_module);
    generator.PushBackModule(&river_module);
    generator.PushBackModule(&post_process_module);
    generator.PushBackModule(&locatoin_module);
    generator.PushBackModule(&item_module);
    generator.PushBackModule(&texture_module);
    generator.PushBackModule(&model_module);

    // There is unlimited count of settings that can be attached to generator.
    // Order of addition is not significant because all settings can be taken
    // by std::string key.
    generator.AddSettings(&basis_settings);
    generator.AddSettings(&cliff_settings);
    generator.AddSettings(&mountain_settings);
    generator.AddSettings(&general_settings);
    generator.AddSettings(&water_settings);
    generator.AddSettings(&river_settings);
    generator.AddSettings(&system_settings);
    generator.AddSettings(&post_process_settings);
    generator.AddSettings(&texture_settings);
    generator.AddSettings(&names_settings);
    generator.AddSettings(&location_settings);
    generator.AddSettings(&item_settings);
    generator.AddSettings(&model_settings);
    // After addition generator can fill settings with values from file. All
    // values will be overwritten.
    generator.LoadSettings(config_filename);
    // After that settings can be overwritten using API. In that example
    // there is no need to change them.

    // Launch generator pipeline.
    bool success = generator.Generate();

    // Free data from storage manually.
    storage.RemoveData<float>(kStorageBeachLevel);
    storage.RemoveData<Array2D<float> >(kStorageHeightMap);
    storage.RemoveData<Array2D<Location> >(kStorageLocationMap);
    storage.RemoveData<Array2D<float> >(kStorageMountainMask);
    storage.RemoveData<Array2D<float> >(kStorageRiverMask);
    storage.RemoveData<float>(kStorageSeaLevel);
    storage.RemoveData<ModelIO>(kStorageModelIO);
    storage.RemoveData<ImageIO>(kStorageImageIO);

    return !success;
}
