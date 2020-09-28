MS_DIR = src/MeanscriptCpp
CMD_DIR = src/MeanscriptCmd
DEMO_DIR = src/Demo

CMD_SRC = $(CMD_DIR)/MeanscriptCmd.cpp
DEMO_SRC = $(DEMO_DIR)/DemoMain.cpp
SRC = $(MS_DIR)/MS.cpp
SRC += $(wildcard $(MS_DIR)/core/*.cpp)
SRC += $(wildcard $(MS_DIR)/pub/*.cpp)

CC = g++
FLAGS = -std=c++14 -I$(CMD_DIR) -I$(MS_DIR) -I$(MS_DIR)/core -I$(MS_DIR)/pub
RELEASE_TARGET_EXE = bin/mean
DEBUG_TARGET_EXE = bin/meandbg
DEMO_TARGET_EXE = bin/meandemo
RELEASE_FLAGS = -Os -s -DMS_RELEASE
DEBUG_FLAGS = -fsanitize=leak -fsanitize=address -DMS_DEBUG

release: $(SRC) $(CMD_SRC)
	@echo --- ByteAutomata RELEASE ---
	$(CC) $(FLAGS) $(RELEASE_FLAGS) $(SRC) $(CMD_SRC) -o $(RELEASE_TARGET_EXE) -Wall
	
debug: $(SRC) $(CMD_SRC)
	@echo --- ByteAutomata DEBUG ---
	$(CC) $(FLAGS) $(DEBUG_FLAGS) $(SRC) $(CMD_SRC) -o $(DEBUG_TARGET_EXE) 

demo: $(SRC) $(DEMO_SRC)
	@echo --- ByteAutomata DEMO ---
	$(CC) $(FLAGS) $(DEBUG_FLAGS) $(SRC) $(DEMO_SRC) -o $(DEMO_TARGET_EXE) 
	
clean:
	@echo --- TODO: ByteAutomata CLEAN ---
	#rm $(TARGET_EXE)
