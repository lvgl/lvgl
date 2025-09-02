const { needs, relaunch } = require('frogfs');

let installed = false;
installed |= needs('@babel/core');
installed |= needs('babel-preset-minify')
if (installed) {
	relaunch();
}

const { exit, stdin, stdout } = require('process');
const babel = require('@babel/core');

var input = '';
stdin.setEncoding('utf-8');
stdin.on('data', (data) => {
	input = input.concat(data.toString());
});
stdin.on('close', () => {
	let result = babel.transformSync(input, {presets: ['minify']});
	stdout.write(result.code);
	exit(0);
});
