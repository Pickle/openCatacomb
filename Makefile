# openCatacomb makefile

PROGRAM = opencatacomb

# Build type
#BUILDTYPE = debug
BUILDTYPE = release

# Compiler flags
BASE_FLAGS   = -g -Wall -Wextra -O3 -fsigned-char
CCFLAGS      = -std=c99
CXXFLAGS     = -std=c++98

# Linker flags
BASE_LDFLAGS = -L$(LIBRARY) -lSDL_mixer -lSDL

# Target compiler options
ifeq ($(BUILDTARGET),PANDORA)
PREFIX   = /data/devel/toolchains/pandora/arm-2011.09
TOOLS    = bin
TARGET   = arm-none-linux-gnueabi-
OGLTYPE  = opengles2
INCLUDE  = $(PREFIX)/usr/include
LIBRARY  = $(PREFIX)/usr/lib
NEON     = -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -ffast-math -fsingle-precision-constant -fno-inline-functions
BASE_FLAGS  += $(NEON) -DPANDORA
LDFLAGS  = $(BASE_LDFLAGS) -lpng12 -lz -ltiff -lmad -ljpeg -lts -lm
else
ifeq ($(BUILDTARGET),CAANOO)
PREFIX   = /data/devel/toolchains/caanoo/GPH_SDK
TOOLS    = tools/gcc-4.2.4-glibc-2.7-eabi/bin
TARGET   = arm-gph-linux-gnueabi-
OGLTYPE  = opengles1
INCLUDE  = $(PREFIX)/DGE/include
LIBRARY  = $(PREFIX)/DGE/lib/target
BASE_FLAGS  += -DCAANOO
LDFLAGS  = $(BASE_LDFLAGS) -lpng12 -lz
else
ifeq ($(BUILDTARGET),WIZ)
PREFIX   = /data/devel/toolchains/openwiz/arm-openwiz-linux-gnu
TOOLS    = bin
TARGET   = arm-openwiz-linux-gnu-
OGLTYPE  = opengles1
INCLUDE  = $(PREFIX)/include
LIBRARY  = $(PREFIX)/lib
BASE_FLAGS  += -DWIZ
LDFLAGS  = $(BASE_LDFLAGS) -lpng12 -lz -lmad -lvorbisidec
else
ifeq ($(BUILDTARGET),WIN32)
PREFIX   = /usr
TOOLS    = bin
TARGET   = i686-w64-mingw32-
OGLTYPE  = opengl2
INCLUDE  = $(PREFIX)/i686-w64-mingw32/sys-root/i686-w64-mingw32/include
LIBRARY  = $(PREFIX)/i686-w64-mingw32/sys-root/i686-w64-mingw32/bin
BASE_FLAGS  += -DWIN32
LDFLAGS  = $(BASE_LDFLAGS) -static-libgcc -mwindows -lmingw32 -lSDLmain -lpng12-0 -lzlib1
else # default linux
BUILDTARGET = LINUX
PREFIX   = /usr
TOOLS    = bin
TARGET   =
OGLTYPE  = opengles2
INCLUDE  = $(PREFIX)/include
LIBRARY  = $(PREFIX)/lib
LDFLAGS  = $(BASE_LDFLAGS)
endif # pandora
endif # caanoo
endif # wiz
endif # win32

# Assign includes
BASE_FLAGS  += -Isrc -I$(INCLUDE) -I$(INCLUDE)/SDL

# Source files
CXX_SRCS = frontend.cpp

CC_SRCS  = catacomb.c \
           cat_play.c \
           cpanel.c   \
           editor.c   \
           objects.c  \
           system.c   \
           util.c     \
           input.c    \
           sound.c    \
           video.c    \
           opengl.c   \
           md5.c      \
           unlzexe.c

# Debug Configuration
ifeq ($(BUILDTYPE),debug)
BASE_FLAGS  += -DDEBUG
endif

# OpenGL Configuration
ifeq ($(OGLTYPE),opengl1)
BASE_FLAGS  += -DUSE_GL1 -I$(INCLUDE)/GL
LDFLAGS     += -lGL
else
ifeq ($(OGLTYPE),opengl2)
CC_SRCS     += shader.c
BASE_FLAGS  += -DUSE_GL2 -DUSE_FBO -I$(INCLUDE)/GL
ifeq ($(BUILDTARGET),WIN32)
LDFLAGS     += -lopengl32
else
LDFLAGS     += -lGL
endif

