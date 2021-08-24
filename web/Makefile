CX = clang++
CF = -Wall --target=wasm32 -nostdlib -flto -fno-exceptions
CF += -Oz
CF += -fno-threadsafe-statics
CF += -fvisibility=hidden
CF += -ffunction-sections
CF += -fdata-sections
CF += -DWASM

LL = wasm-ld-11
LF = --no-entry --strip-all --import-memory --export-dynamic
LF += --lto-O3
LF += -O3
#LF += -z,stack-size=8388608
#LF += --initial-memory=131072
#LF += -error-limit=0
LF += --gc-sections
LF += --allow-undefined-file=src/wasm.sym

CF += -I src

SRC = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp)
OBJ = $(SRC:src/%.cpp=obj/%.o)
TRG = blob.wasm


all: $(TRG)


obj/%.o: src/%.cpp Makefile
	@mkdir -p $(dir $@)
	$(CX) $(CF) $< -c -o $@


$(TRG): $(OBJ) Makefile
	$(LL) $(LF) $(OBJ) -o $@


clean:
	rm -rf obj/ $(TRG)
