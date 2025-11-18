/*---------------------------------------------------------------------
 * Called by 'DOMContentLoaded' event listener below; adds
 * documentation-version dropdown list just above search form in
 * upper part of left (nav) panel.
 *---------------------------------------------------------------------*/

function add_version_selector() {
  return fetch("https://raw.githubusercontent.com/lvgl/docs_compiled/gh-pages/versionlist.txt")
    .then((res) => res.text())
    .then((text) => {
      const versionList = text.split("\n").filter((version) => version.trim().length > 0);
      const versionDropdown = document.getElementById("version_dropdown");
      versionDropdown.innerHTML = versionList
        .map((version) => {
          let versionName = "";
          if (version == "master") versionName = "Latest";
          else versionName = "v" + (version.indexOf(".") != -1 ? version : version + " (latest minor)");
          return `<option value="${version}">${versionName}</option>`;
        })
        .join("");

      // Wait for the next animation frame to ensure DOM is updated
      requestAnimationFrame(() => {
        const versionDropdownValue = window.location.pathname.split("/")[1];
        const isValidVersion = versionDropdownValue.match(/^([0-9]+\.[0-9]+|master)$/);
        versionDropdown.value = isValidVersion ? versionDropdownValue : "master";
      });

      versionDropdown.addEventListener("change", ver_sel);
    });
}

/*---------------------------------------------------------------------
 * Fires when user selects a documentation version from version dropdown.
 *---------------------------------------------------------------------*/
function ver_sel() {
  var x = document.getElementById("version_dropdown").value;
  var new_url = window.location.protocol + "//" + window.location.host + "/" + x + "/";

  if (new_url.startsWith("http")) {
    window.location.href = new_url;
  }
}

/*---------------------------------------------------------------------
 * Once:  add documentation-version dropdown list just above search form in
 * upper part of left panel.
 *---------------------------------------------------------------------*/
document.addEventListener("DOMContentLoaded", (event) => {
  add_version_selector().then(() => {
    var value = window.location.pathname.split("/")[1];
    document.getElementById("version_dropdown").value = value;
  });
});

/*---------------------------------------------------------------------
 * This listener delays loading (slow-to-load) examples until they are
 * actually in view in the browser's viewport.
 *
 * Once:  for each "lv-example" class element, adds observer which watches
 * for that element to come into view in the browser's viewport.  When it
 * does, `onIntersection()` function is fired, causing example to be loaded
 * if it hasn't already, or removed when its intersectionRatio <= 0.
 */
document.addEventListener("DOMContentLoaded", (event) => {
  function onIntersection(entries) {
    entries.forEach((entry) => {
      let currentlyLoaded = entry.target.getAttribute("data-is-loaded") == "true";
      let shouldBeLoaded = entry.intersectionRatio > 0;
      if (currentlyLoaded != shouldBeLoaded) {
        entry.target.setAttribute("data-is-loaded", shouldBeLoaded);
        if (shouldBeLoaded) {
          let iframe = document.createElement("iframe");
          iframe.src = entry.target.getAttribute("data-real-src");
          entry.target.appendChild(iframe);
        } else {
          let iframe = entry.target.querySelector("iframe");
          iframe.parentNode.removeChild(iframe);
        }
      }
    });
  }
  const config = {
    rootMargin: "600px 0px",
    threshold: 0.01,
  };
  let observer = new IntersectionObserver(onIntersection, config);
  document.querySelectorAll(".lv-example").forEach((iframe) => {
    observer.observe(iframe);
  });
});

