build:
	g++ client.cpp functions.cpp helpers.cpp buffer.cpp requests.cpp -o client
run:
	./client
clean:
	rm client