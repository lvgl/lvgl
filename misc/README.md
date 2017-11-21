# Miscellaneous Library

This library is written especially for the Littlev Graphics Library however its useful hardware independent software modules can be used in your non graphical projects too. Some of the most important components:
* Dynamic memory manager with defrag. and memory monitoring 
* Simple task scheduler with priority and idle measurement
* File system interface to access different mediums easily  
* RAM FS
* FIFO
* Linked list
* Fonts
* etc

## Usage
1. Clone the repository into the root folder of your project: `git clone https://github.com/littlevgl/misc.git`
2. In your IDE add the project **root folder as include path**
3. Copy *misc/misc_conf_templ.h* as **misc_conf.h** to the project root folder
4. Delete the first `#if 0` and the last `#endif` to enable the file
5. Enable/disable or configure the components
6. To initialize the library `#include misc/misc.h` and call `misc_init()`

## Documentation
 * You will find **well commented** source and header files.
 * There are same documentation on the graphics library's webpage: http://gl.littlev.hu
 
## Contributing
1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request!

If you find an issue, please report it via GitHub!

## Donate
If you are pleased with the Misc. Library and found it useful please support its further development:

[![Donate](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=LWHHAQYZMRQJS)
