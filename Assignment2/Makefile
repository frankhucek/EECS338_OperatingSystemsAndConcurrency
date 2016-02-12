CC = gcc
SRC = FrankHucek_Assign2.c
LIB = lib_safe.c
OUT = output_assign_2
all: $(SRC) $(LIB)
	@$(CC) $(SRC) $(LIB) -o $(OUT)
clean: $(OUT)
	@rm $(OUT)
debug: $(SRC) $(LIB)
	@$(CC) -g $(SRC) $(LIB) -o $(OUT)
