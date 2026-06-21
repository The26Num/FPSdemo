# UE5 C++ 多人 FPS Demo 技术文档

## 1. 项目简介

本项目是一个基于 Unreal Engine 5.4 和 C++ 实现的局域网多人 FPS Demo。项目支持玩家在本地局域网环境下创建服务器、加入服务器、进入 Lobby 房间，并在同一局游戏中进行多人射击、击杀敌人、个人计分和胜负结算。

项目采用 Listen Server 的联机方式，其中一名玩家作为主机创建服务器，其他玩家通过 IP 加入服务器。游戏内主要逻辑由服务器进行权威判定，包括子弹生成、敌人生成、伤害处理、得分统计和比赛结果判断。

GitHub 仓库地址：[The26Num/FPSdemo](https://github.com/The26Num/FPSdemo#)

## 2. 主要技术

本项目主要使用以下技术：

* Unreal Engine 5.4
* C++
* Unreal 网络同步机制
* Server RPC / Client RPC
* GameMode / GameState / PlayerState
* UMG UI 系统
* Enhanced Input 输入系统
* Listen Server 局域网联机

## 3. 功能概述

项目实现了以下主要功能：

1. 主菜单系统  
   玩家可以选择 Host LAN 创建局域网服务器，或者输入 IP 加入已有服务器。

2. Lobby 房间系统  
   玩家进入服务器后可以创建房间、加入房间，并由房主开始游戏。同一个 Lobby Server 中只允许存在一个房间。

3. 多人射击系统  
   玩家可以拾取武器、开火、换弹。开火请求由客户端发送到服务器，服务器检查弹药并生成子弹。

4. 敌人系统  
   敌人由服务器生成，并在场景中追击玩家。敌人被击杀后会在一定时间后从随机刷怪点重新生成。

5. 个人得分系统  
   每个玩家拥有独立分数。敌人被击杀时，服务器根据子弹 Owner 判断击杀者，并为对应玩家加分。

6. 比赛结算系统  
   游戏包含倒计时机制。时间结束后比较玩家个人分数，分数更高者获胜。如果游戏过程中有玩家死亡，则比赛立即结束，存活玩家获胜。

7. HUD 显示系统  
   HUD 显示玩家血量、弹药、个人分数、剩余敌人数、倒计时和比赛结果。

## 4. 技术实现过程

### 基础 FPS 操作

项目首先基于 UE5 第一人称模板搭建基础 FPS 操作，包括玩家移动、视角控制、武器拾取和子弹发射。随后在此基础上逐步加入多人网络同步功能。

### 服务器权威

在联机实现方面，项目采用 Server Authoritative 的设计方式。客户端只负责发送操作请求，例如开火、换弹、创建房间和加入房间，真正的游戏状态修改由服务器完成。例如玩家开火时，客户端调用 Server RPC，请求服务器使用当前装备武器开火。服务器检查武器弹药数量，如果弹药充足，则扣除弹药并生成子弹。这样可以避免客户端直接决定关键游戏状态。

### 游戏状态同步

在游戏状态同步方面，项目使用 GameMode、GameState 和 PlayerState 进行分工。GameMode 只存在于服务器端，用于处理游戏规则，例如刷怪、得分、计时和胜负判断。GameState 用于同步全局状态，例如剩余敌人数、倒计时和比赛结果。PlayerState 用于保存每个玩家自己的个人分数。这样可以避免将所有状态都放在同一个类中，使多人同步结构更加清晰。

### Lobby 房间系统

在 Lobby 系统中，项目设计了主菜单地图和 Lobby 地图。玩家可以在主菜单中选择 Host LAN 创建 Listen Server，也可以输入局域网 IP 加入服务器。进入 Lobby 后，玩家可以创建房间或加入已有房间。由于本项目是小型 Demo，因此设定为一个服务器一次只允许存在一个房间。房主点击 Start Game 后，服务器通过 ServerTravel 将所有玩家带入正式游戏地图。

### 敌人系统

在敌人系统中，服务器负责生成敌人。地图中预先放置多个 TargetPoint 作为敌人刷怪点，并设置统一 Tag。游戏开始时，服务器会在各个刷怪点生成敌人。敌人死亡后，服务器会在 3 秒后从刷怪点中随机选择一个位置补充新的敌人，并限制场上最多同时存在 4 个敌人。为了保证运行时生成的敌人可以正常移动，敌人蓝图设置了 Auto Possess AI，并在生成后确保其拥有 AIController。

### 个人得分系统

在得分系统中，子弹命中敌人后，由服务器处理敌人伤害和死亡逻辑。由于子弹的 Owner 是开火玩家，因此敌人死亡时可以根据子弹 Owner 判断击杀者。服务器随后找到击杀者对应的 PlayerState，并增加该玩家的个人分数。HUD 上显示的分数为玩家自己的分数，而不是全局分数。

### 比赛结算系统

在比赛结算方面，GameMode 维护比赛倒计时。当时间结束后，服务器遍历所有玩家的 PlayerState，比较个人分数并设置最终结果。如果比赛过程中某名玩家死亡，则服务器立即结束比赛，并判定仍然存活的玩家获胜。最终比赛结果通过 GameState 同步到所有客户端，并显示在 HUD 上。

## 5. 核心模块说明

| 模块       | 主要类                                                             | 作用                     |
| -------- | --------------------------------------------------------------- | ---------------------- |
| 主菜单系统    | MainMenuUserWidget / MainMenuPlayerController                   | 负责 Host LAN 和 Join LAN |
| Lobby 系统 | FPSLobbyGameMode / FPSLobbyGameState / FPSLobbyPlayerController | 负责房间创建、加入和开始游戏         |
| 玩家系统     | FPSdemoCharacter                                                | 负责玩家生命值、死亡、HUD 和当前武器   |
| 武器系统     | TP_WeaponComponent                                              | 负责开火、弹药和换弹             |
| 子弹系统     | FPSdemoProjectile                                               | 负责命中检测和伤害传递            |
| 敌人系统     | EnemyCharacter                                                  | 负责敌人血量、追击、攻击和死亡        |
| 游戏规则     | FPSdemoGameMode                                                 | 负责刷怪、得分、计时和胜负判断        |
| 状态同步     | FPSdemoGameState                                                | 同步敌人数量、倒计时和比赛结果        |
| 个人状态     | FPSdemoPlayerState                                              | 保存每个玩家的个人分数            |
| UI 系统    | MyUserWidget / LobbyUserWidget / MainMenuUserWidget             | 显示 HUD、Lobby 和主菜单      |

## 6. 运行与测试方式

本地双窗口测试流程如下：

1. 第一个窗口进入 MainMenuMap，点击 Host LAN Game。

2. 第二个窗口进入 MainMenuMap，输入 `127.0.0.1`，点击 Join LAN Game。

3. 两个窗口进入 Lobby 后，第一个玩家创建房间。

4. 第二个玩家输入相同房间名并加入房间。

5. 房主点击 Start Game，服务器将所有玩家带入正式游戏地图。

6. 玩家在游戏地图中进行射击、击杀敌人和得分。

7. 时间结束后根据个人分数结算胜负，或者某名玩家死亡后立即结算胜负。

局域网测试时，Host 玩家点击 Host LAN Game，Client 玩家输入 Host 电脑的 IPv4 地址加入服务器。

## 7. 总结

本项目完成了一个基于 UE5 C++ 的局域网多人 FPS Demo，实现了从主菜单、Lobby 房间到多人战斗和比赛结算的完整流程。项目重点使用了 UE 的多人网络框架，包括 RPC、GameMode、GameState、PlayerState 和 Replication 等机制。通过服务器权威的方式处理关键逻辑，使开火、敌人生成、得分和胜负判断能够在多人环境下保持一致。

该项目展示了 UE5 C++ 多人游戏开发中的基本流程和核心技术，包括局域网连接、房间系统、服务器生成 Actor、状态同步、HUD 更新和多人比赛规则设计。

10. 项目代码逻辑与类关系说明

----------------

## 8. 代码逻辑补充

### 8.1 整体代码结构

项目整体可以分为三个阶段：主菜单阶段、Lobby 阶段和正式游戏阶段。

主菜单阶段主要由 `MainMenuGameMode`、`MainMenuPlayerController` 和 `MainMenuUserWidget` 组成。该阶段负责玩家选择创建服务器或加入服务器。

Lobby 阶段主要由 `FPSLobbyGameMode`、`FPSLobbyGameState`、`FPSLobbyPlayerController` 和 `LobbyUserWidget` 组成。该阶段负责房间创建、加入和开始游戏。

正式游戏阶段主要由 `FPSdemoGameMode`、`FPSdemoGameState`、`FPSdemoPlayerState`、`FPSdemoCharacter`、`TP_WeaponComponent`、`FPSdemoProjectile`、`EnemyCharacter` 和 `MyUserWidget` 组成。该阶段负责实际的多人射击玩法，包括玩家操作、武器开火、敌人生成、伤害处理、个人得分、倒计时和胜负结算。

通过这种阶段划分，项目代码不会把菜单逻辑、房间逻辑和战斗逻辑混在一起，整体结构更清晰。

### 8.2 主菜单模块的类关系

主菜单模块负责让玩家进入局域网联机流程。

`MainMenuGameMode` 是主菜单地图使用的 GameMode，它指定主菜单阶段使用的 PlayerController，并且不生成战斗角色。因为主菜单只需要 UI 操作，不需要玩家 Pawn。

`MainMenuPlayerController` 负责创建主菜单 UI，并将输入模式切换为 UI 模式，使玩家可以点击按钮和输入 IP 地址。

`MainMenuUserWidget` 负责显示 Host LAN 和 Join LAN 两个按钮。玩家点击 Host LAN 后，当前窗口会打开 Lobby 地图并成为 Listen Server；玩家点击 Join LAN 后，会读取输入框中的 IP 地址，并连接到对应服务器。

因此，主菜单模块的逻辑关系是：`MainMenuGameMode` 管理主菜单地图规则，`MainMenuPlayerController` 创建并控制 UI，`MainMenuUserWidget` 负责响应按钮操作并触发进入服务器或连接服务器的流程。

### 8.3 Lobby 模块的类关系

Lobby 模块负责房间系统。

`FPSLobbyGameMode` 是 Lobby 阶段的核心类，只存在于服务器端。它负责判断房间是否可以创建、玩家是否可以加入房间、房主是否可以开始游戏等。由于项目设定为一个服务器一次只允许存在一个房间，因此当前房间名、房主玩家和房间内玩家列表都由 `FPSLobbyGameMode` 维护。

`FPSLobbyGameState` 用于同步 Lobby 的公共信息，例如当前是否存在房间、房间名、房间人数等。因为 GameState 会存在于服务器和客户端，所以客户端可以通过它看到当前房间状态。

`FPSLobbyPlayerController` 负责接收 Lobby UI 的按钮请求。当玩家点击创建房间、加入房间或开始游戏时，UI 会调用 PlayerController 中对应的函数，再由 PlayerController 向服务器发送请求。服务器收到请求后，将具体判断交给 `FPSLobbyGameMode` 处理。

`LobbyUserWidget` 只负责显示 Lobby UI 和收集用户输入，例如房间名。它本身不直接修改房间状态，而是把请求交给 `FPSLobbyPlayerController`。

这个模块的调用关系可以概括为：玩家点击 Lobby UI 按钮，`LobbyUserWidget` 将操作交给 `FPSLobbyPlayerController`，`FPSLobbyPlayerController` 通过服务器请求通知 `FPSLobbyGameMode`，`FPSLobbyGameMode` 判断并更新房间状态，`FPSLobbyGameState` 再把房间状态同步给客户端。

### 8.4 游戏规则模块的类关系

正式游戏阶段的核心规则由 `FPSdemoGameMode` 管理。由于 GameMode 只存在于服务器端，所以它适合处理所有需要服务器权威判断的逻辑。

`FPSdemoGameMode` 负责敌人生成、敌人死亡处理、玩家得分处理、比赛倒计时、玩家死亡结算和时间结束结算。它不会直接负责 UI 显示，而是通过 `FPSdemoGameState` 或玩家自身的接口把结果同步给客户端。

`FPSdemoGameState` 用于保存所有玩家都需要知道的全局状态，例如剩余敌人数、剩余时间和比赛结果。客户端通过 GameState 获取这些公共状态，并更新本地 HUD。

`FPSdemoPlayerState` 用于保存单个玩家自己的个人分数。之所以不把个人分数放在 GameState 中，是因为 GameState 更适合保存全局状态，而每名玩家的分数是玩家自身状态，放在 PlayerState 中更符合多人游戏的结构。

因此，正式游戏中的全局规则由 `FPSdemoGameMode` 决定，全局结果通过 `FPSdemoGameState` 同步，个人数据通过 `FPSdemoPlayerState` 保存。

### 8.5 玩家、武器和子弹之间的关系

玩家角色由 `FPSdemoCharacter` 表示。它负责玩家的生命值、死亡状态、当前装备武器和 HUD 引用。玩家本身不直接处理复杂的武器逻辑，而是通过持有的 `TP_WeaponComponent` 来完成开火和换弹。

`TP_WeaponComponent` 是武器组件，负责武器的弹药数量、最大弹药数、换弹状态和开火请求。因为场景中可以存在多把武器，所以弹药状态放在武器组件中，而不是统一放在玩家角色中。这样每把武器都可以拥有自己的弹药状态。

当玩家开火时，客户端只负责发送开火请求，真正的弹药检查、扣除弹药和子弹生成由服务器完成。服务器生成 `FPSdemoProjectile` 子弹时，会将子弹的 Owner 设置为开火的玩家角色。这样子弹命中敌人后，就可以根据 Owner 判断是哪名玩家造成了击杀。

`FPSdemoProjectile` 负责命中检测和伤害传递。它不会直接处理得分，而是把伤害来源传递给 `EnemyCharacter`。敌人死亡后，再由 `FPSdemoGameMode` 统一处理得分。

这部分的逻辑关系是：`FPSdemoCharacter` 持有当前武器，`TP_WeaponComponent` 处理武器状态和开火请求，`FPSdemoProjectile` 负责命中敌人，`EnemyCharacter` 处理伤害和死亡，最终由 `FPSdemoGameMode` 根据击杀者更新分数。

### 8.6 敌人系统的类关系

敌人由 `EnemyCharacter` 表示。敌人负责自身生命值、是否死亡、追击玩家和攻击玩家。敌人的移动和攻击逻辑主要在服务器端执行，以保证所有客户端看到的敌人状态一致。

敌人的生成不由敌人自己决定，而是由 `FPSdemoGameMode` 统一管理。地图中放置多个带有指定 Tag 的刷怪点，GameMode 在游戏开始时找到这些点位，并在服务器端生成敌人。

初始阶段，GameMode 会在各个刷怪点生成敌人；后续敌人死亡后，GameMode 会等待一定时间，再从刷怪点中随机选择一个位置生成新的敌人。为了避免敌人无限增加，GameMode 会检查当前场上敌人数，保证最多同时存在指定数量的敌人。

敌人死亡时不会自己决定分数，而是通知 `FPSdemoGameMode`。GameMode 根据敌人传回的击杀者信息，找到对应玩家的 PlayerState，并为其增加个人分数。

因此，敌人系统中的关系是：`FPSdemoGameMode` 负责生成和管理敌人数量，`EnemyCharacter` 负责单个敌人的行为和死亡状态，玩家和子弹负责造成伤害，最终得分仍由 GameMode 统一处理。

### 8.7 得分系统的类关系

得分系统使用 `FPSdemoPlayerState` 保存每名玩家自己的分数。

当子弹命中敌人并导致敌人死亡时，敌人会把击杀者信息传递给 `FPSdemoGameMode`。GameMode 会根据击杀者找到对应的 `FPSdemoPlayerState`，并增加该玩家的个人分数。

HUD 中显示的分数是当前玩家自己的分数，而不是全局分数。之前如果让 GameState 同时更新 Score，就会出现个人分数被全局分数覆盖的问题。因此项目中将分数逻辑从 GameState 中拆出，改由 PlayerState 管理。

这部分的逻辑关系是：`FPSdemoProjectile` 提供伤害来源，`EnemyCharacter` 在死亡时传递击杀者，`FPSdemoGameMode` 负责加分判断，`FPSdemoPlayerState` 保存个人分数，`MyUserWidget` 显示当前玩家自己的分数。

### 8.8 比赛结算系统的类关系

比赛结算由 `FPSdemoGameMode` 统一控制。项目中存在两种结算方式：倒计时结束结算和玩家死亡结算。

倒计时由 GameMode 维护，并通过 GameState 同步给所有客户端。时间结束后，GameMode 会遍历所有玩家的 PlayerState，比较每个玩家的个人分数，并判定分数最高的玩家获胜。如果分数相同，则判定为平局。

玩家死亡时，`FPSdemoCharacter` 会通知 `FPSdemoGameMode`。GameMode 收到玩家死亡事件后，会立即结束比赛，并寻找仍然存活的玩家作为胜利者。最终胜负结果不会由角色自己直接显示，而是由 GameMode 写入 GameState，再由 GameState 同步给所有客户端显示。

为了避免重复结算，GameMode 使用比赛结束标记来判断当前比赛是否已经结束。如果比赛已经结束，那么后续的计时、刷怪、得分和死亡结算都不会继续执行。

这部分的类关系是：`FPSdemoCharacter` 负责报告玩家死亡，`FPSdemoGameMode` 负责判断胜负，`FPSdemoGameState` 负责同步结果，`MyUserWidget` 负责显示最终结果。

### 8.9 UI 与游戏逻辑的关系

项目中的 UI 类只负责显示和输入，不直接决定游戏规则。

`MainMenuUserWidget` 负责主菜单按钮和 IP 输入；`LobbyUserWidget` 负责房间名输入和房间操作按钮；`MyUserWidget` 负责游戏中的 HUD 显示，包括血量、弹药、个人分数、剩余敌人数、倒计时和比赛结果。

UI 层不会直接修改游戏核心状态。例如 Lobby UI 不直接创建房间，而是通过 PlayerController 向服务器发送请求；HUD 不直接修改玩家分数，而是显示 PlayerState 或服务器同步过来的结果。

这种设计可以避免 UI 和游戏逻辑耦合过深，使 UI 只作为展示层和输入层存在。

### 8.10 项目代码组织总结

整体来看，本项目代码逻辑遵循以下原则：

第一，服务器端类负责核心规则。GameMode 用于处理房间规则、敌人生成、伤害结算、得分和胜负判断。

第二，同步类负责状态传播。GameState 用于同步全局状态，PlayerState 用于保存并同步玩家个人状态。

第三，角色和组件负责具体行为。Character 负责玩家自身状态，WeaponComponent 负责武器逻辑，Projectile 负责命中检测，EnemyCharacter 负责敌人行为。

第四，UI 类只负责显示和输入。Widget 不直接决定游戏结果，而是通过 PlayerController 或同步状态与游戏逻辑发生联系。

第五，关键事件由服务器统一判定。开火、生成子弹、敌人死亡、玩家加分、比赛结束等关键逻辑都由服务器处理，从而保证多人游戏中的状态一致性。

通过这种类之间的分工，项目整体形成了从“输入请求”到“服务器判断”，再到“状态同步”和“客户端显示”的完整流程，使多人 FPS Demo 的代码结构较为清晰，也方便后续继续扩展新的玩法。
