file = Main
test = test

codigo: $(file).cpp
	g++ $(file).cpp -o $(file)
	./$(file)

clean:
	rm -f $(file)
	rm -f $(test)
	rm -f $(file).exe
	rm -f $(test).exe
	rm -f Soluciones/*.out

test: $(test).cpp
	g++ $(test).cpp -o $(test)
	./$(test)