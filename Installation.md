
## Installing the precompiled plugin (Blueprint and C++ projects)
Get the [latest release](https://github.com/BenPyton/ProceduralDungeon/releases) of the precompiled plugin compatible with your Unreal version.

### In the engine
You have to extract the plugin inside your Unreal engine `Plugins` folder.\
For instance: `C:\Program Files\Epic Games\UE_5.2\Engine\Plugins`

### In your project
You have to extract the plugin in the `Plugins` folder of your project.\
For instance: `D:\PathToYourProject\Plugins`

## Installing the sources of the plugin (C++ projects only)
Alternatively, you can clone or download the plugin sources in your project `Plugins` folder,\
then regenerate your project's .sln and build from visual studio. 

---

## For older versions of the plugin (before v2.0.0)
### Blueprint only projects
__Before plugin version 1.2.1__:\
Upgrade your project to a C++ project and follow the instruction for C++ project below, or you wont be able to package your game.\
(To do that, download Visual Studio 2019 and create a new C++ class from the Unreal Editor)

Since plugin version 1.2.1:\
You have to extract the plugin inside your Unreal engine `Plugins` folder.\
For instance: `C:\Program Files\Epic Games\UE_4.26\Engine\Plugins`

### C++ projects
Since the plugin version 1.2.1, you can install the plugin in the engine like for blueprint projects.\
You can also extract the plugin in the `Plugins` folder of your project.\
For instance: `D:\PathToYourProject\Plugins`