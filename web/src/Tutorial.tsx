import { Show } from 'solid-js/web';

import { createSignal } from 'solid-js';
import styles from './Tutorial.module.css';

const Tutorial = () => {
  const [showTutorial, setShowTutorial] = createSignal(true);
  return (
    <Show when={showTutorial()}>
      <div class={styles.introPopup} onClick={() => setShowTutorial(false)}>
        <section class={styles.introContent} onClick={(e) => e.stopPropagation()}>
          <h1>hello sir welcome to my house</h1>
        </section>
      </div>
    </Show>
  );
};

export default Tutorial;
