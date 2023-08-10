// https://developer.mozilla.org/en-US/docs/Web/HTTP/Browser_detection_using_the_user_agent
function isTouch() {
  let hasTouchScreen = false;

  if ('maxTouchPoints' in navigator) {
    hasTouchScreen = navigator.maxTouchPoints > 0;
  } else if ('msMaxTouchPoints' in navigator) {
    hasTouchScreen = navigator.msMaxTouchPoints > 0;
  } else {
    const mQ = matchMedia('(pointer:coarse)');
    if (mQ && mQ.media === '(pointer:coarse)') {
      hasTouchScreen = !!mQ.matches;
    } else if ('orientation' in window || 'orientation' in screen) {
      hasTouchScreen = true; // deprecated, but good fallback
    } else {
      // Only as a last resort, fall back to user agent sniffing
      var UA = navigator.userAgent;
      hasTouchScreen =
        /\b(BlackBerry|webOS|iPhone|IEMobile)\b/i.test(UA) ||
        /\b(Android|Windows Phone|iPad|iPod)\b/i.test(UA);
    }
  }
  return hasTouchScreen;
}

// same breakpoint as bootstrap: https://getbootstrap.com/docs/5.0/layout/breakpoints/
function isLargeScreen() {
  const mq = window.matchMedia('(min-width: 992px)');
  return mq.matches;
}

export default { isTouch: isTouch(), isLargeScreen: isLargeScreen() };
