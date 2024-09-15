# github fork 别人代码，然后基于某个trunk建分支
### lvgl 项目为例
1. fork 别人的仓库到自己的github
2. git clone https://github.com/simazhuge/lvgl.git
3. git remote add upstream https://github.com/lvgl/lvgl.git
4.  git remote -v
5.  git fetch upstream
6、git switch -c   upstream/release/v9.2
7、git push origin  upstream/release/v9.2
