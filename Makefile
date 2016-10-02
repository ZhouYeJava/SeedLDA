all: 
	g++ -std=c++11 ./src/SeedLDA.cpp -o ./bin/SeedLDA
run:
	./bin/SeedLDA
clean:
	$(RM) ./bin/SeedLDA
