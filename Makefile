# Compiler settings
CXX = g++ --std=c++17
CXXFLAGS = -w -Wl,--stack,67108864 -Wl,-subsystem,console 

# SDL paths (不知為何，在我的電腦中只能使用相對路徑)
SDL_INCLUDE = -I ../../projects/SDL/SDL3-3.2.12/x86_64-w64-mingw32/include -I ../../projects/SDL/SDL3_image-3.2.4/x86_64-w64-mingw32/include
SDL_LIB = -L ../../projects/SDL/SDL3-3.2.12/x86_64-w64-mingw32/lib -L ../../projects/SDL/SDL3_image-3.2.4/x86_64-w64-mingw32/lib

# Libraries (請記得修改環境變數)
LIB = -lSDL3 -lSDL3_image 

# Target executable name
TARGET = test

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