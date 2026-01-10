📋 LVGL 新 Widgets 开发 PR 提交完整指南

🎯 提交前检查清单

1️⃣ 代码格式化检查 (必须)

# 运行代码格式化脚本
python scripts/code-format.py

# 确保使用 Astyle v3.4.12
# 配置文件: scripts/code-format.cfg

2️⃣ 配置文件检查 (如果涉及配置选项)

# 如果在 lv_conf_template.h 中添加了新选项
python scripts/lv_conf_internal_gen.py

# 更新 Kconfig 配置
# 检查 src/lv_conf_internal.h 是否自动生成

3️⃣ 代码规范检查

- ✅ 命名规范: lv_<widget_name>_<action>
- ✅ 类型定义: 使用 _t 后缀
- ✅ 私有函数/结构体: 使用 _ 前缀
- ✅ API函数: 第一个参数为 lv_obj_t * parent
- ✅ Doxygen注释: 每个公共API必须有完整注释
- ✅ 行长度: 不超过120字符

4️⃣ 测试用例添加

# 新 widgets 必须包含测试
# 测试文件位置: tests/src/test_cases/

# 运行测试验证
./tests/main.py test

# 如果涉及截图测试，更新参考图像
./tests/main.py --update-image test

5️⃣ 文档和示例

- ✅ API文档: 在 docs/src/ 下添加相应文档
- ✅ 代码示例: 在 examples/ 目录下添加使用示例
- ✅ Doxygen注释: 所有公共API必须有完整文档

6️⃣ Pre-commit Hooks 检查

# 安装 pre-commit (如果未安装)
pip install pre-commit
pre-commit install

# 运行所有 hooks
pre-commit run --all-files

📝 PR 提交流程

步骤 1: 创建功能分支

git checkout -b feat/add-your-widget-name
# 或
git checkout -b fix/your-widget-bug-fix

步骤 2: 开发和测试

- 按照代码规范开发新 widgets
- 添加完整的测试用例
- 编写文档和示例
- 运行本地测试确保通过

步骤 3: 提交前最终检查

# 1. 格式化代码
python scripts/code-format.py

# 2. 运行所有测试
./tests/main.py test

# 3. 检查配置文件 (如需要)
python scripts/lv_conf_internal_gen.py

# 4. 检查 pre-commit hooks
pre-commit run --all-files

# 5. 查看变更
git status
git diff

步骤 4: 提交代码

提交信息格式 (Angular风格):
feat(widgets): add new span widget

添加 span widget，支持混合不同字体大小、颜色和样式的文本。
类似 HTML 的 <span> 标签功能。

- 支持多字体混合显示
- 支持局部样式设置
- 完整的 API 文档和示例

@example
lv_span_t * span = lv_span_create(parent);
lv_span_set_text(span, "Hello World");

Fixes: #1234

提交类型:
- feat(widgets) - 新功能/新 widgets
- fix(widgets) - widgets 相关 bug 修复
- docs(widgets) - 文档更新
- test(widgets) - 测试相关

步骤 5: 推送并创建 PR

git push origin feat/add-your-widget-name

PR 描述模板:
## Summary
添加 [widget名称] widget，用于 [功能描述]

## Changes
- 新增 `lv_[widget]_create()` 函数
- 新增 `lv_[widget]_set_*()` API
- 添加测试用例 `tests/src/test_cases/test_[widget].c`
- 添加示例 `examples/widgets/[widget]/`
- 更新文档 `docs/src/widgets/[widget].rst`

## Test Plan
- [x] 本地测试通过
- [x] 代码格式化检查通过
- [x] 文档和示例已添加
- [ ] CI 检查通过

## Related Issues
Fixes #xxxx

🤖 Generated with [Claude Code](https://claude.com/claude-code)

步骤 6: PR 标记和评审

- 初始创建时标记为 Draft
- 完成所有检查后标记为 Ready for review
- 根据评审意见修改
- 修改后重新请求评审

🔍 CI 自动化检查 (必须通过)

1. ✅ 代码格式验证 - check_style.yml
2. ✅ 配置文件验证 - check_conf.yml
3. ✅ 文件模板检查 - check_templ.yml
4. ✅ BOM字符检查 - check_bom.yml
5. ✅ 属性名称验证 - check_properties.yml
6. ✅ 编译测试 - ccpp.yml
7. ✅ 单元测试 - main.yml

⚠️ 特别注意事项

新 Widgets 开发要求:

1. 构造函数: lv_<widget_name>_create(lv_obj_t * parent)
2. API前缀: 所有函数以 lv_<widget_name>_ 开头
3. 私有数据: 使用 lv_<widget_name>_t 类型定义
4. 事件处理: 遵循标准事件回调格式，包含 user_data 参数
5. 样式继承: 正确处理样式继承和默认值
6. 内存管理: 确保析构函数正确释放资源

DCO (开发者原创声明):

- 确认代码为原创或兼容 MIT 许可
- 第三方代码需要明确许可声明
- 添加版权头（如果需要）

🛠️ 实用脚本汇总

# 安装依赖
scripts/install-prerequisites.sh

# 格式化代码
python scripts/code-format.py

# 生成内部配置
python scripts/lv_conf_internal_gen.py

# 运行测试
./tests/main.py test

# 运行预提交钩子
pre-commit run --all-files

# 清理并生成覆盖率报告
./tests/main.py --clean --report build test

📚 参考文档

- 代码规范: docs/src/contributing/coding_style.rst
- PR流程: docs/src/contributing/pull_requests.rst
- 贡献指南: docs/src/contributing/introduction.rst
- 测试指南: tests/README.md