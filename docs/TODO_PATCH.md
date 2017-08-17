# TODOs for patch versions
Patch versions (x.y.1, x.y.2) contain bugfixes without changing the API but they can apppear in minor (x.1.0, x.2.0) or major (1.0.0, 2.0.0) versions too.

Bugfixes are done in `bugfix` branche.

The bugfixes of the still not released version are in `beta` branche.

## Contributing
Please create an issue to introduce a bug instead of adding pull request to this file.


## Nest release
- [ ] lv_btnm: check hide code (\177) at 0. byte position too (if width is not specified)

## v4.2  (released at: 17.08.2017)
- [x] lv_slider: don't let indicator or bar to disappear because of hpad/vpad
- [x] lv_ta: memory leak if deleted in password mode
