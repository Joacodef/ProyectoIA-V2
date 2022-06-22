file = Main
test = test1

codigo: $(file).cpp
	g++ $(file).cpp -o $(file) -Wall
	./$(file)

clean:
	rm -f $(file)
	rm -f $(test)

test: $(test).cpp
	g++ $(test).cpp -o $(test) -Wall
	./$(test)