---
title: Packaging
---

# Packaging your game with the plugin

The [releases](https://github.com/BenPyton/ProceduralDungeon/releases) I provide contains only pre-compiled binaries for the Windows 64bits platform.

You can use it directly to develop with the Unreal Engine Editor out of the box for both Blueprint-only and C++ projects.\
You can also package your game with no difficulties when targeting Windows platform only.

*(if you have issues packaging the plugin for Windows despite being installed in the engine, then try to place it in the `Plugins/Marketplace` folder of the engine instead)*

However, to develop and/or package your game for other platforms, you will need to compile the plugin yourself for those platforms.

## Packaging a C++ project

It's very simple as you just have to install the plugin in your project's `Plugins` folder (not in the engine).\
Then, the plugin will be automatically compiled alongside your project when you package your project.

If you want to install the plugin in the engine, though, you will have to follow the same steps as for a Blueprint-only project (see below).

## Packaging a Blueprint-only project

If you want to package your Blueprint-only project for **another platform** than Windows, then you will have to compile the plugin for the target platforms you want and then [install it like you did before](Installation.md).

To compile the plugin, you don't need to have an Unreal Engine project.\
However, you will need to have the necessary development tools for the targeted platforms.\
For example, you will need a MacOS to build for MacOS and iOS. For Android you will need to install the engine binaries from the Epic Games Launcher and follow the steps described in [their docs](https://dev.epicgames.com/documentation/en-us/unreal-engine/linux-development-requirements-for-unreal-engine?application_version=5.3) to setup the toolchains.

Then, you can place a copy of the plugin sources wherever you want.\
I will use `D:/ProceduralDungeon` as example (the `.uplugin` should be located in that directory).

After you've installed all the necessary toolchains and development, you will have to whitelist these platforms in the `.uplugin` file.\
To do that, open the `ProceduralDungeon.uplugin` in a text editor, and add the platforms you want in the `WhitelistPlatforms` of the modules (put only the ones you want):

```text title="ProceduralDungeon.uplugin"
"WhitelistPlatforms": [
	"Win64",
	"Android",
	"IOS",
	"Mac",
	"Linux"
]
```

Then, you will need to open a command prompt and run those commands (replace `RunUAT.bat` with `RunUAT.sh` if you are compiling *from* a Mac or Linux, **not** when you target those platforms from Windows).\
In my example I'm using `C:\Epic Games\UE_5.2` as the installation directory of the Unreal Engine. Replace it with your own installation path.

```cmd title="Command Prompt"
> cd "C:\Epic Games\UE_5.2\Engine\Build\BatchFiles"
> RunUAT.bat BuildPlugin -Plugin="D:\ProceduralDungeon\ProceduralDungeon.uplugin" -Package="D:\Output\ProceduralDungeon" -TargetPlatforms="Android"
```

For the example above, I've used `D:/Output/ProceduralDungeon` as the output directory where the pre-compiled plugin binaries are placed.
You can replace it with any other path you want.

Finally, copy the output directory where you've installed the plugin before.\
It is now containing the pre-compiled binaries for your target platforms and you should be able to package your Blueprint-only project for those platforms.
