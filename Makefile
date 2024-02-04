main: src/main.cxx
	em++ src/main.cxx -o web/assets/out.js -L/opt/homebrew/Cellar/emscripten/3.1.51/libexec/system/lib -lembind

emhtml: src/main.cxx
	em++ src/main.cxx -o test/index.html

clean:
	rm -f web/static/* test/index*