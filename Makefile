CC = gcc -std=c11
FLAGS = -Iinclude -m64
RELEASE_FLAGS = -Ofast -Wl,-subsystem,windows
DEBUG_FLAGS = -Wall -O0 -ggdb -g3 -Wno-unused-parameter -Wno-unused-function
LINKS := opengl32
LINKS := $(addprefix -l, $(LINKS))

LINKS += `pkg-config --libs glfw3`
LINKS += `pkg-config --libs glew`
LINKS += `pkg-config --libs portaudio-2.0`
LINKS += `pkg-config --libs libuv`

SHADERS_DIRECTORY = include/ae/shaders
SHADERS_LIST = $(wildcard $(SHADERS_DIRECTORY)/*.glsl)

AE_OBJS = $(patsubst src/ae/%.c,obj/ae/%.o,$(wildcard src/ae/*.c))
GAME_OBJS = $(patsubst src/game/%.c,obj/game/%.o,$(wildcard src/game/*.c))

build: $(SHADERS_DIRECTORY)/shaders.h bin/o.exe
release: clean $(SHADERS_DIRECTORY)/shaders.h bin/release.exe

bin/o.exe: src/main.c ${AE_OBJS} ${GAME_OBJS}
	$(CC) $^ -o bin/o.exe -Wextra $(DEBUG_FLAGS) $(FLAGS) $(LINKS)

bin/release.exe: src/main.c ${AE_OBJS} ${GAME_OBJS}
	$(CC) $^ -o bin/release.exe -Wextra $(RELEASE_FLAGS) $(FLAGS) $(LINKS)

obj/ae/%.o: src/ae/%.c include/ae/%.h
	$(CC) -c $< -DAE_DEBUG_LOGS $(DEBUG_FLAGS) $(FLAGS) -o $@

obj/game/%.o: src/game/%.c include/game/%.h
	$(CC) -c $< -DAE_DEBUG_LOGS $(DEBUG_FLAGS) $(FLAGS) -o $@

$(SHADERS_DIRECTORY)/shaders.h: $(SHADERS_LIST)
	@printf "Updating $(SHADERS_DIRECTORY)/shaders.h...\n"
	@printf "#ifndef __SHADER_H__\n#define __SHADER_H__\n" > $@
	@cd $(dir $@); set -- $(foreach x, $^, $(notdir $x)); for x; do xxd -i "$$x" >> $(notdir $@); done
	@printf "\n#endif\n" >> $@`

obj/ae/shader.o: src/ae/shader.c include/ae/shader.h include/ae/texture_defs_str.h $(SHADERS_DIRECTORY)/shaders.h
	$(CC) -c $< $(DEBUG_FLAGS) $(FLAGS) -o $@

include/ae/texture_defs_str.h: include/ae/texture_defs.h
	@printf "Updating src/ae/texture_defs_str.h...\n"
	@printf "#ifndef __TEXTURE_DEFS_STR_H__\n#define __TEXTURE_DEFS_STR_H__\n" > $@
	@xxd -i "$^" >> $@
	@printf "\n#endif\n" >> $@

# obj/text.o: src/ae/text.c include/ae/test.h
# 	$(CC) -c $<

# obj/%.o: src/common/%.c src/common/%.h
# 	$(CC) -c $< -DAE_DEBUG_LOGS $(FLAGS) -o $@

# obj/%.o: src/math/%.c src/math/%.h
# 	$(CC) -c $< -DAE_DEBUG_LOGS $(FLAGS) -o $@

# obj/%.o: src/system/%.c src/system/%.h
# 	$(CC) -c $< -DAE_DEBUG_LOGS $(FLAGS) -o $@

# obj/%.o: src/graphics/%.c src/graphics/%.h
# 	$(CC) -c $< -DAE_DEBUG_LOGS $(FLAGS) -o $@

run: build
	@cd bin; ./o.exe

debug: build
	@cd bin; gdb -q o

clean:
	rm include/ae/texture_defs_str.h
	rm $(SHADERS_DIRECTORY)/shaders.h
	rm obj/game/*
	rm obj/ae/*