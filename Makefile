TARGET_CLI = bin/cli
TARGET_SRV = bin/server

SRC_SRV = $(wildcard src/server/*c)
OBJ_SRV = $(SRC_SRV:src/server/%.c=obj/server/%.o) 

SRC_CLI = $(wildcard src/cli/*c)
OBJ_CLI = $(SRC_CLI:src/cli/%.c=obj/cli/%.o) 


run: clean default
	./$(TARGET_SRV) -f ./mynewdb.db -n -p 8080 &
	./$(TARGET_CLI) 127.0.0.1
	kill -9 $$(pidof dbserver)


default: $(TARGET_SRV) $(TARGET_CLI)

clean:
	rm -f obj/server/*.o
	rm -f obj/cli/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET_SRV): $(OBJ_CLI)
	gcc -o $@ $?

$(OBJ_SRV): obj/server/%.o: src/serer/%.c
	gcc -c $< -o $@ -Iinclude

$(TARGET_CLI): $(OBJ_CLI)
	gcc -o $@ $?

$(OBJ_CLI): obj/cli/%.o: src/cli/%.c
	gcc -c $< -o $@ -Iinclude
