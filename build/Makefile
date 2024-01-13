SHELL := pwsh.exe
.SHELLFLAGS := -NoProfile -Command

CXX := g++
CXXFLAGS := -std=c++11 -Wall -g
LINKER := -I ../header/ -L ../source/ -lraylib -lopengl32 -lgdi32 -lwinmm

OBJS := *.o
SRCS := ../source/*.cpp
HEADERS := ../header/*.h

ifeq ($(UI), 1)
	LINKER += -mwindows
endif

output: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o output.exe $(LINKER)

$(OBJS): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $(SRCS)

run:
	start cmd "/c ; output & pause"

clean:
	rm *.o , output.exe