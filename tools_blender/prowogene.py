import bpy
import dataclasses
import enum
import json
import os
import pathlib
import subprocess


# Classes for check configs, chosing import mode, converting serialized
# data and another work with config files.


class ImportMode(enum.Enum):
    '''Mode of possible import.

    Attributes:
        NONE: Import is impossible.
        CHUNK: Import landscape as number of submeshes with objects.
        COMPLEX: Import landscape as one mesh with objects.
    '''
    NONE = 0
    CHUNK = 1
    COMPLEX = 2


@dataclasses.dataclass
class ItemInfo:
    '''Information about object for import.

    Attributes:
        model: Path to the model file.
        texture: Path to the texture file.
        normal: Path to the normal map file.
        x: X coordinate of object.
        y: Y coordinate of object.
        z: Z coordinate of object.
        angle: Object's rotation angle in radians.
    '''
    model = ''
    texture = ''
    normal = ''
    x = 0.0
    y = 0.0
    z = 0.0
    angle = 0.0


@dataclasses.dataclass
class ImportInfo:
    '''Information about importing objects.

    Attributes:
        mode: Prefered import mode.
        water_level: Height of location's waterline.
        objects: Objects to import.
    '''
    mode = ImportMode.NONE
    water_level = 0.0
    objects = []


class ConfigWorker:
    '''Class for check configs and extract data from them.

    Attributes:
        water_level: height of location's waterline.
    '''
    def __init__(self):
        '''Inits ConfigWorker.'''
        self.water_level = 0.0

    @staticmethod
    def FileExists(filename):
        '''Check file exists or not.

        Args:
            filename: Name of file to check. Can be absolute or relative path.

        Returns:
            True if file exists
            False if file does not exists.
        '''
        if not os.path.exists(os.path.abspath(filename)):
            return False
        return True

    @staticmethod
    def FileExistsOrEmpty(filename):
        '''Check file exists or not if filename is not empty.

        Args:
            filename: Name of file to check. Can be absolute or relative path.

        Returns:
            True if file exists or filename is empty
            False if file does not exists.
        '''
        if filename == '':
            return True
        return ConfigWorker.FileExists(filename)

    @staticmethod
    def ConfigContains(config, keys):
        '''Check that config contains all given keys.

        Args:
            config: Config where keys will be searched.
            keys: List of keys to check.

        Returns:
            True if all given keys was found in config.
            False if one or more keys wasn't found in config.
        '''
        for key in keys:
            if key not in config:
                return False
        return True

    @staticmethod
    def GetImportConfigName(settings_path):
        '''Find output config name in generator settings.

        Args:
            settings_path: Path to generator settings.

        Returns:
            Import config name or '' if errors occured.
        '''
        try:
            with open(settings_path, 'r') as json_file:
                config = json.load(json_file)
            return config['item']['config']['file']
        except ValueError:
            return ''

    def __CheckItem(self, item_config):
        '''Check config with import info for single 3D model item.
        Also checks that 3D model file exists.

        Args:
            item_config: Config to check.

        Returns:
            True if config is valid and 3D model file exists.
            False if config is invalid and/or 3D model file doesn't exists.
        '''
        if not ConfigWorker.ConfigContains(item_config,
                                           ['angle', 'file', 'id',
                                            'x', 'y', 'z']):
            return False
        if not ConfigWorker.FileExists(item_config['file']):
            return False
        return True

    def __CheckChunk(self, config):
        '''Check config with import info for ImportMode.CHUNK mode.
        Also checks that attributes with filenames points at existing files.

        Args:
            config: Config to check.

        Returns:
            True if config is valid and all pointed files are missed.
            False if config is invalid and/or some pointed files are missed.
        '''
        try:
            chunk_config = config['chunks']
            if not ConfigWorker.ConfigContains(chunk_config,
                                               ['count_x', 'count_y', 'data']):
                return False

            if (chunk_config['count_x'] == 0 or
                    chunk_config['count_y'] == 0 or
                    len(chunk_config['data']) == 0):
                print('No chunk data available')
                return False

            data_list = chunk_config['data']
            data_count = 0
            for data in data_list:
                info = data['info']

                if not ConfigWorker.ConfigContains(info, ['model', 'x', 'y']):
                    return False
                if not ConfigWorker.FileExists(info['model']):
                    return False
                if not ConfigWorker.FileExistsOrEmpty(info['normal']):
                    return False
                if not ConfigWorker.FileExistsOrEmpty(info['texture']):
                    return False

                items = data['items']
                for item in items:
                    if not self.__CheckItem(item):
                        return False
                data_count += 1
            data_count_ref = chunk_config['count_x'] * chunk_config['count_y']
            if not data_count == data_count_ref:
                return False
        except ValueError:
            return False
        return True

    def __CheckComplex(self, config):
        '''Check config with import info for ImportMode.COMPLEX mode.
        Also checks that attributes with filenames points at existing files.

        Args:
            config: Config to check.

        Returns:
            True if config is valid and all pointed files are missed.
            False if config is invalid and/or some pointed files are missed.
        '''
        try:
            complex_info = config['complex']
            if not ConfigWorker.FileExists(complex_info['model']):
                return False
            if not ConfigWorker.FileExistsOrEmpty(complex_info['normal']):
                return False
            if not ConfigWorker.FileExistsOrEmpty(complex_info['texture']):
                return False
            items = complex_info['items']
            for item in items:
                if not self.__CheckItem(item):
                    return False
        except ValueError:
            return False
        return True

    def __GetItemInfo(self, config):
        '''Fills ItemInfo object with values according to config.

        Args:
            config: Valid config with info about external 3D model.

        Returns:
            ItemInfo filled with values from config.
        '''
        item_info = ItemInfo()
        item_info.model = os.path.abspath(config['file'])
        item_info.x = config['x']
        item_info.y = config['y']
        item_info.z = config['z']
        item_info.angle = config['angle']
        return item_info

    def __GetObjectsChunks(self, config):
        '''Creates list of ItemInfo objects that contain information about
        generated chunk 3D models and external models placed on them.

        Args:
            config: Valid config with chunk import info.

        Returns:
            List of ItemInfo objects with both chunks and external models info.
        '''
        objects = []
        for chunk_config in config['data']:
            chunk_info = ItemInfo()
            chunk_model = chunk_config['info']
            chunk_info.model = os.path.abspath(chunk_model['model'])
            chunk_info.texture = chunk_model['texture']
            chunk_info.normal = chunk_model['normal']
            objects.append(chunk_info)

            for item in chunk_config['items']:
                item_info = self.__GetItemInfo(item)
                objects.append(item_info)
        return objects

    def __GetObjectsComplex(self, config):
        '''Creates list of ItemInfo objects that contain information about
        generated complex 3D model and external models placed on it.

        Args:
            config: Valid config with complex import info.

        Returns:
            List of ItemInfo objects with complex and external models info.
        '''
        objects = []
        complex_item = ItemInfo()
        complex_item.model = os.path.abspath(config['model'])
        complex_item.texture = config['texture']
        complex_item.normal = config['normal']
        objects.append(complex_item)

        for item in config['items']:
            item_info = self.__GetItemInfo(item)
            objects.append(item_info)
        return objects

    def GetWaterLevel(self, path):
        '''Find water level in import config.

        Args:
            path: Path to import config.

        Returns:
            Height of water level or 0.0 if errors occured.
        '''
        try:
            with open(path, 'r') as json_file:
                config = json.load(json_file)
            return config['water_level']
        except ValueError:
            return 0

    def GetImportMode(self, path):
        '''Checks import config and chose prefered import mode.

        Args:
            path: Path to import config.

        Returns:
            ImportMode with prefered mode.
        '''
        try:
            with open(path, 'r') as json_file:
                config = json.load(json_file)
        except ValueError:
            return ImportMode.NONE

        chunks_exists = self.__CheckChunk(config)
        complex_exists = self.__CheckComplex(config)
        if chunks_exists:
            return ImportMode.CHUNK
        elif complex_exists:
            return ImportMode.COMPLEX
        else:
            return ImportMode.NONE

    def GetImportInfo(self, path, mode):
        '''Get all import data from config according to given import mode.

        Args:
            path: Path to import config.
            mode: Mode for import. It can differs from GetImportMode().

        Returns:
            ImportMode with prefered mode.
        '''
        if mode == ImportMode.NONE:
            return ImportInfo()
        try:
            with open(path, 'r') as json_file:
                config = json.load(json_file)
            info = ImportInfo()
            info.mode = mode
            info.water_level = config['water_level']
            if info.mode == ImportMode.CHUNK:
                info.objects = self.__GetObjectsChunks(config['chunks'])
            else:
                info.objects = self.__GetObjectsComplex(config['complex'])
            return info
        except (ValueError, KeyError):
            return ImportInfo()


