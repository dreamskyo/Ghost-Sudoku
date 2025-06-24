# Compiler settings (windows可改為console，console方便debug)
CXX = g++ --std=c++17
CXXFLAGS = -w -Wl,-subsystem,windows

# SDL paths (請確認路徑中無中文)
SDL_INCLUDE = -I local/SDL3-3.2.12/x86_64-w64-mingw32/include -I local/SDL3_image-3.2.4/x86_64-w64-mingw32/include -I local/SDL3_mixer/include
SDL_LIB = -L local/SDL3-3.2.12/x86_64-w64-mingw32/lib -L local/SDL3_image-3.2.4/x86_64-w64-mingw32/lib -L local/SDL3_mixer/lib

# Libraries (請記得修改環境變數)
LIB = -lSDL3 -lSDL3_image -lSDL3_mixer

# Target executable name
TARGET = Ghost-sudoku

# Source files
SRCS = DLX.cpp mSDL.cpp main.cpp

# Build rules
all: $(TARGET)
$(TARGET): $(SRCS)
	$(CXX) $(SRCS) $(SDL_INCLUDE) $(SDL_LIB) $(CXXFLAGS) $(LIB) -o $(TARGET)

.PHONY: clean
clean:
# for windos
	del $(TARGET).exe