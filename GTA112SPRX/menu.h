// Esctasy menu for GTA V 1.12 BLES (PS3).
// Included by prx.cpp AFTER gtaCode.h; uses this project's native/graphics hooks.
// This is a PS3-native menu, not a ScriptHookV DLL or a Windows build.
#pragma once

namespace esctasy
{
    enum Page {
        HOME, PLAYER_PAGE, MODELS_PAGE, VEHICLE_PAGE, SPAWNER_PAGE,
        CUSTOMIZER_PAGE, TELEPORT_PAGE, WORLD_PAGE, SETTINGS_PAGE, PAGE_COUNT
    };
    enum OptionKind { SUBMENU, ACTION, TOGGLE, SPAWN_VEHICLE, CHANGE_MODEL, VEHICLE_MOD, VEHICLE_TURBO };

    struct Option {
        char* name;
        OptionKind kind;
        Page submenu;
        void (*callback)();
        bool* toggle;
        char* model;
        int modSlot;
    };

    static Option options[40];
    static int optionCount = 0;
    static int selected[PAGE_COUNT] = {0};
    static int scroll[PAGE_COUNT] = {0};
    static Page parents[PAGE_COUNT] = {HOME};
    static Page page = HOME;
    static bool opened = false;

    static bool godMode = false;
    static bool noWanted = false;
    static bool invisible = false;
    static bool infiniteClip = false;
    static bool neonAccent = true;
    static bool spawnInside = true;

    static Hash queuedVehicle = 0;
    static Hash queuedPlayerModel = 0;
    static Hash originalPlayerModel = 0;

    static const int VISIBLE_ITEMS = 11;

    void add(char* name, OptionKind kind, Page sub = HOME,
             void (*action)() = 0, bool* state = 0, char* model = 0, int slot = -1)
    {
        if (optionCount >= 40) return;
        Option& o = options[optionCount++];
        o.name = name;
        o.kind = kind;
        o.submenu = sub;
        o.callback = action;
        o.toggle = state;
        o.model = model;
        o.modSlot = slot;
    }

    void addSubmenu(char* text, Page p) { add(text, SUBMENU, p); }
    void addAction(char* text, void (*fn)()) { add(text, ACTION, HOME, fn); }
    void addToggle(char* text, bool* flag) { add(text, TOGGLE, HOME, 0, flag); }
    void addVehicle(char* text, char* model) { add(text, SPAWN_VEHICLE, HOME, 0, 0, model); }
    void addModel(char* text, char* model) { add(text, CHANGE_MODEL, HOME, 0, 0, model); }
    void addMod(char* text, int slot) { add(text, VEHICLE_MOD, HOME, 0, 0, 0, slot); }

    Ped me() { return PLAYER::PLAYER_PED_ID(); }
    Vehicle currentVehicle() { return PED::GET_VEHICLE_PED_IS_IN(me(), false); }