# Blender 3D addon logic


bl_info = {
    'name': 'PROWOGENE Toolkit',
    'description': ('Procedural landscape generator toolkit '
                    'for quick import of generator output'),
    'author': 'Konstantin Fedorov',
    'version': (1, 0, 0),
    'blender': (2, 80, 0),
    'location': '3D View > Tools',
    "support": "COMMUNITY",
    'category': 'All'
}
cache_filename = os.path.join(pathlib.Path.home(),
                              'prowogene_blender_cache.json')


class PG_PROWOGENE_Properties(bpy.types.PropertyGroup):
    '''Class for PROWOGENE add-on preferences.

    Attributes:
        application_path: Full path to PROWOGENE console application.
        settings_path: Full path to PROWOGENE settings .json file.
        working_directory: Directory according to which was filled PROWOGENE
            settings .json file.
    '''
    application_path: bpy.props.StringProperty(
        name='Application',
        description='Full path to PROWOGENE console application',
        default='',
        maxlen=1024,
    )
    settings_path: bpy.props.StringProperty(
        name='Settings file',
        description='Full path to PROWOGENE settings .json file',
        default='',
        maxlen=1024,
    )
    working_directory: bpy.props.StringProperty(
        name='Working directory',
        description=('Directory according to which was'
                     'filled PROWOGENE settings .json file.'),
        default='',
        maxlen=1024,
    )

    def SerializeFields(self):
        '''Saves current preference's state to cache.'''
        if (self.application_path == '' and
                self.settings_path == '' and
                self.working_directory == ''):
            return
        data = {}
        data['application'] = self.application_path
        data['settings'] = self.settings_path
        data['working_dir'] = self.working_directory
        try:
            with open(cache_filename, 'w') as outfile:
                json.dump(data, outfile)
        except IOError:
            pass

    def DeserializeFields(self):
        '''Load saved preference's state from cache.'''
        try:
            with open(cache_filename, 'r') as infile:
                data = json.load(infile)
            self.application_path = data['application']
            self.settings_path = data['settings']
            self.working_directory = data['working_dir']
        except IOError:
            pass


