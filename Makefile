TARGET = bin/final
SRC = $(wildcard src/*c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC)) 

run: clean default
	./$(TARGET) -f mydb.db -n
	./$(TARGET) -f mydb.db -a "Seagin, Rawai Thailand 1222, 200" 
	./$(TARGET) -f mydb.db -a "Bank,  Kata Thailand 1222, 300" 
	./$(TARGET) -f mydb.db -l

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o: src/%.c
	gcc -c $< -o $@ -Iinclude
