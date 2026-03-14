# Game Mechanics

<details>
<summary>Relevant source files</summary>

The following files were used as context for generating this wiki page:

- [Development.h](Development.h)
- [Player.cpp](Player.cpp)
- [config.json](config.json)

</details>



This document provides an overview of the gameplay systems that define the RTS experience in new-aoe. It covers the interconnected mechanics of resource management, technology progression, unit/building properties, and combat resolution. For implementation details of specific subsystems, see:
- Resource gathering and economy flow: [Resource and Economy System](#3.1)
- Technology tree structure and upgrades: [Technology Tree](#3.2)
- Entity lifecycle and stats: [Units and Buildings](#3.3)
- Attack/defense calculations: [Combat System](#3.4)

For the rendering and display of game entities, see [Rendering and Display](#4.3). For AI decision-making based on these mechanics, see [AI Architecture](#5.1).

## Gameplay Overview

The game implements classic RTS mechanics where players gather resources, advance through technological ages, construct buildings, train units, and engage in combat. All mechanics are data-driven through [config.json:1-471](), enabling balance adjustments without recompilation.

The core gameplay loop operates through the `Player` class [Player.cpp:1-358](), which manages:
- Resource pools (wood, food, stone, gold)
- Entity collections (buildings, humans, missiles)
- Technology state via `Development` class [Development.h:1-139]()
- Population limits and housing requirements

## Core Mechanics Architecture

```mermaid
graph TB
    subgraph "Player State"
        PLAYER["Player Class<br/>[Player.cpp]"]
        RES["Resource Pool<br/>wood/food/stone/gold<br/>[Player.cpp:218-251]"]
        ENTITIES["Entity Lists<br/>build/human/missile<br/>[Player.cpp:21-27]"]
        DEV["Development<br/>playerScience<br/>[Player.cpp:8,14]"]
    end
    
    subgraph "Technology System"
        DEVLAB["developLab map<br/>[Development.h:131]"]
        CIV["civilization<br/>Stone→Tool→Bronze→Iron<br/>[Development.h:114]"]
        UPGRADES["Upgrade Chains<br/>conditionDevelop nodes"]
    end
    
    subgraph "Resource Sources"
        NATURE["Natural Resources<br/>Trees/Stone/Gold/Animals<br/>[config.json:46-77]"]
        FARMERS["Farmer Gathering<br/>[Player.cpp:108-119]"]
        FARMS["Farm Buildings<br/>Renewable Food<br/>[config.json:241-245]"]
    end
    
    subgraph "Resource Sinks"
        BUILD["Building Construction<br/>changeResource_byBuild<br/>[Player.cpp:253-259]"]
        TRAIN["Unit Training<br/>addArmy/addFarmer<br/>[Player.cpp:62-119]"]
        RESEARCH["Technology Research<br/>changeResource_byBuildAction<br/>[Player.cpp:261-268]"]
    end
    
    subgraph "Entity Creation"
        ADDBUILDING["addBuilding<br/>[Player.cpp:32-49]"]
        ADDHUMAN["addHuman/addArmy/addFarmer<br/>[Player.cpp:51-132]"]
        ENFORCEMENT["enforcementAction<br/>[Player.cpp:272-324]"]
    end
    
    subgraph "Configuration"
        CONFIG["config.json<br/>400+ Parameters<br/>[config.json:1-471]"]
        GLOBALVAR["GlobalVariate<br/>Runtime Constants"]
    end
    
    PLAYER --> RES
    PLAYER --> ENTITIES
    PLAYER --> DEV
    
    DEV --> DEVLAB
    DEV --> CIV
    DEVLAB --> UPGRADES
    
    NATURE --> FARMERS
    FARMS --> FARMERS
    FARMERS --> RES
    
    RES --> BUILD
    RES --> TRAIN
    RES --> RESEARCH
    
    BUILD --> ADDBUILDING
    TRAIN --> ENFORCEMENT
    RESEARCH --> DEV
    
    ENFORCEMENT --> ADDHUMAN
    ADDBUILDING --> ENTITIES
    ADDHUMAN --> ENTITIES
    
    CONFIG --> GLOBALVAR
    GLOBALVAR --> DEV
    GLOBALVAR --> ADDBUILDING
    GLOBALVAR --> ADDHUMAN
```

**Sources:** [Player.cpp:1-358](), [Development.h:1-139](), [config.json:1-471]()

## Resource Types

The game uses four primary resource types that fuel all player actions:

| Resource | Primary Sources | Gathering Method | Storage | Key Uses |
|----------|----------------|------------------|---------|----------|
| **Wood** | Trees [config.json:56-57]() | Farmers chop/carry [config.json:60,68]() | Storage Buildings | Building construction, some units |
| **Food** | Gazelles, Bushes, Farms, Fish [config.json:46,74,77,242]() | Farmers hunt/gather/farm [config.json:62,69]() | Granary Buildings | Unit training, age advancement |
| **Stone** | Stone deposits [config.json:75]() | Farmers mine/carry [config.json:63,71]() | Storage Buildings | Defensive structures, advanced buildings |
| **Gold** | Gold ore deposits [config.json:76]() | Farmers mine/carry [config.json:62,70]() | Storage Buildings | Advanced units, technologies |

Resource management is handled through `Player::changeResource()` [Player.cpp:218-251](), which validates and updates the player's resource pool based on gather/spend operations.

### Initial Resources

Players start with:
```
Wood:  200  [config.json:80]
Food:  200  [config.json:81]
Gold:  150  [config.json:82]
Stone: 0    [config.json:83]
```

### Farmer Carry Capacity

Farmers have limited carry capacity that can be upgraded:

| Resource | Base Capacity | Upgraded Capacity | Upgrade Location |
|----------|---------------|-------------------|------------------|
| Wood | 10 [config.json:60]() | 12 [config.json:64]() | Market |
| Food | 10 [config.json:61]() | 13 [config.json:65]() | Market |
| Gold | 10 [config.json:62]() | 15 [config.json:66]() | Market |
| Stone | 10 [config.json:63]() | 13 [config.json:67]() | Market |

### Gather Speed

Base gathering rates (resources per frame):
```
Wood:  0.02  [config.json:68]
Food:  0.02  [config.json:69]
Gold:  0.02  [config.json:70]
Stone: 0.02  [config.json:71]
```

These rates are modified by technology upgrades through `Development::get_rate_ResorceGather()` [Development.h:36]().

**Sources:** [Player.cpp:218-251](), [config.json:60-83](), [Development.h:34-37]()

## Civilization Ages

The technology progression system divides gameplay into distinct ages, each unlocking new buildings, units, and upgrades. Age advancement is managed by the `Development` class:

```mermaid
graph LR
    STONE["Stone Age<br/>CIVILIZATION_STONEAGE<br/>civilization=1<br/>[Development.h:114]"]
    TOOL["Tool Age<br/>CIVILIZATION_TOOLAGE<br/>civilization=2"]
    BRONZE["Bronze Age<br/>CIVILIZATION_BRONZEAGE<br/>civilization=3"]
    IRON["Iron Age<br/>CIVILIZATION_IRONAGE<br/>civilization=4"]
    
    STONE -->|"500 Food<br/>60 sec<br/>[config.json:99-100]"| TOOL
    TOOL -->|"1000 Food<br/>800 Gold<br/>60 sec<br/>[config.json:101-102]"| BRONZE
    BRONZE -->|"Additional Research"| IRON
    
    STONE -.->|"Basic units<br/>Basic buildings"| STONE
    TOOL -.->|"Advanced units<br/>Market/Dock"| TOOL
    BRONZE -.->|"Elite units<br/>Siege weapons"| BRONZE
    IRON -.->|"Top-tier units"| IRON
```

Age advancement occurs through the Town Center building action and is tracked by `Development::civilization` [Development.h:114](). The `civiChange()` method [Development.h:135]() handles age transitions.

**Sources:** [Development.h:50-52,114,135](), [config.json:99-102]()

## Entity Statistics System

All game entities (units, buildings) have statistics defined in [config.json]() and accessed through the `Development` class. The stat calculation system applies base values plus technology bonuses:

### Stat Categories

```mermaid
graph TB
    subgraph "Base Stats [config.json]"
        BLOOD["Hit Points<br/>BLOOD_*"]
        SPEED["Movement Speed<br/>SPEED_*"]
        ATK["Attack Damage<br/>ATK_*"]
        DEF["Defense Values<br/>DEFCLOSE_*/DEFSHOOT_*"]
        VISION["Vision Range<br/>VISION_*"]
        DIS["Attack Range<br/>DIS_*"]
        INTERVAL["Attack Interval<br/>INTERVAL_*"]
    end
    
    subgraph "Technology Modifiers [Development.h]"
        RATEBLD["get_rate_Blood<br/>[Development.h:21]"]
        RATEMOVE["get_rate_Move<br/>[Development.h:20]"]
        RATEATK["get_rate_Attack<br/>[Development.h:24]"]
        RATEDEF["get_rate_Defence<br/>[Development.h:29]"]
        ADDATK["get_addition_Attack<br/>[Development.h:25]"]
        ADDDEF["get_addition_Defence<br/>[Development.h:30]"]
        ADDDIS["get_addition_DisAttack<br/>[Development.h:27]"]
    end
    
    subgraph "Final Calculated Stats"
        FINALBLD["Effective HP"]
        FINALSPD["Effective Speed"]
        FINALATK["Effective Attack"]
        FINALDEF["Effective Defense"]
    end
    
    BLOOD --> RATEBLD
    RATEBLD --> FINALBLD
    
    SPEED --> RATEMOVE
    RATEMOVE --> FINALSPD
    
    ATK --> RATEATK
    ATK --> ADDATK
    RATEATK --> FINALATK
    ADDATK --> FINALATK
    
    DEF --> RATEDEF
    DEF --> ADDDEF
    RATEDEF --> FINALDEF
    ADDDEF --> FINALDEF
    
    DIS --> ADDDIS
    ADDDIS --> FINALATK
```

**Sources:** [Development.h:19-31](), [config.json:56-470]()

## Unit Type Examples

Sample unit statistics from [config.json]():

### Infantry Units

| Unit | HP | Speed | Attack | Range | Close Def | Shoot Def | Cost |
|------|----|----|--------|-------|-----------|-----------|------|
| Clubman (Tier 1) | 40 [config.json:273]() | 2.44 [config.json:266]() | 3 [config.json:270]() | Melee | 0 [config.json:271]() | 0 [config.json:272]() | 50 Food [config.json:159]() |
| Clubman (Tier 2) | 50 [config.json:274]() | 2.44 [config.json:275]() | 5 [config.json:279]() | Melee | 0 [config.json:280]() | 0 [config.json:281]() | 100 Food (upgrade) [config.json:161]() |
| Short Swordsman (Tier 1) | 150 [config.json:282]() | 2.44 [config.json:283]() | 9 [config.json:287]() | Melee | 1 [config.json:288]() | 0 [config.json:289]() | 35 Food + 15 Gold [config.json:464-465]() |

### Ranged Units

| Unit | HP | Speed | Attack | Range | Defense | Cost | Train Time |
|------|----|----|--------|-------|---------|------|------------|
| Slinger | 25 [config.json:314]() | 2.44 [config.json:315]() | 2 [config.json:319]() | 4 [config.json:317]() | 0/2 [config.json:320-321]() | 40 Food + 10 Stone [config.json:163-164]() | 24 sec [config.json:165]() |
| Bowman | 35 [config.json:322]() | 2.44 [config.json:323]() | 3 [config.json:327]() | 5 [config.json:325]() | 0/0 [config.json:328-329]() | 40 Food + 20 Wood [config.json:170-171]() | 30 sec [config.json:172]() |
| Improved Bowman (Tier 1) | 120 [config.json:330]() | 2.44 [config.json:331]() | 8 [config.json:335]() | 6 [config.json:333]() | 0/0 [config.json:336-337]() | - | - |

### Cavalry Units

| Unit | HP | Speed | Attack | Defense | Cost | Special |
|------|----|----|--------|---------|------|---------|
| Scout | 80 [config.json:346]() | 4.07 [config.json:347]() | 5 [config.json:351]() | 0/0 [config.json:352-353]() | 60 Food [config.json:177]() | High vision: 8 [config.json:348]() |
| Cavalry | 150 [config.json:366]() | 4.07 [config.json:367]() | 8 [config.json:371]() | 0/0 [config.json:372-373]() | 70 Food + 80 Gold [config.json:179-180]() | Fast assault unit |
| Chariot | 120 [config.json:354]() | 3.5 [config.json:355]() | 10 [config.json:357]() | 1/0 [config.json:360-361]() | 40 Food + 60 Wood [config.json:363-364]() | Moderate speed |

**Sources:** [config.json:266-470]()

## Building Mechanics

Buildings serve multiple roles: resource storage, unit production, technology research, and defense. Key building parameters:

### Construction Costs and Times

| Building | HP | Wood Cost | Build Time | Special Properties |
|----------|----|----|------------|-------------------|
| Town Center | 600 [config.json:93]() | 200 [config.json:95]() | 60 sec [config.json:96]() | Creates farmers, age advancement [config.json:97-102]() |
| House | 75 [config.json:103]() | 30 [config.json:105]() | 20 sec [config.json:106]() | +4 population [config.json:43]() |
| Storage Building | 350 [config.json:107]() | 120 [config.json:109]() | 30 sec [config.json:110]() | Resource storage, infantry upgrades [config.json:111-142]() |
| Granary | 350 [config.json:147]() | 120 [config.json:149]() | 30 sec [config.json:150]() | Archer research, wall research [config.json:151-154]() |
| Army Camp | 350 [config.json:155]() | 125 [config.json:157]() | 30 sec [config.json:158]() | Creates clubmen [config.json:159-162]() |
| Archery Range | 350 [config.json:166]() | 150 [config.json:168]() | 40 sec [config.json:169]() | Creates bowmen [config.json:170-172]() |
| Stable | 350 [config.json:173]() | 150 [config.json:175]() | 40 sec [config.json:176]() | Creates cavalry [config.json:177-181]() |
| Market | 350 [config.json:182]() | 150 [config.json:184]() | 40 sec [config.json:185]() | Economic upgrades [config.json:196-240]() |
| Dock | 350 [config.json:186]() | 100 [config.json:188]() | 40 sec [config.json:189]() | Creates ships [config.json:190-195]() |
| Farm | 50 [config.json:241]() | 75 [config.json:244]() | 30 sec [config.json:245]() | Produces 250 food [config.json:242]() |
| Arrow Tower | 125 [config.json:246]() | 150 Stone [config.json:252]() | 80 sec [config.json:253]() | ATK: 3, Range: 7 [config.json:247,250]() |
| Wall | 200 [config.json:254]() | 5 Stone [config.json:256]() | 10 sec [config.json:257]() | Defensive structure |

### Building Action System

Buildings can perform actions (train units, research technologies) through the `Player::enforcementAction()` system [Player.cpp:272-324](). When an action completes:

1. Resources are consumed via `changeResource_byBuildAction()` [Player.cpp:261-268]()
2. Technology state updates via `Development::finishAction()` [Development.h:79-80]()
3. If the action creates units, `enforcementAction()` spawns them at valid adjacent blocks [Player.cpp:295-323]()

**Sources:** [Player.cpp:272-324](), [config.json:93-257](), [Development.h:79-103]()

## Population Management

Population is managed through housing requirements:

```mermaid
graph LR
    subgraph "Population Calculation [Development.h]"
        HOUSES["Houses + Town Center<br/>get_homeNum()<br/>[Development.h:69]"]
        HOUSENUM["House Count<br/>homeNum<br/>[Development.h:119]"]
        CENTERNUM["Center Count<br/>centerNum<br/>[Development.h:120]"]
        
        HOUSENUM --> HOUSES
        CENTERNUM --> HOUSES
    end
    
    subgraph "Population Limits"
        MAXPOP["Max Population<br/>get_homeNum() * 4<br/>[Development.h:60]"]
        HARDCAP["Hard Cap<br/>humanNum_Top = 50<br/>[Development.h:122]"]
        ACTUALPOP["Current Population<br/>humanNum<br/>[Development.h:121]"]
    end
    
    HOUSES --> MAXPOP
    MAXPOP --> CANREACH["Population Capacity<br/>getHumanNumCanReach()<br/>[Development.h:62]"]
    HARDCAP --> CANREACH
    
    ACTUALPOP --> CHECK["Can Add Unit?<br/>get_isHumanHaveSpace()<br/>[Development.h:64]"]
    CANREACH --> CHECK
```

Each house provides 4 population slots [config.json:43](). The Town Center counts as one house [Development.h:69](). Population is tracked with:
- `addHumanNum()` / `subHumanNum()` [Development.h:55-56]()
- `addHome()` / `subHome()` [Development.h:70-71]() when houses are built/destroyed

**Sources:** [Development.h:54-72,119-122](), [config.json:43]()

## Technology Upgrade Chains

Technologies are organized as linked upgrade paths in the `developLab` map [Development.h:131](). Each building can perform multiple research actions:

### Example: Infantry Attack Upgrades (Storage Building)

```mermaid
graph LR
    BASE["Base Infantry Attack"]
    T1["Infantry Attack +2<br/>100 Food<br/>40 sec<br/>[config.json:111-113]"]
    T2["Infantry Attack +2<br/>200 Food + 120 Gold<br/>40 sec<br/>[config.json:114-117]"]
    
    BASE -->|"BUILDING_STOCK_UPGRADE_CLOSER_ATTACK"| T1
    T1 -->|"BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2"| T2
```

### Example: Resource Gathering Upgrades (Market)

| Technology | Effect | Cost | Time | Bonus |
|-----------|--------|------|------|-------|
| Wood Cutting | Wood +2 carry, +0.2 gather rate, +1 arrow range [config.json:199-201]() | 120 Food + 75 Wood [config.json:196-197]() | 40 sec [config.json:198]() | Affects all farmers |
| Stone Mining | Stone +3 carry, +0.2 gather rate, slinger +1 ATK/range [config.json:216-219]() | 100 Food + 50 Stone [config.json:213-214]() | 60 sec [config.json:215]() | Multi-effect upgrade |
| Gold Mining | Gold +3 carry, +0.2 gather rate [config.json:223-224]() | 120 Food + 100 Wood [config.json:220-221]() | 60 sec [config.json:222]() | Economy boost |
| Farm Upgrade | Farm food +75 [config.json:228]() | 200 Food + 50 Wood [config.json:225-226]() | 60 sec [config.json:227]() | Increases farm yield |

Upgrade bonuses are applied through `Development::get_addition_*()` and `get_rate_*()` methods [Development.h:19-37](), which query the current state of the `developLab` map.

**Sources:** [Development.h:19-37,131](), [config.json:111-240]()

## Combat Parameters

Combat involves attack, defense, and missile mechanics. Key parameters:

### Attack Types and Defense

Units have separate defense values against close combat and ranged attacks:
- **DEFCLOSE**: Defense against melee attacks [config.json:271,280,288,etc.]()
- **DEFSHOOT**: Defense against ranged attacks [config.json:272,281,289,etc.]()

### Missile System

Ranged units create `Missile` objects [Player.cpp:134-153]() with speed parameters:

| Missile Type | Speed | Range | Special |
|-------------|-------|-------|---------|
| Spear | 8.94 [config.json:398]() | - | Lion/Elephant attacks |
| Arrow | 20.12 [config.json:399]() | - | Archer/Bowman projectiles |
| Cobblestone | 20.12 [config.json:400]() | - | Slinger projectiles |
| Boulder | 8.94 [config.json:401]() | 2 AoE [config.json:402]() | Siege weapons, area damage |

Missiles are created via `Player::addMissile()` [Player.cpp:134-153]() and tracked in the `missile` list [Player.cpp:27]().

### Attack Distances

```
Close Combat:     17.89  [config.json:262]
Hit Target:       4.0    [config.json:263]
Elephant Attack:  42.57  [config.json:264]
```

Attack resolution uses these distances to determine when units can engage targets.

**Sources:** [Player.cpp:134-153](), [config.json:262-402]()

## Frame-Based Timing

Many game mechanics use frame-based timing with 25 FPS [config.json:33]():

### Unit Creation Times (in seconds)

| Action | Frames at 25 FPS |
|--------|------------------|
| Create Farmer | 20 sec → 500 frames [config.json:98]() |
| Create Clubman | 26 sec → 650 frames [config.json:160]() |
| Create Bowman | 30 sec → 750 frames [config.json:172]() |
| Create Scout | 30 sec → 750 frames [config.json:178]() |
| Build Town Center | 60 sec → 1500 frames [config.json:96]() |
| Build Arrow Tower | 80 sec → 2000 frames [config.json:253]() |

### Action Intervals

Units have attack intervals that determine how frequently they can attack:

```
Clubman:       1.5 sec  [config.json:269]
Bowman:        1.4 sec  [config.json:326]
Cavalry:       1.5 sec  [config.json:370]
Stone Thrower: 4.2 sec  [config.json:387]
```

These intervals are tracked in frame counts during combat execution.

**Sources:** [config.json:33,96-470]()

## Validation and Constraints

The `Development` class provides validation methods before allowing actions:

```mermaid
graph TB
    REQUEST["Player Action Request"]
    
    SHOW["get_isBuildActionShowAble<br/>[Development.h:92]"]
    ABLE["get_isBuildActionAble<br/>[Development.h:90]"]
    
    CHECK_CIV["Check civilization<br/>requirement"]
    CHECK_RES["Check resource<br/>availability"]
    CHECK_PREREQ["Check technology<br/>prerequisites"]
    
    REQUEST --> SHOW
    SHOW --> CHECK_CIV
    CHECK_CIV -->|"Age too low"| REJECT1["Hide from UI"]
    CHECK_CIV -->|"Age OK"| ABLE
    
    ABLE --> CHECK_RES
    ABLE --> CHECK_PREREQ
    
    CHECK_RES -->|"Insufficient"| REJECT2["Cannot Execute"]
    CHECK_PREREQ -->|"Not researched"| REJECT2
    
    CHECK_RES -->|"Available"| ALLOW["Execute Action"]
    CHECK_PREREQ -->|"Met"| ALLOW
```

The two-phase check (showable → executable) allows the UI to display grayed-out options for unavailable technologies while hiding options that are completely inaccessible in the current age.

**Sources:** [Development.h:85-92]()

## Entity Creation Flow

The complete flow for creating game entities:

```mermaid
sequenceDiagram
    participant UI as "Player UI"
    participant Player as "Player Class<br/>[Player.cpp]"
    participant Dev as "Development<br/>[Player.cpp:8]"
    participant List as "Entity Lists<br/>[Player.cpp:21-27]"
    
    UI->>Player: "Request unit creation"
    
    Player->>Dev: "get_isBuildActionAble()<br/>[Development.h:90]"
    Dev-->>Player: "Check resources & tech"
    
    alt "Resources insufficient"
        Player-->>UI: "Action fails"
    else "Resources available"
        Player->>Player: "changeResource_byBuildAction()<br/>[Player.cpp:261-268]"
        Note over Player: "Deduct resources"
        
        Player->>Player: "Building performs action<br/>over time"
        
        Player->>Player: "enforcementAction()<br/>[Player.cpp:272-324]"
        
        alt "Creates farmer"
            Player->>Player: "addFarmer()<br/>[Player.cpp:108-119]"
        else "Creates army"
            Player->>Player: "addArmy()<br/>[Player.cpp:62-73]"
        end
        
        Player->>List: "Insert into human list<br/>[Player.cpp:57-58,70-72]"
        Player->>Dev: "addHumanNum()<br/>[Development.h:55]"
        
        Player-->>UI: "Unit created"
    end
```

**Sources:** [Player.cpp:62-119,261-324](), [Development.h:55,90]()