class OT_PROWOGENE_GenerateOperator(bpy.types.Operator):
    '''Create location files on hard drive using PROWOGENE application
    according to settings. All relative paths in settings file must be
    set relative to working directory. Status messages are available
    in \'Menu > Window > System Toggle Console\'
    '''

    bl_idname = 'prowogene.generate'
    bl_label = 'Generate Landscape'

    def execute(self, context):
        scene = context.scene
        tool = scene.prowogene_tool
        tool.SerializeFields()

        original_wd = os.getcwd()
        try:
            os.chdir(tool.working_directory)
        except (FileNotFoundError, AttributeError):
            return {'CANCELLED'}

        if not os.path.exists(tool.application_path):
            print('PROWOGENE ERROR: no app found')
            os.chdir(original_wd)
            return {'CANCELLED'}
        if not os.path.exists(tool.settings_path):
            print('PROWOGENE ERROR: no settings file found')
            os.chdir(original_wd)
            return {'CANCELLED'}
        result = subprocess.call([tool.application_path, tool.settings_path])
        if not result == 0:
            print('PROWOGENE ERROR: ',
                  tool.application_path, tool.settings_path,
                  'has finished with code', result)
            os.chdir(original_wd)
            return {'CANCELLED'}

        print('PROWOGENE generation successed')
        os.chdir(original_wd)
        return {'FINISHED'}