    void restoreHealth() {
        Ped p = me();
        if (ENTITY::DOES_ENTITY_EXIST(p))
            ENTITY::SET_ENTITY_HEALTH(p, ENTITY::GET_ENTITY_MAX_HEALTH(p));
    }
    void restoreArmour() { PED::SET_PED_ARMOUR(me(), 100); }
    void clearWanted() { PLAYER::CLEAR_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID()); }
    void giveWeapons() {
        Ped p = me();
        WEAPON::GIVE_WEAPON_TO_PED(p, GAMEPLAY::GET_HASH_KEY("WEAPON_PISTOL"), 250, true, true);
        WEAPON::GIVE_WEAPON_TO_PED(p, GAMEPLAY::GET_HASH_KEY("WEAPON_ASSAULTRIFLE"), 500, false, true);
        WEAPON::GIVE_WEAPON_TO_PED(p, GAMEPLAY::GET_HASH_KEY("WEAPON_PUMPSHOTGUN"), 100, false, true);
    }
    void repairVehicle() {
        Vehicle v = currentVehicle();
        if (!ENTITY::DOES_ENTITY_EXIST(v)) { drawNotification("~r~Enter a vehicle first"); return; }
        VEHICLE::SET_VEHICLE_FIXED(v);
        VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(v);
        VEHICLE::SET_VEHICLE_ENGINE_HEALTH(v, 1000.0f);
        VEHICLE::SET_VEHICLE_DIRT_LEVEL(v, 0.0f);
    }
    void cleanVehicle() {
        Vehicle v = currentVehicle();
        if (ENTITY::DOES_ENTITY_EXIST(v)) VEHICLE::SET_VEHICLE_DIRT_LEVEL(v, 0.0f);
    }
    void cyanVehicle() {
        Vehicle v = currentVehicle();
        if (ENTITY::DOES_ENTITY_EXIST(v)) {
            VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(v, 0, 255, 255);
            VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(v, 0, 20, 35);
        }
    }

    void teleport(float x, float y, float z) {
        Ped p = me();
        Vehicle v = currentVehicle();
        Entity target = ENTITY::DOES_ENTITY_EXIST(v) ? v : p;
        if (!ENTITY::DOES_ENTITY_EXIST(target)) return;
        ENTITY::SET_ENTITY_COORDS_NO_OFFSET(target, x, y, z, false, false, true);
    }
    void teleportAirport() { teleport(-1034.6f, -2733.6f, 20.1f); }
    void teleportSandy() { teleport(1855.6f, 3675.7f, 33.8f); }
    void teleportChiliad() { teleport(450.6f, 5566.8f, 806.2f); }
    void teleportMazeBank() { teleport(-75.0f, -818.8f, 326.2f); }
    void teleportWaypoint() {
        int blip = UI::GET_FIRST_BLIP_INFO_ID(8);
        if (!UI::DOES_BLIP_EXIST(blip)) {
            drawNotification("~r~Place a waypoint first");
            return;
        }
        Vector3 pos = UI::GET_BLIP_COORDS(blip);
        float groundZ = 0.0f;
        if (!GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(pos.x, pos.y, 1000.0f, &groundZ)) {
            drawNotification("~r~Waypoint terrain not loaded");
            return; // Avoid teleporting under the map or dropping from sky.
        }
        teleport(pos.x, pos.y, groundZ + 2.0f);
    }

    void weatherClear() { GAMEPLAY::SET_WEATHER_TYPE_NOW("CLEAR"); }
    void weatherSunny() { GAMEPLAY::SET_WEATHER_TYPE_NOW("EXTRASUNNY"); }
    void weatherRain() { GAMEPLAY::SET_WEATHER_TYPE_NOW("RAIN"); }
    void weatherThunder() { GAMEPLAY::SET_WEATHER_TYPE_NOW("THUNDER"); }
    void timeMorning() { TIME::SET_CLOCK_TIME(8, 0, 0); }
    void timeNoon() { TIME::SET_CLOCK_TIME(12, 0, 0); }
    void timeNight() { TIME::SET_CLOCK_TIME(23, 0, 0); }

    void resetOriginalCharacter() {
        if (originalPlayerModel != 0) {
            queuedPlayerModel = originalPlayerModel;
            STREAMING::REQUEST_MODEL(queuedPlayerModel);
        } else {
            drawNotification("~r~No original model recorded");
        }
    }

    // Build the same main menu families as the PC Esctasy menu.
    // Extra PC-specific entries can be added here with the functions above.
    void buildPage() {
        optionCount = 0;
        switch (page) {
        case HOME:
            addSubmenu("Player", PLAYER_PAGE);
            addSubmenu("Vehicle", VEHICLE_PAGE);
            addSubmenu("Teleport", TELEPORT_PAGE);
            addSubmenu("World", WORLD_PAGE);
            addSubmenu("Settings", SETTINGS_PAGE);
            break;
        case PLAYER_PAGE:
            addToggle("God Mode", &godMode);
            addToggle("Never Wanted", &noWanted);
            addToggle("Invisible", &invisible);
            addToggle("Infinite Clip", &infiniteClip);
            addAction("Max Health", restoreHealth);
            addAction("Max Armour", restoreArmour);
            addAction("Clear Wanted", clearWanted);
            addAction("Give Weapons", giveWeapons);
            addSubmenu("Model Changer", MODELS_PAGE);
            break;
        case MODELS_PAGE:
            addModel("Michael", "player_zero");
            addModel("Franklin", "player_one");
            addModel("Trevor", "player_two");
            addModel("Skater", "a_m_y_skater_01");
            addModel("Chop", "a_c_chop");
            addAction("Reset Original Character", resetOriginalCharacter);
            break;
        case VEHICLE_PAGE:
            addSubmenu("Vehicle Spawner", SPAWNER_PAGE);
            addSubmenu("Vehicle Customiser", CUSTOMIZER_PAGE);
            addAction("Repair Vehicle", repairVehicle);
            addAction("Clean Vehicle", cleanVehicle);
            addAction("Cyan Paint", cyanVehicle);
            addToggle("Spawn Inside Vehicle", &spawnInside);
            break;
        case SPAWNER_PAGE:
            addVehicle("Adder", "adder");
            addVehicle("Zentorno", "zentorno");
            addVehicle("Entity XF", "entityxf");
            addVehicle("T20", "t20");
            addVehicle("Sultan", "sultan");
            addVehicle("Elegy RH8", "elegy2");
            addVehicle("Banshee", "banshee");
            addVehicle("Infernus", "infernus");
            addVehicle("Buffalo", "buffalo");
            addVehicle("Futo", "futo");
            addVehicle("Bati 801", "bati");
            addVehicle("Buzzard", "buzzard2");
            break;
        case CUSTOMIZER_PAGE:
            addMod("Spoiler", 0);
            addMod("Front Bumper", 1);
            addMod("Rear Bumper", 2);
            addMod("Side Skirts", 3);
            addMod("Exhaust", 4);
            addMod("Frame", 5);
            addMod("Grille", 6);
            addMod("Hood", 7);
            addMod("Left Fender", 8);
            addMod("Right Fender", 9);
            addMod("Roof", 10);
            addMod("Engine", 11);
            addMod("Brakes", 12);
            addMod("Transmission", 13);
            addMod("Suspension", 15);
            addMod("Armour", 16);
            add("Turbo", VEHICLE_TURBO);
            break;
        case TELEPORT_PAGE:
            addAction("Waypoint", teleportWaypoint);
            addAction("Los Santos Airport", teleportAirport);
            addAction("Sandy Shores", teleportSandy);
            addAction("Mount Chiliad", teleportChiliad);
            addAction("Maze Bank Roof", teleportMazeBank);
            break;
        case WORLD_PAGE:
            addAction("Clear Weather", weatherClear);
            addAction("Extra Sunny", weatherSunny);
            addAction("Rain", weatherRain);
            addAction("Thunder", weatherThunder);
            addAction("Morning 08:00", timeMorning);
            addAction("Noon 12:00", timeNoon);
            addAction("Night 23:00", timeNight);
            break;
        case SETTINGS_PAGE:
            addToggle("Neon Accent", &neonAccent);
            addToggle("Spawn Inside Vehicle", &spawnInside);
            break;
        default:
            break;
        }
        if (optionCount > 0 && selected[page] >= optionCount)
            selected[page] = optionCount - 1;
        if (selected[page] < 0) selected[page] = 0;
    }

    void changeVehicleMod(int slot, int direction) {
        Vehicle v = currentVehicle();
        if (!ENTITY::DOES_ENTITY_EXIST(v)) {
            drawNotification("~r~Enter a vehicle first");
            return;
        }
        VEHICLE::SET_VEHICLE_MOD_KIT(v, 0);
        int count = VEHICLE::GET_NUM_VEHICLE_MODS(v, slot);
        if (count <= 0) {
            drawNotification("~r~No upgrades available");
            return;
        }
        int value = VEHICLE::GET_VEHICLE_MOD(v, slot) + direction;
        if (value < -1) value = count - 1;
        if (value >= count) value = -1;
        VEHICLE::SET_VEHICLE_MOD(v, slot, value, false);
    }

    void activate(Option& o, int direction) {
        switch (o.kind) {
        case SUBMENU:
            if (direction == 0) {
                parents[o.submenu] = page;
                page = o.submenu;
                playSound("SELECT");
            }
            break;
        case ACTION:
            if (direction == 0 && o.callback) { o.callback(); playSound("SELECT"); }
            break;
        case TOGGLE:
            if (o.toggle) {
                *o.toggle = !*o.toggle;
                // Immediately apply changes even if the player closes the menu.
                if (o.toggle == &godMode) PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), godMode);
                if (o.toggle == &invisible) ENTITY::SET_ENTITY_VISIBLE(me(), !invisible);
                if (o.toggle == &infiniteClip) WEAPON::SET_PED_INFINITE_AMMO_CLIP(me(), infiniteClip);
                playSound("SELECT");
            }
            break;
        case SPAWN_VEHICLE:
            if (direction == 0 && o.model) {
                queuedVehicle = GAMEPLAY::GET_HASH_KEY(o.model);
                if (!STREAMING::IS_MODEL_IN_CDIMAGE(queuedVehicle) ||
                    !STREAMING::IS_MODEL_A_VEHICLE(queuedVehicle)) {
                    queuedVehicle = 0;
                    drawNotification("~r~Model not on this game version");
                } else {
                    STREAMING::REQUEST_MODEL(queuedVehicle);
                    drawNotification("~b~Loading vehicle...");
                }
            }
            break;
        case CHANGE_MODEL:
            if (direction == 0 && o.model) {
                queuedPlayerModel = GAMEPLAY::GET_HASH_KEY(o.model);
                if (!STREAMING::IS_MODEL_IN_CDIMAGE(queuedPlayerModel)) {
                    queuedPlayerModel = 0;
                    drawNotification("~r~Model not installed");
                } else {
                    STREAMING::REQUEST_MODEL(queuedPlayerModel);
                    drawNotification("~b~Loading character...");
                }
            }
            break;
        case VEHICLE_MOD:
            changeVehicleMod(o.modSlot, direction == 0 ? 1 : direction);
            break;
        case VEHICLE_TURBO:
            {
                Vehicle v = currentVehicle();
                if (ENTITY::DOES_ENTITY_EXIST(v)) {
                    VEHICLE::SET_VEHICLE_MOD_KIT(v, 0);
                    bool enabled = VEHICLE::IS_TOGGLE_MOD_ON(v, 18);
                    VEHICLE::TOGGLE_VEHICLE_MOD(v, 18, !enabled);
                }
            }
            break;
        }
    }

    void tickPending() {
        if (queuedVehicle != 0) {
            STREAMING::REQUEST_MODEL(queuedVehicle);
            if (STREAMING::HAS_MODEL_LOADED(queuedVehicle)) {
                Ped p = me();
                if (ENTITY::DOES_ENTITY_EXIST(p)) {
                    Vector3 place = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(p, 0.0f, 6.0f, 0.5f);
                    Vehicle v = VEHICLE::CREATE_VEHICLE(queuedVehicle, place.x, place.y, place.z,
                                                        ENTITY::GET_ENTITY_HEADING(p), true, false);
                    if (ENTITY::DOES_ENTITY_EXIST(v)) {
                        VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(v);
                        if (spawnInside) PED::SET_PED_INTO_VEHICLE(p, v, -1);
                    } else {
                        drawNotification("~r~Vehicle creation failed");
                    }
                }
                STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(queuedVehicle);
                queuedVehicle = 0;
            }
        }
        if (queuedPlayerModel != 0) {
            STREAMING::REQUEST_MODEL(queuedPlayerModel);
            if (STREAMING::HAS_MODEL_LOADED(queuedPlayerModel)) {
                Ped p = me();
                if (ENTITY::DOES_ENTITY_EXIST(p)) {
                    if (originalPlayerModel == 0)
                        originalPlayerModel = ENTITY::GET_ENTITY_MODEL(p);
                    PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), queuedPlayerModel);
                    Ped newPed = me(); // SET_PLAYER_MODEL replaces the old ped.
                    if (ENTITY::DOES_ENTITY_EXIST(newPed))
                        PED::SET_PED_DEFAULT_COMPONENT_VARIATION(newPed);
                }
                STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(queuedPlayerModel);
                queuedPlayerModel = 0;
            }
        }
    }

    void tickToggles() {
        Ped p = me();
        if (!ENTITY::DOES_ENTITY_EXIST(p)) return;
        if (godMode) PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), true);
        if (noWanted) PLAYER::CLEAR_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID());
        if (invisible) ENTITY::SET_ENTITY_VISIBLE(p, false);
        if (infiniteClip) WEAPON::SET_PED_INFINITE_AMMO_CLIP(p, true);
    }

    void updateControls() {
        static int lastActionAt = 0;
        int now = GAMEPLAY::GET_GAME_TIMER();
        if (now < lastActionAt) lastActionAt = 0;
        if (now - lastActionAt < 120) return;

        if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, BUTTON_CIRCLE)) {
            if (page == HOME) opened = false;
            else page = parents[page];
            playSound("BACK");
            lastActionAt = now;
            return;
        }
        if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, DPAD_UP)) {
            selected[page]--;
            if (selected[page] < 0) selected[page] = optionCount - 1;
            playSound("NAV_UP_DOWN");
            lastActionAt = now;
            return;
        }
        if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, DPAD_DOWN)) {
            selected[page]++;
            if (selected[page] >= optionCount) selected[page] = 0;
            playSound("NAV_UP_DOWN");
            lastActionAt = now;
            return;
        }
        if (optionCount <= 0) return;
        Option& o = options[selected[page]];
        if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, DPAD_LEFT)) {
            if (o.kind == VEHICLE_MOD || o.kind == TOGGLE || o.kind == VEHICLE_TURBO) {
                activate(o, -1);
                lastActionAt = now;
            }
        } else if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, DPAD_RIGHT) &&
                   !CONTROLS::IS_CONTROL_PRESSED(0, BUTTON_R1)) {
            if (o.kind == VEHICLE_MOD || o.kind == TOGGLE || o.kind == VEHICLE_TURBO) {
                activate(o, 1);
                lastActionAt = now;
            }
        } else if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, BUTTON_X)) {
            activate(o, 0);
            lastActionAt = now;
        }
    }

    void render() {
        const float x1 = 0.075f, x2 = 0.385f;
        const float rowTop = 0.245f, rowH = 0.031f;
        const int accentR = neonAccent ? 0 : 150;
        const int accentG = neonAccent ? 230 : 150;
        const int accentB = neonAccent ? 255 : 150;

        drawRectXY(x1, 0.170f, x2, 0.239f, 9, 13, 22, 225);
        drawRectXY(x1, 0.233f, x2, 0.240f, accentR, accentG, accentB, 255);
        drawText("ESCTASY", 1, x1 + 0.012f, 0.175f, 0.50f, 0.50f, 255,255,255,255, false,false);
        char* name = "HOME";
        switch (page) {
        case PLAYER_PAGE: name = "PLAYER"; break;
        case MODELS_PAGE: name = "MODEL CHANGER"; break;
        case VEHICLE_PAGE: name = "VEHICLE"; break;
        case SPAWNER_PAGE: name = "VEHICLE SPAWNER"; break;
        case CUSTOMIZER_PAGE: name = "VEHICLE CUSTOMISER"; break;
        case TELEPORT_PAGE: name = "TELEPORT"; break;
        case WORLD_PAGE: name = "WORLD"; break;
        case SETTINGS_PAGE: name = "SETTINGS"; break;
        default: break;
        }
        drawText(name, 0, x1 + 0.012f, 0.216f, 0.25f, 0.25f, accentR, accentG, accentB, 255, false, false);
        int rows = optionCount < VISIBLE_ITEMS ? optionCount : VISIBLE_ITEMS;
        int& first = scroll[page];
        if (selected[page] < first) first = selected[page];
        if (selected[page] >= first + VISIBLE_ITEMS) first = selected[page] - VISIBLE_ITEMS + 1;
        if (first < 0) first = 0;
        drawRectXY(x1, rowTop, x2, rowTop + rows * rowH, 5, 9, 16, 205);

        for (int i = 0; i < rows; ++i) {
            int itemIndex = first + i;
            Option& o = options[itemIndex];
            float y = rowTop + i * rowH;
            bool active = itemIndex == selected[page];
            if (active) {
                drawRectXY(x1, y, x2, y + rowH, 19, 31, 46, 245);
                drawRectXY(x1, y, x1 + 0.004f, y + rowH, accentR, accentG, accentB, 255);
            }
            drawText(o.name, 0, x1 + 0.014f, y + 0.005f, 0.27f, 0.27f,
                     active ? accentR : 235, active ? accentG : 235, active ? accentB : 235,
                     255, false, false);
            char value[40] = {0};
            if (o.kind == SUBMENU) {
                strcpy(value, ">");
            } else if (o.kind == TOGGLE && o.toggle) {
                strcpy(value, *o.toggle ? "~g~ON" : "~r~OFF");
            } else if (o.kind == VEHICLE_TURBO) {
                Vehicle v = currentVehicle();
                strcpy(value, ENTITY::DOES_ENTITY_EXIST(v) ?
                       (VEHICLE::IS_TOGGLE_MOD_ON(v, 18) ? "~g~ON" : "~r~OFF") : "No car");
            } else if (o.kind == VEHICLE_MOD) {
                Vehicle v = currentVehicle();
                if (!ENTITY::DOES_ENTITY_EXIST(v)) strcpy(value, "No car");
                else {
                    VEHICLE::SET_VEHICLE_MOD_KIT(v, 0);
                    int total = VEHICLE::GET_NUM_VEHICLE_MODS(v, o.modSlot);
                    int index = VEHICLE::GET_VEHICLE_MOD(v, o.modSlot);
                    if (total <= 0) strcpy(value, "N/A");
                    else if (index == -1) strcpy(value, "Stock");
                    else snprintf(value, sizeof(value), "%d/%d", index + 1, total);
                }
            }
            if (value[0]) {
                drawText(value, 0, x2 - 0.018f, y + 0.005f, 0.25f, 0.25f,
                         255, 255, 255, 255, false, true);
            }
        }
        if (optionCount > VISIBLE_ITEMS) {
            float railY1 = rowTop, railY2 = rowTop + rows * rowH;
            float totalH = railY2 - railY1;
            float thumbH = totalH * VISIBLE_ITEMS / optionCount;
            float thumbY = railY1 + (totalH - thumbH) * selected[page] / (optionCount - 1);
            drawRectXY(x2 - 0.006f, railY1, x2 - 0.002f, railY2, 45, 55, 67, 210);
            drawRectXY(x2 - 0.006f, thumbY, x2 - 0.002f, thumbY + thumbH,
                       accentR, accentG, accentB, 255);
        }
        float footer = rowTop + rows * rowH;
        drawRectXY(x1, footer, x2, footer + 0.035f, 9, 13, 22, 225);
        char pageCount[24];
        snprintf(pageCount, sizeof(pageCount), "%d / %d",
                 optionCount > 0 ? selected[page] + 1 : 0, optionCount);
        drawText(pageCount, 0, x2 - 0.014f, footer + 0.007f, 0.24f, 0.24f,
                 255, 255, 255, 255, false, true);
        drawText("X Select    O Back", 0, x1 + 0.012f, footer + 0.007f,
                 0.24f, 0.24f, 200, 200, 200, 255, false, false);
    }

    void tick() {
        // Run model streaming regardless of whether the menu is currently visible.
        tickPending();
        static int lastToggleTick = 0;
        if (IsTimerReady(100, &lastToggleTick)) tickToggles();

        if (!opened) {
            if (CONTROLS::IS_CONTROL_PRESSED(0, BUTTON_R1) &&
                CONTROLS::IS_CONTROL_JUST_PRESSED(0, DPAD_RIGHT)) {
                opened = true;
                page = HOME;
                playSound("SELECT");
            }
            return;
        }
        exclusiveInputs();
        static int lastDrawTick = 0;
        if (!IsTimerReady(15, &lastDrawTick)) return;
        buildPage();
        updateControls();
        if (opened) {
            buildPage(); // After following a submenu or returning to its parent.
            render();
        }
    }
}

void draw_menu() {
    esctasy::tick();
}
