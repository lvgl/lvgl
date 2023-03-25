import os

from docutils import nodes
from docutils.parsers.rst import Directive, directives
# from docutils.parsers.rst.directives.images import Image
# from sphinx.directives.code import LiteralInclude


def excluded_list(argument):
    return argument.split(',')


class LvExample(Directive):
    required_arguments = 1
    option_spec = {
        'excluded_languages': excluded_list,
        'language': directives.unchanged,
        'description': directives.unchanged
    }

    def get_example_code_path(self, example_path, language):
        base_path = os.path.dirname(__file__)
        examples_path = os.path.abspath(os.path.join(base_path, '..', 'examples'))
        example_path = os.path.join(examples_path, example_path + '.' + language)
        return example_path

    def human_language_name(self, language):
        if language == 'py':
            return 'MicroPython'
        elif language == 'c':
            return 'C'
        else:
            return language

    def github_path(self, example_path, language):
        env = self.state.document.settings.env
        return f"https://github.com/lvgl/lvgl/blob/{env.config.repo_commit_hash}/examples/{example_path}.{language}"

    def embed_code(self, example_file, example_path, language, buttons={}):
        toggle = nodes.container('', literal_block=False, classes=['toggle'])
        header = nodes.container('', literal_block=False, classes=['header'])
        toggle.append(header)

        try:
            with open(example_file, 'rb') as f:
                contents = f.read().decode('utf-8')
        except FileNotFoundError:
            print('File Not Found', example_file)
            contents = 'Error encountered while trying to open ' + example_file

        literal_list = nodes.literal_block(contents, contents)
        literal_list['language'] = language
        toggle.append(literal_list)
        paragraph_node = nodes.raw(text=f"<p>{self.human_language_name(language)} code &nbsp;</p>", format='html')
        for text, url in buttons.items():
            paragraph_node.append(nodes.raw(text=f"<a class='lv-example-link-button' onclick=\"event.stopPropagation();\" href='{url}'>{text}</a>", format='html'))
        header.append(paragraph_node)
        return toggle

    def run(self):
        example_path = self.arguments[0]
        example_name = os.path.split(example_path)[1]
        excluded_languages = self.options.get('excluded_languages', [])
        node_list = []

        env = self.state.document.settings.env

        iframe_html = ""

        c_path = self.get_example_code_path(example_path, 'c')
        py_path = self.get_example_code_path(example_path, 'py')

        if os.path.exists(c_path):
            c_code = self.embed_code(c_path, example_path, 'c', buttons={
                '<i class="fa fa-github"></i>&nbsp;View on GitHub': self.github_path(example_path, 'c')
            })
        else:
            c_code = None

        if os.path.exists(py_path):
            py_code = self.embed_code(py_path, example_path, 'py', buttons={
                '<i class="fa fa-github"></i>&nbsp;View on GitHub': self.github_path(example_path, 'py'),
                '<i class="fa fa-play"></i>&nbsp;MicroPython Simulator': f"https://sim.lvgl.io/v{env.config.version}/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/{env.config.repo_commit_hash}/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/{env.config.repo_commit_hash}/examples/{example_path}.py"
            })
        else:
            py_code = None

        if 'c' not in excluded_languages:
            if env.app.tags.has('html'):
                iframe_html = f"<div class='lv-example' data-real-src='/{env.config.version}/_static/built_lv_examples/index.html?example={example_name}&w=320&h=240'></div>"

        description_html = f"<div class='lv-example-description'>{self.options.get('description', '')}</div>"
        layout_node = nodes.raw(text=f"<div class='lv-example-container'>{iframe_html}{description_html}</div>", format='html')

        node_list.append(layout_node)
        if 'c' not in excluded_languages and c_code is not None:
            node_list.append(c_code)
        if 'py' not in excluded_languages and py_code is not None:
            node_list.append(py_code)

        trailing_node = nodes.raw(text=f"<hr/>", format='html')
        node_list.append(trailing_node)

        return node_list


def setup(app):
    app.add_directive("lv_example", LvExample)
    app.add_config_value("repo_commit_hash", "", "env")

    return {
        'version': '0.1',
        'parallel_read_safe': True,
        'parallel_write_safe': True,
    }