class OT_PROWOGENE_ImportOperator(bpy.types.Operator):
    '''Import data generated by PROWOGENE generator application. All
    relative paths in settings file must be set relative to working
    directory. Status messages are available in \'Menu > Window >
    System Toggle Console\'
    '''
    bl_idname = 'prowogene.import'
    bl_label = 'Import Landscape'

    def execute(self, context):
        scene = context.scene
        tool = scene.prowogene_tool
        tool.SerializeFields()

        original_wd = os.getcwd()
        try:
            os.chdir(tool.working_directory)
        except (FileNotFoundError, AttributeError):
            return {'CANCELLED'}

        cw = ConfigWorker()
        import_config = cw.GetImportConfigName(tool.settings_path)
        import_mode = cw.GetImportMode(import_config)
        if import_mode == ImportMode.NONE:
            print('PROWOGENE ERROR: Invalid import config')
            os.chdir(original_wd)
            return {'CANCELLED'}

        water_level = cw.GetWaterLevel(import_config)
        print('Water level:', water_level)

        import_info = cw.GetImportInfo(import_config, import_mode)
        for obj in import_info.objects:
            ext = pathlib.Path(obj.model).suffix
            if ext.lower() == '.obj':
                bpy.ops.import_scene.obj(filepath=os.path.abspath(obj.model))
            elif ext.lower() == '.fbx':
                bpy.ops.import_scene.fbx(filepath=os.path.abspath(obj.model))
            else:
                continue
            model = bpy.context.selected_objects[0]
            model.location = (obj.x, obj.y, obj.z)
        os.chdir(original_wd)
        return {'FINISHED'}


class OT_PROWOGENE_RecentValues(bpy.types.Operator):
    '''Load saved preference's state from cache.'''
    bl_idname = 'prowogene.recent_values'
    bl_label = 'Set As Recent Values'

    def execute(self, context):
        print(os.getcwd())
        scene = context.scene
        tool = scene.prowogene_tool
        tool.DeserializeFields()
        return {'FINISHED'}


class OBJECT_PT_PROWOGENE_Panel (bpy.types.Panel):
    '''Container for all add-on's controls.'''
    bl_idname = 'OBJECT_PT_prowogene_importer'
    bl_label = 'PROWOGENE'
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = 'Tool'
    bl_context = 'objectmode'

    @classmethod
    def poll(self, context):
        return context.object is not None

    def draw(self, context):
        layout = self.layout
        scene = context.scene
        tool = scene.prowogene_tool

        layout.prop(tool, 'application_path')
        layout.prop(tool, 'settings_path')
        layout.prop(tool, 'working_directory')
        layout.operator('prowogene.generate')
        layout.operator('prowogene.import')
        layout.operator('prowogene.recent_values')


# register and unregister add-on in Blender 3D application.


classes = (
    OBJECT_PT_PROWOGENE_Panel,
    OT_PROWOGENE_GenerateOperator,
    OT_PROWOGENE_ImportOperator,
    OT_PROWOGENE_RecentValues,
    PG_PROWOGENE_Properties
)


def register():
    '''Register add-on'''
    for cls in classes:
        bpy.utils.register_class(cls)
    bpy.types.Scene.prowogene_tool = (
        bpy.props.PointerProperty(type=PG_PROWOGENE_Properties))


def unregister():
    '''Unregister add-on'''
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)
    del bpy.types.Scene.prowogene_tool


if __name__ == '__main__':
    register()
