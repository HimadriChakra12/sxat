NPROC   := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
MAKEFLAGS += -j$(NPROC)

CCACHE  := $(shell command -v ccache 2>/dev/null)
CC      := $(CCACHE) clang

target:= sxat

src_dir := src
obj_dir := .o
dep_dir := .d

srcs := $(shell find $(src_dir) -name "*.c" -type f)
hdrs := $(shell find $(src_dir) -name "*.h" -type f)
objs := $(patsubst %,$(obj_dir)/%.o,$(basename $(srcs)))
deps := $(patsubst .o/%,.d/%,$(objs:.o=.d))

PKGCONFIG := pkg-config

CFLAGS = -Wall -g -std=c99 -pedantic \
		 -I $(src_dir) \
		 $(shell $(PKGCONFIG) --cflags sdl2 SDL2_image SDL2_ttf) \
		 $(shell $(PKGCONFIG) --cflags fontconfig) \
		 $(shell $(PKGCONFIG) --cflags libpng)

LDFLAGS = $(shell $(PKGCONFIG) --libs sdl2 SDL2_image SDL2_ttf) \
		  $(shell $(PKGCONFIG) --libs fontconfig) \
		  $(shell $(PKGCONFIG) --libs libpng)

APP :=/usr/local/bin/

