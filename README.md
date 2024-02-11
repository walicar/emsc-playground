# WASM Playground
Boilerplate code to play with C compiled WASM modules

## Overview
- `web/` contains all of the files that will be used to render website
- `test/` contains code to run a local server, use `npm run dev` to start this server
- `emhtml` target is used for [Emscripten compiled pages](https://emscripten.org/docs/compiling/Deploying-Pages.html), use `npm run emhtml` to run this page.

## Notes
- If deploying a similar structure onto GitHub pages, you must provide a way to return the site's assets with [COOP and COEP headers](https://web.dev/articles/coop-coep).