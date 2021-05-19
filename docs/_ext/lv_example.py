from docutils.parsers.rst import Directive
from docutils import nodes
from docutils.parsers.rst.directives.images import Image
from sphinx.directives.code import LiteralInclude
import os

class LvExample(Directive):
    required_arguments = 3
    def run(self):
        example_path = self.arguments[0]
        example_name = os.path.split(example_path)[1]
        node_list = []

        env = self.state.document.settings.env

        if self.arguments[2] == 'py':
            paragraph_node = nodes.raw(text=f"Click to try in the simulator!<br/><a target='_blank' href='https://sim.lvgl.io/v7/micropython/ports/javascript/bundle_out/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lv_examples/{env.config.example_commit_hash}/src/header.py&script=https://raw.githubusercontent.com/lvgl/lv_examples/{env.config.built_example_commit_hash}/{example_name}/{example_name}.py'><img alt='{example_name}' src='https://raw.githubusercontent.com/lvgl/lv_examples/{env.config.built_example_commit_hash}/{example_name}/{example_name}.png'/></a>", format='html')
        else:
            paragraph_node = nodes.raw(text=f"<iframe class='lv-example' src='/{env.config.version}/_static/built_lv_examples?example={example_name}&w=320&h=240'></iframe>", format='html')
        toggle = nodes.container('', literal_block=False, classes=['toggle'])
        header = nodes.container('', literal_block=False, classes=['header'])
        toggle.append(header)
        example_file = os.path.abspath("../examples/" + example_path + "." + self.arguments[2])

        try:
            with open(example_file) as f:
                contents = f.read()
        except FileNotFoundError:
            contents = 'Error encountered while trying to open ' + example_file
        literal_list = nodes.literal_block(contents, contents)
        literal_list['language'] = self.arguments[2]
        toggle.append(literal_list)
        header.append(nodes.paragraph(text="code"))
        if env.app.tags.has('html'):
            node_list.append(paragraph_node)
        node_list.append(toggle)
        return node_list

def setup(app):
    app.add_directive("lv_example", LvExample)
    app.add_config_value("example_commit_hash", "", "env")
    app.add_config_value("built_example_commit_hash", "", "env")

    return {
        'version': '0.1',
        'parallel_read_safe': True,
        'parallel_write_safe': True,
    }
