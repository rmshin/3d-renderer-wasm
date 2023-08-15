Web app that wraps the WASM-compiled 3d renderer and extends it with UI controls. It's built using [SolidJS](https://solidjs.com) and [Vite](https://vitejs.dev/), and deployed to Github Pages via the Actions workflow defined in `gh-pages.yml`.

## Installation

To get the app running locally, clone the parent repo and run the following:

```bash
$ make wasm # compiles c renderer to wasm
$ cd web
$ npm install # or yarn install
$ npm run dev # or npm start
```

### `npm run dev` or `npm start`

Runs the app in the development mode.<br>
Open [http://localhost:3002](http://localhost:3002) to view it in the browser.

The page will reload if you make edits.<br>

### `npm run build`

Builds the app for production to the `dist` folder.<br>
It correctly bundles Solid in production mode and optimizes the build for the best performance.

The build is minified and the filenames include the hashes.
