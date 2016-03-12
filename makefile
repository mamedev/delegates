EXE :=
ifeq ($(OS),Windows_NT)
EXE := .exe
endif

.PHONY: all clean

all: delegate$(EXE)

delegate$(EXE): src/delegate.cpp src/delegate.h src/devdelegate.cpp src/devdelegate.h src/emu.h main.cpp
	$(CXX) -o delegate$(EXE) src/delegate.cpp src/devdelegate.cpp main.cpp  -std=c++11 -static -O3 -pthread -lstdc++

clean:
	@echo Cleaning...
	-@rm -rf delegate$(EXE)
