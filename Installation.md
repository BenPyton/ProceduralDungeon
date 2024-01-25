<details>
<summary>

# Installing on Windows
</summary>
<details>
<summary>

## Installing the precompiled plugin (Blueprint and C++ projects)
</summary>

Get the [latest release](https://github.com/BenPyton/ProceduralDungeon/releases) of the precompiled plugin compatible with your Unreal version.

### In the engine
You have to extract the plugin inside your Unreal engine `Plugins` folder.\
For instance: `C:\Program Files\Epic Games\UE_5.2\Engine\Plugins`

### In your project
You have to extract the plugin in the `Plugins` folder of your project.\
For instance: `D:\PathToYourProject\Plugins`

</details>
<details>
<summary>

## Installing the sources of the plugin (C++ projects only)
</summary>

Clone or download the plugin sources into your project's `Plugins` folder.\
Install Visual Studio with the "C++ for Video Games development" modules.\
Regenerate your project's `.sln` (right click on the `.uproject` and choose `Generate Visual Studio project files`).\
Open the `.sln` in Visual Studio and build it.

</details>
</details>
<details>
<summary>

# Installing on Mac OS
</summary>

## Build plugin from source (Blueprint and C++ projects)
Since I don't have a Mac to be able to compile the plugin myself, you will have to do it yourself.

Clone or download the plugin sources somewhere (referred later as `PluginPath`).\
Open the `.uplugin` file in the plugin's folder in a text editor and add `Mac` to the whitelisted platforms for both listed modules:
```
"WhitelistPlatforms": ["Win64", "Mac"]
```
Open the terminal and change the working directory to wherever `RunUAT.sh` is located.\
(e.g. `/Users/Shared/Epic Games/UE_5.3/Engine/Build/BatchFiles`)\
You may need to run this command to make sure the script is executable: `chmod +x RunUAT.sh`\
Then, run the following command (replace `PluginPath` with your plugin location and `OutputPath` with the location where you want the compiled binaries to be placed):
```
`./RunUAT.sh BuildPlugin -Plugin="/PluginPath/ProceduralDungeon.uplugin" -Package="/OutputPath" -Rocket -TargetPlatforms=Mac
```
Finally copy your `OutputPath` folder into your engine's or project's `Plugins` folder. 

</details>

---

<details>
<summary>

## For older versions of the plugin (before v2.0.0)
</summary>

<details>
<summary>

### Blueprint only projects
</summary>

**Before plugin version 1.2.1**:\
Upgrade your project to a C++ project and follow the instruction for C++ project below, or you wont be able to package your game.\
(To do that, download Visual Studio 2019 and create a new C++ class from the Unreal Editor)

**Since plugin version 1.2.1**:\
You have to extract the plugin inside your Unreal engine `Plugins` folder.\
For instance: `C:\Program Files\Epic Games\UE_4.26\Engine\Plugins`

</details>
<details>
<summary>

### C++ projects
</summary>

Since the plugin version 1.2.1, you can install the plugin in the engine like for blueprint projects.\
You can also extract the plugin in the `Plugins` folder of your project.\
For instance: `D:\PathToYourProject\Plugins`

</details>