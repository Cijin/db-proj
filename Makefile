TARGET_CLIENT = bin/client
TARGET_SRV = bin/server

SRC_SRV = $(wildcard src/server/*c)
OBJ_SRV = $(SRC_SRV:src/server/%.c=obj/server/%.o) 

SRC_CLIENT = $(wildcard src/client/*c)
OBJ_CLIENT = $(SRC_CLI:src/client/%.c=obj/client/%.o) 


run: clean default
	./$(TARGET_SRV) -f ./mynewdb.db -n -p 8080 &
	./$(TARGET_CLI) 127.0.0.1
	kill -9 $$(pidof dbserver)

clean:
	rm -f obj/server/*.o
	rm -f obj/client/*.o
	rm -f bin/*
	rm -f *.db

default: $(TARGET_SRV) $(TARGET_CLI)

$(TARGET_SRV): $(OBJ_CLIENT)
	gcc -o $@ $?

$(OBJ_SRV): obj/server/%.o: src/server/%.c
	gcc -c $< -o $@ -Iinclude

$(TARGET_CLIENT): $(OBJ_CLIENT)
	gcc -o $@ $?

$(OBJ_CLIENT): obj/client/%.o: src/client/%.c
	gcc -c $< -o $@ -Iinclude
