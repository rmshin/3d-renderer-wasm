import { Show } from 'solid-js/web';

import styles from './Intro.module.css';
import { createSignal } from 'solid-js';

const IntroScreen = () => {
  const [showIntro, setShowIntro] = createSignal(true);
  return (
    <Show when={showIntro()}>
      <div class={styles.introPopup} onClick={() => setShowIntro(false)}>
        <section class={styles.introContent} onClick={(e) => e.stopPropagation()}>
          <h1>hello sir welcome to my house</h1>
        </section>
      </div>
    </Show>
  );
};

export default IntroScreen;
