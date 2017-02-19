EXE :=
ifeq ($(OS),Windows_NT)
EXE := .exe
endif

.PHONY: all clean

all: delegate$(EXE) delegate_compat$(EXE)

delegate$(EXE): src/delegate.cpp src/delegate.h main.cpp
	$(CXX) -o delegate$(EXE) src/delegate.cpp main.cpp  -std=c++11 -O3 -pthread -lstdc++ -D LOG_DELEGATES

delegate_compat$(EXE): src/delegate.cpp src/delegate.h main.cpp
	$(CXX) -o delegate_compat$(EXE) src/delegate.cpp main.cpp  -std=c++11 -O3 -pthread -lstdc++ -D FORCE_COMPATIBLE

delegate_sanitizer$(EXE): src/delegate.cpp src/delegate.h main.cpp
	$(CXX) -o delegate_s_undefined$(EXE) src/delegate.cpp main.cpp  -std=c++11 -O0 -pthread -lstdc++ -fsanitize=undefined -D LOG_DELEGATES
	$(CXX) -o delegate_s_address$(EXE) src/delegate.cpp main.cpp  -std=c++11 -O0 -pthread -lstdc++ -fsanitize=address -D LOG_DELEGATES
	$(CXX) -o delegate_s_undefined_opt$(EXE) src/delegate.cpp main.cpp  -std=c++11 -O3 -pthread -lstdc++ -fsanitize=undefined -D LOG_DELEGATES
	$(CXX) -o delegate_s_address_opt$(EXE) src/delegate.cpp main.cpp  -std=c++11 -O3 -pthread -lstdc++ -fsanitize=address -D LOG_DELEGATES

delegate_compat_sanitizer$(EXE): src/delegate.cpp src/delegate.h main.cpp
	$(CXX) -o delegate_compat_s_undefined$(EXE) src/delegate.cpp main.cpp  -std=c++11 -O0 -pthread -lstdc++ -fsanitize=undefined -D FORCE_COMPATIBLE
	$(CXX) -o delegate_compat_s_address$(EXE) src/delegate.cpp main.cpp  -std=c++11 -O0 -pthread -lstdc++ -fsanitize=address -D FORCE_COMPATIBLE
	$(CXX) -o delegate_compat_s_undefined_opt$(EXE) src/delegate.cpp main.cpp  -std=c++11 -O3 -pthread -lstdc++ -fsanitize=undefined -D FORCE_COMPATIBLE
	$(CXX) -o delegate_compat_s_address_opt$(EXE) src/delegate.cpp main.cpp  -std=c++11 -O3 -pthread -lstdc++ -fsanitize=address -D FORCE_COMPATIBLE
	
clean:
	@echo Cleaning...
	-@rm -rf delegate$(EXE) delegate_compat$(EXE) delegate_s_undefined$(EXE) delegate_s_undefined_opt$(EXE) delegate_s_address$(EXE) delegate_s_address_opt$(EXE) delegate_compat_s_undefined$(EXE) delegate_compat_s_undefined_opt$(EXE) delegate_compat_s_address$(EXE) delegate_compat_s_address_opt$(EXE)
