CC=arm-xilinx-linux-gnueabi-gcc
AR=arm-xilinx-linux-gnueabi-gcc-ar

SRC_DIR=./src
OBJ_DIR=./obj
DST_DIR=./lib

DST=$(DST_DIR)/libacclib.a

CFLAGS	=-fPIC
COMPILE	=-c $(CFLAGS) \
		 -I./include

$(DST): $(OBJ_DIR)/acclib.o 
	$(AR) -rcs $@ $(OBJ_DIR)/acclib.o 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(COMPILE) $< -o $@

clean:
	rm -rf $(DST)
	rm -rf ./obj/*
	
