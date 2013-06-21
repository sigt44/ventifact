
#Toolchain defines, make sure these are compatible with the system architecture.
TOOLCHAIN_PATH = /usr
TOOLCHAIN_INCLUDE = /usr/include
SDL_INCLUDE_PATH = /usr/include/SDL
SDL_LIB_PATH = /usr/lib

#C Code defines
SYSTEM_TYPE = LINUX #LINUX, WINDOWS, PANDORA, GP2X or DINGOO
SCREEN_BIT = SCREEN_BIT_32 #SCREEN_BIT_32, SCREEN_BIT_16
MEMORY_TRACKING = #-DMEM_TRACK -DMEM_PRINTF -DMEM_FILE -DMEM_PRINTF_V -DMEM_FILE_V
FILE_LOGGING = FILE_LOGGING #FILE_LOGGING NO_LOG


TARGET = libVentEngine-Linux.a
OBJS= BaseState.o\
 Camera.o\
 Collision.o\
 Controls.o\
 DataStruct.o\
 File.o\
 Flag.o\
 Font.o\
 Maths.o\
 Memory.o\
 RK4.o\
 Screenshot.o\
 Sound.o\
 Vector2D.o\
 Vector2DInt.o\
 Common/HashTable.o\
 Common/List.o\
 Common/ListWatcher.o\
 Common/Fuzzy/FuzzyLV.o\
 Common/Fuzzy/FuzzyManager.o\
 Common/Fuzzy/FuzzySet.o\
 Graphics/Animation.o\
 Graphics/DrawManager.o\
 Graphics/Graphics.o\
 Graphics/Pixel.o\
 Graphics/PixelManager.o\
 Graphics/Sprite.o\
 Graphics/SpriteEffects.o\
 Graphics/SpriteManager.o\
 Graphics/Surface.o\
 Graphics/SurfaceManager.o\
 Graphics/Effects/Blink.o\
 Graphics/Effects/FadeIn.o\
 Graphics/Effects/FadeOut.o\
 Graphics/Effects/Slide.o\
 Graphs/Edge.o\
 Graphs/Graph.o\
 Graphs/Node.o\
 Graphs/Search/DijkstraSearch.o\
 GUI/Button.o\
 GUI/ButtonChain.o\
 GUI/ButtonChooser.o\
 GUI/ButtonFunc.o\
 GUI/ButtonPaths.o\
 GUI/ButtonScroll.o\
 GUI/Pointer.o\
 GUI/Spine.o\
 GUI/SpineEntity.o\
 GUI/TextBox.o\
 GUI/TextInput.o\
 GUI/UiShape.o\
 Kernel/Kernel.o\
 Kernel/Kernel_Init.o\
 Kernel/Kernel_State.o\
 Time/Fps.o\
 Time/Timer.o


CC=$(TOOLCHAIN_PATH)/bin/gcc
AR=$(TOOLCHAIN_PATH)/bin/ar
STRIP=$(TOOLCHAIN_PATH)/bin/strip

DEFAULT_CFLAGS = #--sysroot=$(TOOLCHAIN_PATH)

MORE_CFLAGS = \
-I. -I$(TOOLCHAIN_PATH)/usr/include \
 -I$(TOOLCHAIN_PATH)/usr/include/SDL \
 -I$(SDL_INCLUDE_PATH) \
 -I$(TOOLCHAIN_INCLUDE) \
 -O3 -fsigned-char -ffast-math -fomit-frame-pointer -fno-strength-reduce \
 -DSOUNDS \
 -D$(SYSTEM_TYPE) \
 -D$(SCREEN_BIT) \
 -D$(FILE_LOGGING) \
 $(MEMORY_TRACKING)

CFLAGS = $(DEFAULT_CFLAGS) $(MORE_CFLAGS) 
LDFLAGS = -s

LIBS += -L$(TOOLCHAIN_PATH)/usr/lib \
-L$(SDL_LIB_PATH) \
-lSDL -lSDL_image -lSDL_ttf -lSDL_mixer\

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(AR) rcs $(TARGET) $(OBJS)

install: $(TARGET)
	cp $< ../bin

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)

ctags: 
	ctags *[ch]
