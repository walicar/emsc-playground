main: src/main.cxx
	em++ src/main.cxx -o web/assets/out.js -s EXPORTED_FUNCTIONS='["_int_sqrt"]' -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]'

emhtml: src/main.cxx
	em++ src/main.cxx -o test/index.html

clean:
	rm -f web/static/* test/index*