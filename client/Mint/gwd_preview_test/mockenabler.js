/**
 * @fileoverview A mock for the Enabler.js file. This is used to provide a
 * mock environment when user previews a GWD DoubleClick creative while offline.
 * @author ksubramanian@google.com (Kishore Subramanian)
 */

(function() {

  /**
   * Logs a message in the window console if available.
   * @param {string} msg The message to print in the console.
   */
  var log = function(msg) {
    if (window.console) {
      window.console.log(msg);
    }
  };


  /**
   * The mock of the DoubleClick studio namespace.
   */
  window.studio = {
    common: {
      Environment: {
        hasType: function(type) {
        },
        Type: {
          /** Running in a live environment (non LOCAL). */
          'LIVE': 1 << 0,
          /** Local environment. */
          'LOCAL': 1 << 1,
          /** Running in a browser (vs a WebView, for in app ads). */
          'BROWSER': 1 << 2,
          /** Running in inside of an app. */
          'IN_APP': 1 << 3,
          /** Running in layouts preview. */
          'LAYOUTS_PREVIEW': 1 << 4,
          /** Running in GDN CreativeToolset context. */
          'CREATIVE_TOOLSET': 1 << 5,
          /** Running in Studio rendering environment. */
          'RENDERING_STUDIO': 1 << 6,
          /** Running in test rendering environment. */
          'RENDERING_TEST': 1 << 7
        }
      }
    },
    events: {
      StudioEvent: {
        INIT: 'init',
        VISIBLE: 'visible',
        HIDDEN: 'hidden',
        PAGE_LOADED: 'pageloaded',
        EXIT: 'exit',
        EXPAND_START: 'expandStart',
        COLLAPSE_START: 'collapseStart'
      }
    },
    module: {
      ModuleId: {
        VIDEO: 'video'
      }
    },
    sdk: {
      ContainerState: {
        /**
         * The state when the ad is collapsing.
         */
        'COLLAPSING': 'collapsing',
        /**
         * The state when the ad is collapsed.
         */
        'COLLAPSED': 'collapsed',
        /**
         * The state when the ad is expanding.
         */
        'EXPANDING': 'expanding',
        /**
         * The state when the ad is expanded.
         */
        'EXPANDED': 'expanded',
        /**
         * The state when the ad is collapsing from fullscreen.
         */
        'FS_COLLAPSING': 'fs_collapsing',
        /**
         * The state when the ad is expanding to fullscreen.
         */
        'FS_EXPANDING': 'fs_expanding',
        /**
         * The state when the ad has expanded to fullscreen.
         */
        'FS_EXPANDED': 'fs_expanded'
      };
      }
    },
    video: {
      Reporter: {
        attach: function(id, videoElement, isAutoplay) {
        },
        detach: function(id) {
        }
      }
    }
  };


  /**
   * Mock of the DoubleClick Enabler.
   */
  var Enabler = {
    events: {}
  };


  /**
   * Adds an event listener to the event target. The same handler can only be
   * added once per the type. Even if you add the same handler multiple times
   * using the same type then it will only be called once when the event is
   * dispatched.
   *
   * @param {string} type The type of the event to listen for.
   * @param {Function|Object} handler The function to handle the event. The
   *     handler can also be an object that implements the handleEvent method
   *     which takes the event object as an argument.
   * @param {boolean=} opt_capture In DOM-compliant browsers, this determines
   *     whether the listener is fired during the capture or bubble phase
   *     of the event.
   * @param {Object=} opt_handlerScope Object in whose scope to call
   *     the listener.
   */
  Enabler.addEventListener = function(
      type, handler, opt_capture, opt_handlerScope) {
    this.events[type] = handler;
  };


  /**
   * Removes an event listener from the event target. The handler must be the
   * same object as the one added. If the handler has not been added then
   * nothing is done.
   * @param {string} type The type of the event to listen for.
   * @param {Function|Object} handler The function to handle the event. The
   *     handler can also be an object that implements the handleEvent method
   *     which takes the event object as an argument.
   * @param {boolean=} opt_capture In DOM-compliant browsers, this determines
   *     whether the listener is fired during the capture or bubble phase
   *     of the event.
   * @param {Object=} opt_handlerScope Object in whose scope to call
   *     the listener.
   */
  Enabler.removeEventListener = function(
      type, handler, opt_capture, opt_handlerScope) {
    delete this.events[type];
  };


  /**
   * Returns whether the Enabler is intialized.
   * @return {boolean} Whether the Enabler is intialized.
   */
  Enabler.isInitialized = function() {
    log('No network connection. Preview will proceed but some features that' +
        ' require a network connection may not work as expected.');
    log('Enabler initialized');
    return true;
  };


  /**
   * Returns whether the ad is visible. The iframe is notified when the ad is
   * visible and the enabler dispatches the <code>StudioEvent.VISIBLE</code>
   * event.
   * @return {boolean} Whether the ad is visible or not.
   */
  Enabler.isVisible = function() {
    log('The creative is visible now');
    return true;
  };


  /**
   * @return {boolean} Whether the page containing the ad has loaded.
   */
  Enabler.isPageLoaded = function() {
    log('Page loaded');
    return true;
  };


  /**
   * Tracks a counter event.
   * @export
   * @param {string} eventId The string ID of the event to count.
   * @param {boolean=} opt_isCumulative Optional parameter that indicates
   *     whether or not the counter event should be counted cumulatively.
   */
  Enabler.counter = function(eventId, opt_isCumulative) {
    log('Counter ' + eventId + ' invoked.');
  };


  /**
   * Opens a new window with the URL as identified by the given exit ID.
   * @param {string} id The exit ID.
   * @param {string=} opt_url The url to navigate to.
   * @export
   * @override
   */
  Enabler.exit = function(exitId, opt_url) {
    log('Exit ' + exitId + ' invoked');
  };


  /**
   * Opens a new window with the URL as identified by the given exit ID.
   * This differs from normal exit() as the url value will always override
   * the value modified in Studio or elsewhere.
   * @param {string} id The exit ID.
   * @param {string} url The url to navigate to regardless of what has been set
   *     in Studio.
   */
  Enabler.exitOverride = function(exitId, url) {
    log('Exit override for ' + exitId + ' invoked');
  };


  /**
   * Starts an event timer.
   * @export
   * @param {string} timerId The string ID of the timer to start.
   */
  Enabler.startTimer = function(timerId) {
    log('Timer ' + timerId + ' started.');
  };


  /**
   * Stops an event timer.
   * @export
   * @param {string} timerId The string ID of the timer to stop.
   */
  Enabler.stopTimer = function(timerId) {
    log('Timer ' + timerId + ' stopped.');
  };


  /**
   * Records a manual closing of a floating, pop-up, expanding, in-page with
   * pop-up, or in-page with floating ad.
   */
  Enabler.reportManualClose = function() {
    log('Ad was closed by user action.');
  };


  /**
   * Initiates the expand lifecycle. This begins by calling requestExpand(),
   * waiting for the EXPAND_START event, animating your expand, then calling
   * finishExpand() when the creative has fully expanded.
   * Please use event listeners to invoke the expanded state of the ad as the
   * expand may be dispatched by the environment independently of calling
   * {@link studio.Enabler#requestExpand()}. This is especially true on networks
   * such as the Google Display Network that display a hover timer before
   * allowing the ad to expand. Typical usage will look like:
   * <pre><code>Enabler.addEventListener(
   *     studio.events.StudioEvent.EXPAND_START,
   *     function(event) {
   *       // For multi directional expands, direction to expand in can be
   *       // obtained by calling Enabler.getExpandingDirection() (or
   *       // from event.direction).
   *
   *       // Do expand action then...
   *       Enabler.finishExpand();
   *     });
   * Enabler.requestExpand();
   * </code></pre>
   *
   * This function is only applicable for expandable ad formats.
   */
  Enabler.requestExpand = function() {
    log('The creative will expand now.');
    if (this.events.expandStart) {
      this.events.expandStart();
    }
  };


  /**
   * Initiates the collapse lifecycle. This begins by calling requestCollapse(),
   * waiting for the COLLAPSE_START event, animating your collapse, then calling
   * finishCollapse() when the creative has fully collapsed.
   * Please use event listeners to invoke the collapsed state of the ad as the
   * collapse event may be dispatched by the environment independently of
   * calling {@link studio.Enabler#requestCollapse()}.
   * <pre><code>Enabler.addEventListener(
   *     studio.events.StudioEvent.COLLAPSE_START,
   *     function() {
   *       // Do collapse action then...
   *       Enabler.finishCollapse();
   *     });
   * Enabler.requestCollapse();
   * </code></pre>
   *
   * This function is only applicable for expandable ad formats.
   */
  Enabler.requestCollapse = function() {
    log('The creative will collapse now.');
    if (this.events.collapseStart) {
      this.events.collapseStart();
    }
  };


  /**
   * Finalizes the expand call via the rendering code.
   */
  Enabler.finishExpand = function() {
    log('The creative has expanded.');
  };


  /**
   * Clips the container of the html asset from the expanded dimensions
   * to the collapsed dimensions.
   * Please use event listeners to invoke the collapsed state of the ad as the
   * calling collapse event may be dispatched by the environment independently
   * of {@link studio.Enabler#requestCollapse()}.
   * <pre><code>Enabler.addEventListener(
   *     studio.events.StudioEvent.COLLAPSE_START,
   *     function() {
   *       // collapse action
   *       Enabler.finishCollapse();
   *     });
   * Enabler.requestCollapse();
   * </code></pre>
   *
   * This function is only applicable for expandable ad formats.
   */
  Enabler.finishCollapse = function() {
    log('The creative has collapsed.');
  };


  /**
   * Loads an additional module.
   * @param {string} moduleName The name of the module to load.
   * @param {Function=} opt_loadedCallback The callback to invoke when the
   *     module is loaded.
   */
  Enabler.loadModule = function(moduleName, opt_loadedCallback) {};


  /**
   * Gets the runtime URL given the original compile-time filename.
   * @param {string} filepath The original full path of the asset
   * @export
   * @override
   * @return {string} The URL to be used at runtime when served through Studio
   *     and DART.
   */
  Enabler.getUrl = function(url) {
    return url;
  };


  /**
   * Turns on a quick simulation mode.
   */
  Enabler.setRushSimulatedLocalEvents = function() {};


  /**
   * Returns the current state of the container.
   * @return {studio.sdk.ContainerState} The container state.
   */
  Enabler.getContainerState = function() {};

  window.Enabler = Enabler;
  window.studio.Enabler = Enabler;

})();
