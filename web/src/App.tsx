import { onMount, type Component, onCleanup, createSignal, createEffect } from 'solid-js';

import IntroScreen from './Intro';
import styles from './App.module.css';

const INPUT_FIELD = {
  VERTEX: 'vertices',
  WIRE: 'wireframe',
  FILL_WIRE: 'fill-wireframe',
  FILL: 'fill',
  TEXTURE: 'texture',
  ROTATE: 'rotate-models',
};

// these must be the same as those defined in wasm compiled code
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

const App: Component = () => {
  onMount(() => {
    // mimic the keyboard shortcuts implemented
    // within the compiled wasm code
    function handleKeypress(e: KeyboardEvent) {
      let el;
      switch (e.key) {
        case '1':
          el = document.getElementById(INPUT_FIELD.VERTEX) as HTMLInputElement;
          el.checked = true;
          break;
        case '2':
          el = document.getElementById(INPUT_FIELD.WIRE) as HTMLInputElement;
          el.checked = true;
          break;
        case '3':
          el = document.getElementById(INPUT_FIELD.FILL_WIRE) as HTMLInputElement;
          el.checked = true;
          break;
        case '4':
          el = document.getElementById(INPUT_FIELD.FILL) as HTMLInputElement;
          el.checked = true;
          break;
        case '5':
          if (!textureDisabled()) {
            el = document.getElementById(INPUT_FIELD.TEXTURE) as HTMLInputElement;
            el.checked = true;
          }
          break;
        case 'r':
          el = document.getElementById(INPUT_FIELD.ROTATE) as HTMLInputElement;
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
          // handle displaying all meshes separately
          if (e.target.value == MODEL.ALL) {
            // exported wasm function
            _load_all_meshes();
            setTextureDisabled(false);
            return;
          }
          // exported wasm function
          const ptr = stringToNewUTF8(e.target.value);
          _load_single_mesh(ptr);
          _free(ptr);

          const hasTexture = _mesh_has_texture(Object.keys(MODEL).indexOf(e.target.value));
          setTextureDisabled(!hasTexture);
          // change selected display option to "fill" in case no texture is available
          if (!hasTexture) {
            const fillEl = document.getElementById(INPUT_FIELD.FILL) as HTMLInputElement;
            fillEl.click();
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

const [textureDisabled, setTextureDisabled] = createSignal(true);

const DisplayOptions = () => {
  return (
    <fieldset
      id="display-options"
      onChange={(e) => {
        // exported wasm function
        _set_display_mode(parseInt(e.target.value));
      }}
    >
      <legend>Display mode:</legend>

      <div>
        <input type="radio" id={INPUT_FIELD.VERTEX} name="display" value="0" />
        <label for={INPUT_FIELD.VERTEX}>Vertices</label>
      </div>

      <div>
        <input type="radio" id={INPUT_FIELD.WIRE} name="display" value="1" checked />
        <label for={INPUT_FIELD.WIRE}>Wireframe</label>
      </div>

      <div>
        <input type="radio" id={INPUT_FIELD.FILL_WIRE} name="display" value="2" />
        <label for={INPUT_FIELD.FILL_WIRE}>Wireframe with fill</label>
      </div>

      <div>
        <input type="radio" id={INPUT_FIELD.FILL} name="display" value="3" />
        <label for={INPUT_FIELD.FILL}>Fill</label>
      </div>

      <div>
        <input
          type="radio"
          id={INPUT_FIELD.TEXTURE}
          name="display"
          value="4"
          disabled={textureDisabled()}
        />
        <label for={INPUT_FIELD.TEXTURE}>Texture</label>
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
          id={INPUT_FIELD.ROTATE}
          name={INPUT_FIELD.ROTATE}
          onChange={(e) => {
            // exported wasm function
            e.target.checked ? _set_rotation_mode(1) : _set_rotation_mode(0);
          }}
        />
        <label for={INPUT_FIELD.ROTATE}>Rotate model</label>
      </div>
    </fieldset>
  );
};

export default App;
