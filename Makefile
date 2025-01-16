INC_DIR = inc
INCLUDES = $(wildcard $(INC_DIR)/*.inc)

main: $(INCLUDES) main.s
	nasm -f elf64 -o main.o main.s -g -w+all -I$(INC_DIR)/
	ld -o main main.o


