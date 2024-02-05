main: src/main.c
	emcc src/main.c -o web/assets/out.js -pthread -sPROXY_TO_PTHREAD -sPTHREAD_POOL_SIZE=1

emhtml: src/main.c
	emcc src/main.c -o test/index.html -pthread -sPROXY_TO_PTHREAD -sPTHREAD_POOL_SIZE=1

clean:
	rm -f web/assets/* test/index*