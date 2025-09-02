const { argv, env, exit, platform, stderr } = require('process');
const { spawnSync } = require('child_process');

const needs = (module) => {
    try {
        require(module + '/package');
    } catch {
        stderr.write("installing... ");
        let result;
        const args = ['--quiet', '--no-progress', '--prefix=' + env.NODE_PREFIX,
                'install', module];
        const options = {'stdio': ['ignore', 'ignore', 'inherit']};
        if (platform == 'win32') {
            result = spawnSync('cmd', ['/C', 'npm', ...args], options);
        } else {
            result = spawnSync('npm', args, options);
        }
        if (result.status != 0) {
            stderr.write('npm failed to run, is it installed?\n');
            exit(result.status);
        }
        return true;
    }
    return false;
}

const relaunch = () => {
    let result = spawnSync('node', argv.slice(1), {stdio: 'inherit'});
    exit(result.status);
}

exports.needs = needs;
exports.relaunch = relaunch;
