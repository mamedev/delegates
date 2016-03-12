EXE :=
ifeq ($(OS),Windows_NT)
EXE := .exe
endif

.PHONY: all clean

all: delegate$(EXE)

delegate$(EXE): src/delegate.cpp main.cpp
	$(CXX) -o delegate$(EXE) src/delegate.cpp main.cpp  -std=c++11 -static -O3 -pthread

clean:
	@echo Cleaning...
	-@rm -rf delegate$(EXE)