else # GLES Configs

ifeq ($(OGLTYPE),opengles1)

ifeq ($(BUILDTARGET),PANDORA)
CC_SRCS     += eglport.c
BASE_FLAGS  += -DUSE_GLES1 -DUSE_EGL_RAW -I$(INCLUDE)/GLES -I$(INCLUDE)/EGL
LDFLAGS     += -lGLES_CM -lIMGegl -lEGL -lsrv_um -lX11 -lXau -lXdmcp 
else
ifeq ($(BUILDTARGET),CAANOO)
CC_SRCS     += eglport.c gph.c
BASE_FLAGS  += -DUSE_GLES1 -DUSE_EGL_RAW -I$(INCLUDE)/OpenGLES -I$(INCLUDE)/EGL
LDFLAGS     += -lopengles_lite
else
ifeq ($(BUILDTARGET),WIZ)
CC_SRCS     += eglport.c gph.c
BASE_FLAGS  += -DUSE_GLES1 -DUSE_EGL_RAW -I$(INCLUDE)/GLES -I$(INCLUDE)/EGL
LDFLAGS     += -lopengles_lite -lglport
else
CC_SRCS     += eglport.c
BASE_FLAGS  += -DUSE_GL1 -DUSE_GLES1 -DUSE_EGL_SDL -I$(INCLUDE)/GLES2 -I$(INCLUDE)/EGL
LDFLAGS     += -lGLESv1_CM -lEGL -lX11
endif # WIZ
endif # CAANOO
endif # PANDORA

else
ifeq ($(OGLTYPE),opengles2)

ifeq ($(BUILDTARGET),PANDORA)
CC_SRCS     += eglport.c shader.c
BASE_FLAGS  += -DUSE_GL2 -DUSE_GLES2 -DUSE_EGL_RAW -DUSE_FBO -I$(INCLUDE)/GLES2 -I$(INCLUDE)/EGL
LDFLAGS     += -lGLESv2 -lIMGegl -lEGL -lsrv_um -lX11 -lXau -lXdmcp 
else
CC_SRCS     += eglport.c shader.c
BASE_FLAGS  += -DUSE_GL2 -DUSE_GLES2 -DUSE_EGL_SDL -DUSE_FBO -I$(INCLUDE)/GLES2 -I$(INCLUDE)/EGL
LDFLAGS     += -lGLESv2 -lEGL -lX11
endif # PANDORA

endif # opengles2
endif # opengles1
endif # opengl2
endif # opengl1

CCFLAGS  += $(BASE_FLAGS)
CXXFLAGS += $(BASE_FLAGS)

# Assign paths to binaries/sources/objects
BUILD      = build
SRCDIR     = src
OBJDIR     = $(BUILD)/objs/$(BUILDTYPE)

CC_SRCS    := $(addprefix $(SRCDIR)/,$(CC_SRCS))
OBJS       := $(addprefix $(OBJDIR)/,$(CC_SRCS:.c=.o))
CXX_SRCS   := $(addprefix $(SRCDIR)/,$(CXX_SRCS))
OBJS       += $(addprefix $(OBJDIR)/,$(CXX_SRCS:.cpp=.o))

PROGRAM    := $(addprefix $(BUILD)/,$(PROGRAM)) 

# Assign Tools
CC  = $(PREFIX)/$(TOOLS)/$(TARGET)gcc
CXX = $(PREFIX)/$(TOOLS)/$(TARGET)g++
AR  = $(PREFIX)/$(TOOLS)/$(TARGET)ar

# Build rules
all: setup $(PROGRAM)

setup:
	mkdir -p $(OBJDIR)/$(SRCDIR)

$(PROGRAM): $(OBJS)
	$(CXX) $(CCFLAGS) -o $(PROGRAM) $(OBJS) $(LDFLAGS) 

$(OBJDIR)/$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CCFLAGS) -c $< -o $@

$(OBJDIR)/$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@   

clean:
	rm -f $(PROGRAM) $(OBJS)
