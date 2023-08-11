import { Show } from 'solid-js/web';

import { createSignal } from 'solid-js';
import device from './device.ts';
import styles from './Tutorial.module.css';
import xSvgUrl from '../assets/x-icon.svg';

const Tutorial = () => {
  const [showTutorial, setShowTutorial] = createSignal(true);
  return (
    <Show when={showTutorial()}>
      <div class={styles.introPopup} onClick={() => setShowTutorial(false)}>
        <section class={styles.introContent} onClick={(e) => e.stopPropagation()}>
          <h1>3D Renderer Demo</h1>
          {device.isTouch ? (
            <>
              <p>This is a web demo of a simple software-based 3d renderer written in C.</p>
              <p>Use two fingers to zoom in & out, and one finger to rotate view.</p>
              <p>
                You can toggle through different 3d models & rendering modes using the control menu.
              </p>
            </>
          ) : (
            <p>
              This is a web demo of a simple software-based 3d renderer written in C. Navigate the
              screen with your keyboard and mouse, and use the control menu at the top right corner
              (or the below shortcuts) to toggle through different 3d models & rendering modes.
            </p>
          )}
          {!device.isTouch && (
            <>
              <h4>Camera Controls</h4>
              <ul class={styles.controlsList}>
                <li>W - move &#8593;</li>
                <li>S - move &#8595;</li>
                <li>A - move &#8592;</li>
                <li>D - move &#8594;</li>
                <li>Mouse drag - rotate view angle</li>
              </ul>
              <h4>Menu Shortcuts</h4>
              <ul class={styles.controlsList}>
                <li>1 - display only vertices (points)</li>
                <li>2 - display only wireframe (edges)</li>
                <li>3 - display wireframe & solid fill</li>
                <li>4 - display solid fill</li>
                <li>5 - display texture (skin)</li>
                <li>r - enable/disable model rotation</li>
              </ul>
            </>
          )}

          <img class={styles.closeIcon} src={xSvgUrl} onClick={() => setShowTutorial(false)} />
        </section>
      </div>
    </Show>
  );
};

export default Tutorial;
