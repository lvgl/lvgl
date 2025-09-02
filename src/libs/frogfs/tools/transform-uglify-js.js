const { needs, relaunch } = require('frogfs');

let installed = false;
installed |= needs('uglify-js');
if (installed) {
	relaunch();
}

const { exit, stdin, stdout } = require('process');
const uglifyjs = require('uglify-js');

var input = '';
stdin.setEncoding('utf-8');
stdin.on('data', (data) => {
	input = input.concat(data.toString());
});
stdin.on('close', () => {
	let result = uglifyjs.minify(input, {toplevel: true});
	if (result.code === undefined) {
		exit(1)
	}
	stdout.write(result.code);
	exit(0);
});
