const { needs, relaunch } = require('frogfs');

let installed = false;
installed |= needs('uglifycss');
if (installed) {
	relaunch();
}

const { exit, stdin, stdout } = require('process');
const uglifycss = require('uglifycss');

var input = '';
stdin.setEncoding('utf-8');
stdin.on('data', (data) => {
	input = input.concat(data.toString());
});
stdin.on('close', () => {
	let output = uglifycss.processString(input);
	stdout.write(output);
	exit(0);
});
