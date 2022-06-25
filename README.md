<div align="center">

![Multiplayer Sessions Plugin logo](Images/Splash.png)
# Multiplayer Sessions Plugin

</div>

## Overview
Plugin for hosting and joining game sessions using Online Subsystem Steam API.

## Prerequisites
* Unreal Engine 5.0.2
* [Steam account](https://store.steampowered.com/login/)
* [Steamworks SDK](https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/Online/Steam/)

## How to use in your project
1. Create or use existing Unreal Engine 5 project.
2. Add Online Subsystem Steam plugin to project. Edit->Plugins->Search Online Subsystem Steam->Check Enabled->Click Restart Now.
3. Open **Config/DefaultEngine.ini** in project folder and add the setting block from **End Result** section of <a href="https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/Online/Steam/">Steamworks SDK</a>.
```ini
[/Script/Engine.GameEngine]
+NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="OnlineSubsystemSteam.SteamNetDriver",DriverClassNameFallback="OnlineSubsystemUtils.IpNetDriver")

[OnlineSubsystem]
DefaultPlatformService=Steam

[OnlineSubsystemSteam]
bEnabled=true
SteamDevAppId=480

; If using Sessions
; bInitServerOnClient=true

[/Script/OnlineSubsystemSteam.SteamNetDriver]
NetConnectionClassName="OnlineSubsystemSteam.SteamNetConnection"
```
4. Add the **Plugins** folder inside project folder.
5. Clone this repository inside Plugins folder `git clone https://github.com/RairunDemps/MultiplayerSessionsPlugin.git Plugins/MultiplayerSessions`.
6. Delete your .vs, Binaries, DerivedDataCache, Intermediate, Saved folders and .sln file. Right click on .uproject file and choose **Generate Visual Studio project files**.
7. Open your .sln file. You should now see Plugins folder in the Solution Explorer and Multiplayer Sessions folder inside.
8. Open your .uproject file. Select **Yes** to rebuild modules.
9. In Content Browser click Settings and check **Show PLugin Content**. You should now see Plugins folder and **Multiplayer Sessions Content**, **Multiplayer Sessions Content C++ Classes** folders inside.
10. Create new level and save it.
11. Open Level Blueprint. Use **Create Widget** and choose **WBP_MenuWidget** as class.
12. Drag off of the **Return Value** output and search for Setup. Select it.
13. Change **Num Public Connections**, **Match Type** and **Lobby Path** to values you want in your game.

## License
Access to the sources of this project and their use are governed by the [Unreal Engine End User License Agreement](https://www.unrealengine.com/eula)