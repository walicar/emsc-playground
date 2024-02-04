main: src/hello.cxx
	em++ src/hello.cxx -o web/static/project.js

emhtml: src/hello.cxx
	em++ src/hello.cxx -o test/index.html

clean:
	rm -f web/static/* test/index*