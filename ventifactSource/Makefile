
#Toolchain defines, make sure these are compatible with the target system architecture.
TOOLCHAIN_PATH = /usr
TOOLCHAIN_INCLUDE = /usr/include
SDL_INCLUDE_PATH = /usr/include/SDL
SDL_LIB_PATH = /usr/lib
ENGINE_INCLUDE = /home/l449/Programming/Engine/engineSource
ENGINE_LIB = /home/l449/Programming/Engine/bin/libVentEngine-Linux.a

#C Code defines
SYSTEM_TYPE = LINUX #LINUX, WINDOWS, PANDORA, GP2X or DINGOO
SCREEN_BIT = SCREEN_BIT_32 #SCREEN_BIT_32, SCREEN_BIT_16

TARGET = Ventifact
OBJS= main.o\
 Config.o\
 ControlMap.o\
 Campaign.o\
 Bullet_Attributes.o\
 Bullets.o\
 Effects.o\
 HealthBar.o\
 Images.o\
 Options.o\
 Player.o\
 Sounds.o\
 Target.o\
 Weapon.o\
 Buildings/Buildings.o\
 Buildings/BuildingBar.o\
 Level/Tiles.o\
 Level/Level.o\
 Level/LevelFile.o\
 Level/Sector.o\
 Ai/Ai.o\
 Ai/Ai_State.o\
 Ai/Ai_Side.o\
 Ai/Ai_Steering.o\
 Ai/Ai_Building.o\
 Ai/AiScan.o\
 Ai/Units/Ai_UnitMove.o\
 Ai/Units/Ai_UnitFunction.o\
 Ai/Units/Ai_UnitAttack.o\
 Editor/LevelEditor.o\
 Editor/LevelMenu.o\
 Editor/MiscMenu.o\
 Editor/NodeMenu.o\
 Editor/ShortcutMenu.o\
 Editor/SupplyMenu.o\
 Editor/TileMenu.o\
 Units/Attributes.o\
 Units/Units.o\
 Units/Supply.o\
 Menus/MainMenu.o\
 Menus/MenuAttributes.o\
 Menus/ControlMenu.o\
 Menus/OptionsMenu.o\
 Menus/UpgradeMenu.o\
 Menus/GameMenu.o\
 Game/Rules.o\
 Game/UnitMenu.o\
 Game/HUD.o\
 Game/VentGame.o\
 Game/State.o\
 Game/Sides.o\
 Game/Stats.o

CC=$(TOOLCHAIN_PATH)/bin/gcc
STRIP=$(TOOLCHAIN_PATH)/bin/strip

DEFAULT_CFLAGS = #--sysroot=$(TOOLCHAIN_PATH)

MORE_CFLAGS = \
-I. -I$(TOOLCHAIN_PATH)/usr/include \
 -I$(TOOLCHAIN_PATH)/usr/include/SDL \
 -I$(SDL_INCLUDE_PATH) \
 -I$(TOOLCHAIN_INCLUDE) \
 -I$(ENGINE_INCLUDE)\
 -O3 -fsigned-char -ffast-math -fomit-frame-pointer -fno-strength-reduce \
 -DSOUNDS \
 -D$(SYSTEM_TYPE) \
 -D$(SCREEN_BIT) \

CFLAGS = $(DEFAULT_CFLAGS) $(MORE_CFLAGS) 
LDFLAGS = -s

LIBS += -L$(TOOLCHAIN_PATH)/usr/lib \
-L$(SDL_LIB_PATH) \
-lSDL -lSDL_image -lSDL_ttf -lSDL_mixer\
 $(ENGINE_LIB)



.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJS) $(LIBS) -o $(TARGET) && $(STRIP) $(TARGET)

install: $(TARGET)
	cp $< ../bin

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)

ctags: 
	ctags *[ch]
