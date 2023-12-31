import type { Component } from 'solid-js';
import { onMount, onCleanup, createSignal, Show } from 'solid-js';

import IntroScreen from './Tutorial';
import device from './device.ts';
import styles from './App.module.css';
import cmSvgUrl from '../assets/control-menu-icon.svg';
import xSvgUrl from '../assets/x-icon.svg';
import ghSvgUrl from '../assets/github-icon.svg';

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

const MODEL_SELECT = 'model-select';
const ROTATE_MODEL = 'rotate-model';

const [displayMode, setDisplayMode] = createSignal(DISPLAY_MODE.WIRE); // hard-coded default within WASM
const [model, setModel] = createSignal(MODEL.SPHERE); // hard-coded default within WASM
const [showShortcuts, setShowShortcuts] = createSignal(false);
const [showControlMenu, setShowControlMenu] = createSignal(false);

const App: Component = () => {
  onMount(() => {
    // mirror the keyboard shortcuts implemented within wasm
    function handleKeydown(e: KeyboardEvent) {
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
        // UI-only shortcut
        case ' ':
          if (!device.isTouch) setShowShortcuts(true);
          break;
        default:
          break;
      }
    }

    function handleKeyup(e: KeyboardEvent) {
      if (e.key === ' ') {
        setShowShortcuts(false);
      }
    }

    document.addEventListener('keydown', handleKeydown);
    document.addEventListener('keyup', handleKeyup);

    onCleanup(() => {
      document.removeEventListener('keydown', handleKeydown);
      document.removeEventListener('keyup', handleKeyup);
    });
  });

  // blur model-select field upon click outside to avoid
  // conflicting "space" key press with shortcuts
  onMount(() => {
    const selectEl = document.getElementById(MODEL_SELECT) as HTMLSelectElement;
    function handleOutsideClick(e: MouseEvent) {
      const el = e.target as HTMLElement;
      if (!el.contains(selectEl)) {
        selectEl.blur();
      }
    }
    document.addEventListener('mousedown', handleOutsideClick);
    onCleanup(() => {
      document.removeEventListener('mousedown', handleOutsideClick);
    });
  });

  return (
    <>
      {/* github link icon is positioned above control menu to blur properly in mobile */}
      <a
        class={styles.githubIconLink}
        href="https://github.com/rmshin/3d-renderer-wasm"
        target="_blank"
      >
        <img src={ghSvgUrl} />
      </a>
      <Show when={!device.isLargeScreen}>
        {!showControlMenu() && (
          <img
            class={styles.controlMenuIcon}
            src={cmSvgUrl}
            onClick={() => setShowControlMenu(true)}
          />
        )}
      </Show>

      <div
        class={`${styles.controlMenu} ${
          !showControlMenu() && !device.isLargeScreen ? styles.hidden : ''
        }`}
      >
        <ModelSelect />
        <DisplayOptions />
        <Actions />
        <button class={styles.controlMenuClose} onClick={() => setShowControlMenu(false)}>
          <img src={xSvgUrl} />
          close
        </button>
      </div>

      <Show when={!device.isTouch}>
        {showShortcuts() ? (
          <p class={styles.shortcutsTip}>
            &#8592;&nbsp;(A) &nbsp; &#8593;&nbsp;(W) &nbsp; &#8595;&nbsp;(S) &nbsp; &#8594;&nbsp;(D)
          </p>
        ) : (
          <p class={styles.shortcutsTip}>
            Press <span>space</span> to view shortcuts
          </p>
        )}
      </Show>
      <IntroScreen />
    </>
  );
};

const ModelSelect = () => {
  return (
    <fieldset>
      <legend>Model:</legend>
      <select
        name="model"
        id={MODEL_SELECT}
        class={styles.modelSelect}
        onChange={(e) => {
          setModel(e.target.value);
          // exported wasm functions
          // handle displaying all meshes separately
          if (e.target.value == MODEL.ALL) {
            // @ts-ignore
            _load_all_meshes();
          } else {
            // @ts-ignore
            const ptr = stringToNewUTF8(e.target.value);
            // @ts-ignore
            _load_single_mesh(ptr);
            // @ts-ignore
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
    </fieldset>
  );
};

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

const DisplayOptions = () => {
  return (
    <fieldset
      id="display-options"
      onChange={(e) => {
        const target = e.target as HTMLInputElement;
        const idx = parseInt(target.value);
        setDisplayMode(Object.values(DISPLAY_MODE)[idx]);
        // exported wasm function
        // @ts-ignore
        _set_display_mode(idx);
      }}
      class={styles.displayOptions}
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
        {showShortcuts() && <span> (1)</span>}
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
        {showShortcuts() && <span> (2)</span>}
      </div>

      <div>
        <input
          type="radio"
          id={DISPLAY_MODE.FILL_WIRE}
          name="display"
          value="2"
          checked={isOptionChecked(DISPLAY_MODE.FILL_WIRE)}
        />
        <label for={DISPLAY_MODE.FILL_WIRE}>Wireframe w/ fill</label>
        {showShortcuts() && <span> (3)</span>}
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
        {showShortcuts() && <span> (4)</span>}
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
        <label class={isTextureDisabled() ? styles.disabled : ''} for={DISPLAY_MODE.TEXTURE}>
          Texture
        </label>
        {showShortcuts() && <span> (5)</span>}
      </div>
    </fieldset>
  );
};

const Actions = () => {
  return (
    <fieldset class={styles.actions}>
      <legend>Actions:</legend>
      <div>
        <input
          type="checkbox"
          id={ROTATE_MODEL}
          name={ROTATE_MODEL}
          onChange={(e) => {
            // exported wasm function
            // @ts-ignore
            e.target.checked ? _set_rotation_mode(1) : _set_rotation_mode(0);
          }}
        />
        <label for={ROTATE_MODEL}>Rotate model</label>
        {showShortcuts() && <span> (r)</span>}
      </div>

      <div>
        <button
          class={styles.resetCameraButton}
          onClick={() => {
            // @ts-ignore
            _reset_camera();
          }}
        >
          Reset camera
        </button>
        {showShortcuts() && <span> (q)</span>}
      </div>
    </fieldset>
  );
};

export default App;
