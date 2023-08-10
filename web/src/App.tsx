import type { Component } from 'solid-js';
import { onMount, onCleanup, createSignal } from 'solid-js';

import IntroScreen from './Tutorial';
import styles from './App.module.css';

// these values must be the same as those defined in wasm
const MODEL = {
  SPHERE: 'sphere',
  CUBE: 'cube',
  CRAB: 'crab',
  DRONE: 'drone',
  EFA: 'efa',
  F22: 'f22',
  F117: 'f117',
  ALL: 'all',
};

// these values must be the same as those defined in wasm
// entry order matters
const DISPLAY_MODE = {
  VERTEX: 'vertices', // 0
  WIRE: 'wireframe', // 1
  FILL_WIRE: 'fill-wireframe', // 2
  FILL: 'fill', // 3
  TEXTURE: 'texture', // 4
};

const ROTATE_MODEL = 'rotate-model';

const [displayMode, setDisplayMode] = createSignal(DISPLAY_MODE.WIRE); // hard-coded default within WASM
const [model, setModel] = createSignal(MODEL.SPHERE); // hard-coded default within WASM

function isTextureDisabled() {
  return model() === MODEL.SPHERE || model() === MODEL.CUBE;
}

function isOptionChecked(option: string) {
  switch (option) {
    case DISPLAY_MODE.VERTEX:
    case DISPLAY_MODE.WIRE:
    case DISPLAY_MODE.FILL_WIRE:
      return displayMode() === option;
    case DISPLAY_MODE.FILL:
      return (
        displayMode() == option || (displayMode() == DISPLAY_MODE.TEXTURE && isTextureDisabled())
      );
    case DISPLAY_MODE.TEXTURE:
      return displayMode() == option && !isTextureDisabled();
  }
}

const App: Component = () => {
  onMount(() => {
    // mirror the keyboard shortcuts implemented within wasm
    function handleKeypress(e: KeyboardEvent) {
      let el;
      switch (e.key) {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
          const idx = parseInt(e.key) - 1;
          const newDisplayMode = Object.values(DISPLAY_MODE)[idx];
          setDisplayMode(newDisplayMode);
          break;
        case 'r':
          el = document.getElementById(ROTATE_MODEL) as HTMLInputElement;
          el.checked = !el.checked;
          break;
        default:
          break;
      }
    }
    document.addEventListener('keydown', handleKeypress);

    onCleanup(() => {
      document.removeEventListener('keydown', handleKeypress);
    });
  });

  return (
    <>
      <div class={styles.controlMenu}>
        <ModelSelect />
        <DisplayOptions />
        <ToggleActions />
      </div>
      <IntroScreen />
    </>
  );
};

const ModelSelect = () => {
  return (
    <div>
      <label for="model-select">Model:</label>
      <select
        name="model"
        id="model-select"
        onChange={(e) => {
          setModel(e.target.value);
          // exported wasm functions
          // handle displaying all meshes separately
          if (e.target.value == MODEL.ALL) {
            _load_all_meshes();
          } else {
            const ptr = stringToNewUTF8(e.target.value);
            _load_single_mesh(ptr);
            _free(ptr);
          }
        }}
      >
        <option value={MODEL.SPHERE}>Sphere</option>
        <option value={MODEL.CUBE}>Cube</option>
        <option value={MODEL.CRAB}>Crab</option>
        <option value={MODEL.DRONE}>Drone</option>
        <option value={MODEL.EFA}>Eurofighter/EFA</option>
        <option value={MODEL.F22}>F-22 Raptor</option>
        <option value={MODEL.F117}>F-117 Nighthawk</option>
        <option value={MODEL.ALL}>All models</option>
      </select>
    </div>
  );
};

const DisplayOptions = () => {
  return (
    <fieldset
      id="display-options"
      onChange={(e) => {
        const idx = parseInt(e.target.value);
        setDisplayMode(Object.values(DISPLAY_MODE)[idx]);
        // exported wasm function
        _set_display_mode(idx);
      }}
    >
      <legend>Display mode:</legend>

      <div>
        <input
          type="radio"
          id={DISPLAY_MODE.VERTEX}
          name="display"
          value="0"
          checked={isOptionChecked(DISPLAY_MODE.VERTEX)}
        />
        <label for={DISPLAY_MODE.VERTEX}>Vertices</label>
      </div>

      <div>
        <input
          type="radio"
          id={DISPLAY_MODE.WIRE}
          name="display"
          value="1"
          checked={isOptionChecked(DISPLAY_MODE.WIRE)}
        />
        <label for={DISPLAY_MODE.WIRE}>Wireframe</label>
      </div>

      <div>
        <input
          type="radio"
          id={DISPLAY_MODE.FILL_WIRE}
          name="display"
          value="2"
          checked={isOptionChecked(DISPLAY_MODE.FILL_WIRE)}
        />
        <label for={DISPLAY_MODE.FILL_WIRE}>Wireframe with fill</label>
      </div>

      <div>
        <input
          type="radio"
          id={DISPLAY_MODE.FILL}
          name="display"
          value="3"
          checked={isOptionChecked(DISPLAY_MODE.FILL)}
        />
        <label for={DISPLAY_MODE.FILL}>Fill</label>
      </div>

      <div>
        <input
          type="radio"
          id={DISPLAY_MODE.TEXTURE}
          name="display"
          value="4"
          disabled={isTextureDisabled()}
          checked={isOptionChecked(DISPLAY_MODE.TEXTURE)}
        />
        <label for={DISPLAY_MODE.TEXTURE}>Texture</label>
      </div>
    </fieldset>
  );
};

const ToggleActions = () => {
  return (
    <fieldset>
      <legend>Actions:</legend>

      <div>
        <input
          type="checkbox"
          id={ROTATE_MODEL}
          name={ROTATE_MODEL}
          onChange={(e) => {
            // exported wasm function
            e.target.checked ? _set_rotation_mode(1) : _set_rotation_mode(0);
          }}
        />
        <label for={ROTATE_MODEL}>Rotate model</label>
      </div>
    </fieldset>
  );
};

export default App;
