MS_DIR = src/MeanscriptCpp
CMD_DIR = src/MeanscriptCmd

SRC = $(CMD_DIR)/MeanscriptCmd.cpp
SRC += $(MS_DIR)/MS.cpp
SRC += $(wildcard $(MS_DIR)/core/*.cpp)
SRC += $(wildcard $(MS_DIR)/pub/*.cpp)

CC = g++
FLAGS = -std=c++14 -I$(CMD_DIR) -I$(MS_DIR) -I$(MS_DIR)/core -I$(MS_DIR)/pub
RELEASE_TARGET_EXE = bin/mean
DEBUG_TARGET_EXE = bin/meandbg
RELEASE_FLAGS = -Os -s -DMS_RELEASE
DEBUG_FLAGS = -fsanitize=leak -fsanitize=address -DMS_DEBUG

release: $(SRC)
	@echo --- ByteAutomata RELEASE ---
	$(CC) $(FLAGS) $(RELEASE_FLAGS) $(SRC) -o $(RELEASE_TARGET_EXE) 
	
debug: $(SRC)
	@echo --- ByteAutomata DEBUG ---
	$(CC) $(FLAGS) $(DEBUG_FLAGS) $(SRC) -o $(DEBUG_TARGET_EXE) 

demo:
	@echo --- TODO: ByteAutomata DEMO ---
	#rm $(TARGET_EXE)
	
clean:
	@echo --- TODO: ByteAutomata CLEAN ---
	#rm $(TARGET_EXE)
