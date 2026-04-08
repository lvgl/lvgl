=======
Browser
=======

Overview
********

There are multiple ways to view LVGL UIs in a browser or even develop UIs directly in a browser environment:

- Compile UI to HTML using **Emscripten**
- Develop in online VSCode using **GitHub Codespaces**
- Preview UIs in the **LVGL UI Editor’s online preview**

Compile UIs to HTML (Emscripten)
********************************

`Emscripten <https://emscripten.org/>`__ can compile C source files into a single HTML output that runs in a browser. This is a convenient way to share developed UIs with stakeholders, as they can test the interface without needing a developer setup.

Instead of producing a binary, Emscripten produces WebAssembly, which is supported across modern browsers.

The `lv_web_emscripten <https://github.com/lvgl/lv_web_emscripten>`__ project provides a ready-to-use setup for compiling any LVGL UI into a browser-capable HTML file.

It uses a specialized, :ref:`build_cmake`‑based build system—check the README for full setup details.

Develop in Online VSCode (GitHub Codespaces)
********************************************

GitHub Codespaces offer a cloud-hosted VSCode environment that runs entirely in the browser.

LVGL provides a preconfigured Codespace environment, which you can launch with just a few clicks. It’s especially beneficial if:

- You're having issues setting up a local development environment
- Your machine is slow or underpowered
- You're working from a device without a local IDE

To get started:

1. Navigate to `https://github.com/lvgl/lvgl <https://github.com/lvgl/lvgl>`__.
2. Click the green **Code** button.
3. Select the **Codespaces** tab.
4. Click **Create Codespace on master**.
5. Wait briefly for the workspace to launch.
6. Open the terminal and run `./build_all.sh`.

This will build LVGL and render the UI in a VSCode tab. You can continue writing and rebuilding simply by running the script again.

Note: The Codespaces setup also uses Emscripten for browser rendering.

LVGL’s UI Editor Online Share
*****************************

Coming soon.
