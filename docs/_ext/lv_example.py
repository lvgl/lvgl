import os

from docutils import nodes
from docutils.parsers.rst import Directive, directives
from docutils.parsers.rst.directives.images import Image
from sphinx.directives.code import LiteralInclude


def excluded_list(argument):
    return argument.split(',')




class LvExample(Directive):
    required_arguments = 1
    option_spec = {
        'excluded_languages': excluded_list,
        'language': directives.unchanged
    }
    def get_example_code_path(self, example_path, language):
        return os.path.abspath("../examples/" + example_path + "." + language)
    def human_language_name(self, language):
        if language == 'py':
            return 'MicroPython'
        elif language == 'c':
            return 'C'
        else:
            return language
    def embed_code(self, example_file, example_path, language):
        env = self.state.document.settings.env
        toggle = nodes.container('', literal_block=False, classes=['toggle'])
        header = nodes.container('', literal_block=False, classes=['header'])
        toggle.append(header)
        try:
            with open(example_file) as f:
                contents = f.read()
        except FileNotFoundError:
            contents = 'Error encountered while trying to open ' + example_file
        literal_list = nodes.literal_block(contents, contents)
        literal_list['language'] = language
        toggle.append(literal_list)
        header.append(nodes.raw(text=f"<p>{self.human_language_name(language)} code &nbsp; <a onclick=\"event.stopPropagation();\" class='fa fa-github' href='https://github.com/lvgl/lvgl/blob/{env.config.repo_commit_hash}/examples/{example_path}.{language}'>&nbsp; view on GitHub</a></p>", format='html'))
        return toggle
    def run(self):
        example_path = self.arguments[0]
        example_name = os.path.split(example_path)[1]
        excluded_languages = self.options.get('excluded_languages', [])
        node_list = []

        env = self.state.document.settings.env

        iframe_node = nodes.raw(text=f"<iframe class='lv-example' src='/{env.config.version}/_static/built_lv_examples?example={example_name}&w=320&h=240'></iframe>", format='html')
        micropython_node = nodes.raw(text=f"<a style='display: inline-block; margin-bottom: 1rem;' target='_blank' href='https://sim.lvgl.io/v{env.config.version}/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/{env.config.repo_commit_hash}/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/{env.config.repo_commit_hash}/examples/{example_path}.py'>Click to try in the MicroPython simulator!</a>", format='html')

        c_path = self.get_example_code_path(example_path, 'c')
        py_path = self.get_example_code_path(example_path, 'py')

        c_code = self.embed_code(c_path, example_path, 'c')
        py_code = self.embed_code(py_path, example_path, 'py')

        if not 'c' in excluded_languages:   
            if env.app.tags.has('html'):
                node_list.append(iframe_node)
        if not 'py' in excluded_languages:
            node_list.append(micropython_node)
        if not 'c' in excluded_languages:
            node_list.append(c_code)
        if not 'py' in excluded_languages:
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
