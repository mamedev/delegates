EXE :=
ifeq ($(OS),Windows_NT)
EXE := .exe
endif

.PHONY: all clean

all: delegate$(EXE) delegate_compat$(EXE)

delegate$(EXE): src/delegate.cpp src/delegate.h mame/devdelegate.cpp mame/devdelegate.h mame/emu.h main.cpp
	$(CXX) -o delegate$(EXE) src/delegate.cpp mame/devdelegate.cpp main.cpp  -std=c++11 -static -O3 -pthread -lstdc++

delegate_compat$(EXE): src/delegate.cpp src/delegate.h mame/devdelegate.cpp mame/devdelegate.h mame/emu.h main.cpp
	$(CXX) -o delegate_compat$(EXE) src/delegate.cpp mame/devdelegate.cpp main.cpp  -std=c++11 -static -O3 -pthread -lstdc++ -D FORCE_COMPATIBLE
	
clean:
	@echo Cleaning...
	-@rm -rf delegate$(EXE) delegate_compat$(EXE)
