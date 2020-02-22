# PROWOGENE Toolkit for Blender 3D 1.0.0

**PROWOGENE Toolkit** is lightweight add-on for Blender 3d application. This tool can be useful for quick landscape generation and it's import to Blender. Add-on has user-friendly interface and requires no programming skills or specific software except **Blender 3D** and **PROWOGENE**.

**Requirements:** OS Windows only (temporarily), Blender 2.80.

# Quick installation guide

1. Open **Blender 3D**.
2. **Top Bar Menu** > **Edit** > **Preferences**.
3. In opened **Blender Preferences** window, select section **Add-ons**.
4. Click the **Install...** button.
5. In opened **Blender File View** window navigate to directory, where `prowogene.py` is located. Select `prowogene.py` and click **Install Add-on** button.
6. Click **Enable Add-on** checkbox in **All: PROWOGENE Toolkit** line to enable add-on.
7. Addon successfully installed!

# Example of using

1. Open **Blender 3D**.
2. Left click on any object in **3D View**. Make sure that **Object Mode** is set.
3. Open object's information panel (Hotkey: **N**) and select **Tool** panel
4. Find section **PROWOGENE**.
5. Fill textbox named **Application** with PROWOGENE console application, for example `C:\Dev\PROWOGENE\build\console\Release\prowogene_console.exe`
6. Fill textbox named **Application** with generator settings file, for example `C:\Dev\PROWOGENE\data\generator_settings.json`. All relative paths in settings file must be set relative to working directory.
7. Fill textbox named **Application** with working directory for plugin, for example `C:\Dev\PROWOGENE\build\console`
8. Click **Generate Landscape** for generate local files using **PROWOGENE**. Note that it can take some time to complete.
9. Click **Import Landscape** for import local files using **PROWOGENE**. Note that it can take some time to complete.
10. Click **Set As Recent Values** for automatic filling textboxes with last used values.

# PROWOGENE Toolkit FAQ

**Q: Can't find PROWOGENE menu. Where is it located?**

A: Make sure that add-on is installed. Make sure that some object is set in **3D View**. Make sure that **Object Mode** is set. Open object's information panel (Hotkey: **N**) and select **Tool** panel there must be section **PROWOGENE**.

**Q: Buttons don't have any effect. What can be wrong?**

A: Check correctness of textboxes and check generator settings. Additional information about fail causes can be found in **Menu** > **Window** > **System Toggle Console**

**Q: Why only Windows is supported?**

A: Linux PROWOGENE is not tested yet. You need to compile **PROWOGENE console** from sources for other platform by yourself.

**Q: Can i use modified version of PROWOGENE console with that plugin?**

A: Yes, `prowogene.py` contains only code for **Blender 3D** add-on. You can create your own application based on **PROWOGENE console**, just make sure that settings filename is first command line arguement.