document.addEventListener("DOMContentLoaded", (event) => {
  /*---------------------------------------------------------------------
   * For API pages:
   *
   * Documented code elements on API pages each have a structure that looks
   * like this example for a function:
   *
   * <dl class="cpp function _state_>                             // container
   *     <dt id="_function_identifier_">function signature</dt>   // code highlighting done with <span> child elements
   *     <dd>documentation</dd>                                   // documentation, initially closed
   *     </dl>
   *
   * where `_state_` is either "expanded" or "unexpanded".  When "unexpanded"
   * class is present, the <dd> element containing the documentation is hidden
   * by CSS in `custom.css`.  Without that class present (i.e. when "expanded"
   * class is present instead), the <dd> element reverts to its normal state
   * of being visible.
   *
   * The job of the code below is to:
   * 1.  Add "unexpanded" class to all <dl class="cpp ..."> elements...
   * 2.  ...except if the URL had a hash string ("...#identifier_string")
   *     that matches the immediate <dt> child element's id attribute, in which
   *     case the "expanded" class is added instead.  (This usually indicates
   *     the user navigated there by clicking a hyperlinked code element in
   *     one of the documentation pages.)
   * 3.  Add an open/close button <span class="lv-api-expansion-button" />
   *     element just before each code element to toggle its expanded/unexpanded
   *     class.  (The click-able arrow icon before it is set in `custom.css`
   *     based on the <dl> element's "expanded" or "unexpanded" class.)
   *---------------------------------------------------------------------*/
  document.querySelectorAll("dl.cpp").forEach((cppListing) => {
    const dt = cppListing.querySelector("dt");
    let shouldBeExpanded = false;
    if (dt.id == document.location.hash.substring(1)) shouldBeExpanded = true;
    cppListing.classList.add(shouldBeExpanded ? "expanded" : "unexpanded");
    const button = document.createElement("span");
    button.classList.add("lv-api-expansion-button");
    button.addEventListener("click", () => {
      cppListing.classList.toggle("unexpanded");
      cppListing.classList.toggle("expanded");
    });

    dt.insertBefore(button, dt.firstChild);
  });

  /*---------------------------------------------------------------------
     * Display any current custom banner in `banner.json` at top of each page.
     *---------------------------------------------------------------------
     * Custom banners are inserted between these two elements at top of page.
    <a class="skip-to-content muted-link" href="#furo-main-content">Skip to content</a>

    <div class="lv-custom-banner-list">
      <p class="lv-custom-banner">
         <em>Important</em> announcement one!
      </p>
      <p class="lv-custom-banner">
         <em>Important</em> announcement two!
      </p>
      <p class="lv-custom-banner">
         <em>Important</em> announcement three!
      </p>
    </div>

    <div class="page">
      ...page content...
     *---------------------------------------------------------------------*/
  let bannerJsonUrl = "https://lvgl.io/data/banner.json";
  let bannerContainerClass = "lv-custom-banner-list";
  let bannerClass = "lv-custom-banner";
  /* Note:  banner priority property can have only one of these values:
   * ("highest" | "high" | "normal" | "low" | "lowest").
   * If not present, the default is "normal-priority".  This controls banner styling. */
  let priorityPropVals = ["highest", "high", "normal", "low", "lowest"];
  let defaultPrioPropVal = priorityPropVals[2];
  let priorityClassSuffix = "-priority";

  /* Sorting json banners in priority order.
   * `a` and `b` are BANNER objects from incoming `banner.json`. */
  function prio_compare(a, b) {
    let aPrioPropStr = a.hasOwnProperty("priority") ? a.priority : defaultPrioPropVal;
    let bPrioPropStr = b.hasOwnProperty("priority") ? b.priority : defaultPrioPropVal;
    let aPrio = 0;
    let bPrio = 0;

    /* Establish numeric values for `a` and `b`. */
    for (var i = 0; i < 5; i++) {
      if (aPrioPropStr === priorityPropVals[i]) {
        aPrio = i;
        break;
      }
    }

    for (var i = 0; i < 5; i++) {
      if (bPrioPropStr === priorityPropVals[i]) {
        bPrio = i;
        break;
      }
    }

    /* Correctness Proof
     * -----------------
     * < 0 = `a` should come before `b`.
     * > 0 = `a` should come after `b`.
     *   0 or NaN = a === b.
     *
     * Example:  a === "highest-priority"; b === "normal-priority".
     *           aPrio === 0             ; bPrio === 2.
     *           aPrio - bPrio === -2 means (`a` should come before `b`).
     */
    return aPrio - bPrio;
  }

  fetch(bannerJsonUrl)
    .then((response) => {
      if (response.ok) {
        return response.json();
      } else {
        /* Note:  per OOSC2, it is not appropriate to throw an exception for a
         * situation that is being checked for.  Sometimes the banner file will
         * not be there, in which case, we simply return an empty array object. */
        return [];
      }
    })
    /* JSON file was fetched successfully.... */
    .then((json) => {
      if (json.constructor !== Array) {
        /* Data structure not recognized. */
      } else {
        /* console.log('JSON is an array.'); */
        /* Does it contain any banners? */
        if (json.length === 0) {
          console.log("JSON has no banners -- nothing to do.");
        } else {
          /* Note:  `div.page` is unique to Furo theme. */

          /* Create and insert banner container. */
          const newDiv = document.createElement("div");
          newDiv.classList.add(bannerContainerClass);
          let bannerCount = 0;

          /* Create a <p> or an <a> element for each banner.
           * First, sort them in priority order with "highest-priority" being
           * at the top.  The JSON is an ARRAY of BANNER objects.
           * `prio_compare()` knows how to compare them.
           *
           * If the BANNER object has a "url" property, then
           * encapsulate banner in an anchor element that will send
           * user to designated URL.
           */
          json.sort(prio_compare);

          for (var i = 0; i < json.length; i++) {
            let banner = json[i];

            if (banner.hasOwnProperty("label")) {
              bannerCount++;
              let priorityClass = "";
              let newElement = null;

              if (banner.hasOwnProperty("url")) {
                newElement = document.createElement("a");
                newElement.setAttribute("href", banner.url);
              } else {
                newElement = document.createElement("p");
              }

              if (banner.hasOwnProperty("priority")) {
                priorityClass = banner.priority + priorityClassSuffix;
              } else {
                priorityClass = defaultPrioPropVal + priorityClassSuffix;
              }

              newElement.innerHTML = banner.label;
              newElement.classList.add(bannerClass);
              newElement.classList.add(priorityClass);
              newDiv.appendChild(newElement);
            }
          }

          if (bannerCount > 0) {
            const page = document.querySelector("div.page");
            const pgParent = page.parentElement;
            pgParent.insertBefore(newDiv, page);

            /* Finally, we need to tell the page element that its `min-hight`
             * is 100% minus the hight of all the banners, including the one
             * supplied by `conf.py` in `conf.html_theme_options.announcement`
             * if one is present === var(--header-height).
             *
             * This extends short pages by just enough to place [PREV] and [NEXT]
             * buttoms and footer at bottom of page without scrolling.
             *
             * Note:  this overrides the `min-height` property set for this
             * element in `furo.css`, which is:  calc(100% - var(--header-height)).
             * It additionally subtracts height of banner list.
             * */
            let height = newDiv.offsetHeight;
            page.style["min-height"] = `calc(100% - var(--header-height) - ${height}px)`;
          }
        }
      }
    })
    .catch((error) => {
      console.error("Fetch error: " + error.message);
    });
});

/*---------------------------------------------------------------------
 * This listener adds a click handler to all .toggle elements.
 * When a .toggle is clicked, all children are hidden except .header.
 * When .header is clicked, all children are toggled.
 *---------------------------------------------------------------------*/
document.addEventListener("DOMContentLoaded", function () {
  document.querySelectorAll(".toggle").forEach(function (toggle) {
    Array.from(toggle.children).forEach(function (child) {
      if (!child.classList.contains("header")) {
        child.style.display = "none";
      }
    });
    var header = toggle.querySelector(".header");
    if (header) {
      header.addEventListener("click", function (e) {
        const toggle = e.target.closest(".toggle");
        Array.from(toggle.children).forEach(function (child) {
          if (!child.classList.contains("header")) {
            if (child.style.display === "none") {
              child.style.display = "";
            } else {
              child.style.display = "none";
            }
          }
        });
        header.classList.toggle("open");
      });
    }
  });
});
