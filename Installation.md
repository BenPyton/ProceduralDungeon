---
toc_min_heading_level: 2
toc_max_heading_level: 2
pagination_prev: guides/Home
pagination_next: guides/Plugin-Settings
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs> {/* [BEGIN] Windows | MacOS */}
<TabItem value="windows" label="Windows" default>
<Tabs> {/* [BEGIN] Blueprint | C++ */}
<TabItem value="bp" label="BP-only Project" default>
            
:::danger[Caution]
The pre-compiled binaries are available only for Windows platform.\
They can be used out-of-the-box to develop your game in the Editor and package your games for Windows.\
If you want to package your game for other platforms (MacOS, Linux, Android, iOS, ...) you will need to either:
- package the plugin yourself for the targeted platforms outside of your project and reinstall the plugin using the one you've packaged,
- **OR** convert your BP-only project into a C++ project, so the plugin will be compiled alongside your project for the targeted platforms.
:::

Download the [pre-compiled plugin](https://github.com/BenPyton/ProceduralDungeon/releases/latest) corresponding with your Unreal Engine version.\
For example, if you are using Unreal Engine 5.3.2 and you want plugin v3.3.1, you have to download the file `ProceduralDungeon_v3.3.1+UE5.3.zip`.

Once downloaded, extract the plugin inside your Unreal engine `Plugins` folder (UE4) or `Plugins/Marketplace` folder (UE5).\
For instance: 
- UE4: `C:\Program Files\Epic Games\UE_4.27\Engine\Plugins`
- UE5: `C:\Program Files\Epic Games\UE_5.2\Engine\Plugins\Marketplace`

:::info
The extracted `.uplugin` must be located directly inside the `ProceduralDungeon` directory.\
Here the example final path for UE5:\
`C:\Program Files\Epic Games\UE_5.2\Engine\Plugins\Marketplace\ProceduralDungeon\ProceduralDungeon.uplugin`
:::


</TabItem> {/* [END] Blueprint */}

<TabItem value="cpp" label="C++ Project">

### Installing the plugin in the Engine

You can follow the steps for Blueprint-only projects to install the plugin in the engine.

### Installing the plugin in the Project

:::info[PREREQUISITES]
You must have Visual Studio 2019 or 2022 installed with the `C++ for Video Games development` modules.
:::

Clone or download the plugin sources into your project's `Plugins` folder.\
The resulting path of the `.uplugin` file must be:\
`<your project path>\Plugins\ProceduralDungeon\ProceduralDungeon.uplugin`


Regenerate your project's solution (`.sln`) with a right click on your `.uproject` and choose `Generate Visual Studio project files`.\
This step is required whenever you modify (add/move/delete) source files in your project outside of the editor.

:::note
Make sure you right click on your `.uproject` located at the root directory of your project!\
It should not be confused with the `.uplugin` within the plugin folder!
:::

Finally, open the `.sln` in Visual Studio and build the editor by using the green triangle in the toolbar of Visual Studio.

</TabItem> {/* [END] C++ */}
</Tabs> {/* [END] Blueprint | C++ */}
</TabItem> {/* [END] Windows  */}

<TabItem value="macos" label="MacOS" default>
<Tabs> {/* [BEGIN] Blueprint | C++ */}
<TabItem value="bp" label="BP-only Project" default>

:::info[BAD APPLE!!!]
Unfortunately, I don't own a Mac computer and didn't successfully created a VM neither.\
So I am unable to compile for MacOS myself right now.\
*Blame Apple for their closed ecosystem policy, preventing cross-compilation!*
:::

You have 2 ways to use the plugin in a Blueprint-only project on MacOS:\
You can convert your Blueprint-only project into a C++ project, and follow the installation steps for C++ project (you can still continue to develop only with Blueprint).\
**OR** if you really want to keep your project Blueprint-only, you can package the plugin yourself for MacOS editor by following those steps:

- Clone or download the plugin sources somewhere (extract the zip file after download). I will refer later to this plugin's root path as `<plugin path>`.

- Open the `.uplugin` file (located directly inside `<plugin path>`) in a text editor and add `Mac` to the whitelisted platforms for both listed modules:
```
"WhitelistPlatforms": ["Win64", "Mac"]
```

- Open the terminal and change the working directory to wherever `RunUAT.sh` is located (using the `cd` command).\
An example where this file is located would be `/Users/Shared/Epic Games/UE_5.3/Engine/Build/BatchFiles`.

- You may need to run this command to make sure the script is executable: `chmod +x RunUAT.sh`\

- Then, run the following command (replace `<plugin path>` with your plugin location and `<output path>` with the location where you want the pre-compiled binaries to be generated):
```
`./RunUAT.sh BuildPlugin -Plugin="/PluginPath/ProceduralDungeon.uplugin" -Package="/OutputPath" -Rocket -TargetPlatforms=Mac
```

- Finally copy your `<output path>` folder into your engine's `Plugins/Marketplace` folder. 

</TabItem> {/* [END] Blueprint */}
<TabItem value="cpp" label="C++ Project">

:::info[PREREQUISITES]
I have never worked on Unreal Engine on MacOS, so I don't know exactly what requirements you must install.\
I would recommend you to follow any tutorial on the Internet, but here some guidelines:
- install XCode and Visual Studio Code.
- in Visual Studio Code, add the `C/C++` extension.
:::

Clone or download the plugin sources into your project's `Plugins` folder.\
The resulting path of the `.uplugin` file must be:\
`<your project path>\Plugins\ProceduralDungeon\ProceduralDungeon.uplugin`

Recompile your project in Visual Studio Code.

</TabItem> {/* [END] C++ */}
</Tabs> {/* [END] Blueprint | C++ */}
</TabItem> {/* [END] MacOS  */}
</Tabs> {/* [END] Windows | MacOS */}
