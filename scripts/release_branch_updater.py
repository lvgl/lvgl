import subprocess
import re
import argparse
import os
import shutil
import sys

# Keeps two branches of every port repo in sync with LVGL:
#
#   master/main -> LVGL master
#   stable      -> the latest LVGL release tag (vX.Y.Z)
#
# `stable` never crosses a major version automatically because majors are breaking.
# If the latest release is a new major, `stable` is left alone and has to be
# migrated manually. After that the script keeps it up to date again.
#
# If a port has no `stable` branch yet, it is created from the port's newest
# release/vX.Y branch of the same major as the latest release, or from master/main
# if there is no such branch. The release/vX.Y branches themselves are no longer updated.
#
# Ports without an LVGL submodule only get their master/main updated.
#
# The bot only adds commits on top of the branches (no force push) and only touches
# the LVGL submodule and lv_conf.h. lv_conf.h is regenerated from lv_conf.defaults,
# so port specific config changes must go to lv_conf.defaults.

LOG = "[release_branch_updater.py]"
STABLE_BRANCH = "stable"


def git_repository(repository: str, token: str):
    if not token:
        return f"https://{repository}"
    return f"https://{token}@{repository}"


def main():

    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("--port-clone-tmpdir", default="port_tmpdir")
    arg_parser.add_argument("--port-urls-path", default=os.path.join(os.path.dirname(__file__), "release_branch_updater_port_urls.txt"))
    arg_parser.add_argument("--lvgl-path", default=os.path.join(os.path.dirname(__file__), ".."))
    arg_parser.add_argument("--dry-run", action="store_true")
    arg_parser.add_argument("--github-token", type=str)
    arg_parser.add_argument("--skip-master", action="store_true", help="don't update the ports' master/main branch")
    arg_parser.add_argument("--skip-stable", action="store_true", help="don't update the ports' stable branch")

    args = arg_parser.parse_args()

    port_clone_tmpdir = args.port_clone_tmpdir
    lvgl_path = args.lvgl_path
    dry_run = args.dry_run

    if not args.github_token and not dry_run:
        print(LOG, "Warning: No github token was provided for this production run. Continuing anyway...")

    _, lvgl_default_branch = get_release_branches(lvgl_path)
    assert lvgl_default_branch is not None
    print(LOG, "LVGL default branch:", lvgl_default_branch)

    latest_release = get_latest_release_tag(lvgl_path)
    assert latest_release is not None, "no vX.Y.Z release tag found in LVGL"
    print(LOG, "LVGL latest release:", fmt_tag(latest_release))

    with open(args.port_urls_path) as f:
        urls = [url for url in map(str.strip, f.read().splitlines()) if url]

    # ensure this script creates the directory i.e. it doesn't belong to the user since it will rm -rf at the end
    assert not os.path.exists(port_clone_tmpdir), "the port clone tmpdir should not exist yet"

    for url in urls:
        print(LOG, "working with port:", url)

        if dry_run:
            port_clone_tmpdir = url[len("https://github.com/lvgl/"):]
            print("port_clone_tmpdir: " + port_clone_tmpdir)

        # It's very important to not leak the github_token here
        # So make sure the stdout and stderr are piped here
        subprocess.run(("git", "clone",
                        git_repository(url.replace("https://", ""), args.github_token),
                        port_clone_tmpdir),
                       stdout=subprocess.PIPE,
                       stderr=subprocess.PIPE)

        port_release_branches, port_default_branch = get_release_branches(port_clone_tmpdir)
        print(LOG, "port default branch:", port_default_branch if port_default_branch is not None else "(none)")

        if not args.skip_master:
            if port_default_branch is None:
                print(LOG, "the port does not have a default branch to update.")
            else:
                print(LOG, f"attempting to update default branch {port_default_branch} ...")
                subprocess.check_call(("git", "-C", port_clone_tmpdir, "checkout", "-B",
                                       port_default_branch, f"origin/{port_default_branch}"))
                update_port_branch(port_clone_tmpdir, port_default_branch, lvgl_path,
                                   f"origin/{lvgl_default_branch}", lvgl_default_branch,
                                   is_new_branch=False, dry_run=dry_run)

        if not args.skip_stable:
            print(LOG, f"attempting to update {STABLE_BRANCH} to {fmt_tag(latest_release)} ...")
            update_stable(port_clone_tmpdir, port_release_branches, port_default_branch,
                          lvgl_path, latest_release, dry_run)

        if not dry_run:
            shutil.rmtree(port_clone_tmpdir)

        print(LOG, "port update complete:", url)


def update_stable(port_dir, port_release_branches, port_default_branch, lvgl_path, latest_release, dry_run):
    latest_major = latest_release[0]
    tag = fmt_tag(latest_release)

    if remote_branch_exists(port_dir, STABLE_BRANCH):
        base = STABLE_BRANCH
        is_new_branch = False
    else:
        # prefer the newest release/vX.Y branch of the same major (migration of the existing ports),
        # otherwise create it from the default branch (new ports)
        candidates = [br for br in port_release_branches if br[0] == latest_major]
        if candidates:
            base = fmt_release(candidates[-1])
        elif port_default_branch is not None:
            base = port_default_branch
        else:
            print(LOG, f"... this port has no {STABLE_BRANCH} branch and nothing to create it from. skipping.")
            return
        print(LOG, f"... this port has no {STABLE_BRANCH} branch. creating it from origin/{base}")
        is_new_branch = True

    # nothing is pushed if one of the checks below fails, so a newly created branch is just discarded
    subprocess.check_call(("git", "-C", port_dir, "checkout", "-B", STABLE_BRANCH, f"origin/{base}"))

    # ports without an LVGL submodule (e.g. Zephyr, ESP-IDF) pull LVGL in another way.
    # only their master is maintained.
    submodule_path = get_lvgl_submodule_path(port_dir)
    if submodule_path is None:
        print(LOG, f"... this port has no LVGL submodule. {STABLE_BRANCH} is not maintained. skipping.")
        return

    # don't cross a major version automatically
    port_major = get_lvgl_major_of_submodule(port_dir, submodule_path, lvgl_path)
    if port_major is None:
        print(LOG, f"... can't determine the LVGL major of origin/{base}. skipping. update it manually.")
        return
    if port_major != latest_major:
        print(LOG, f"... origin/{base} is on LVGL v{port_major} but the latest release is "
                   f"v{latest_major}. major versions are not updated automatically. skipping. "
                   "update it manually.")
        return

    # don't move the submodule backwards or sideways, e.g. if it was manually set to a fix
    # on release/vX.Y that's newer than the latest tag.
    # not checked when created from master/main: LVGL's master is never an ancestor of a release tag.
    port_sha = get_submodule_sha(port_dir, submodule_path)
    res = subprocess.run(("git", "-C", lvgl_path, "merge-base", "--is-ancestor", port_sha, f"{tag}^{{commit}}"))
    if res.returncode != 0 and base != port_default_branch:
        res = subprocess.run(("git", "-C", lvgl_path, "merge-base", "--is-ancestor", f"{tag}^{{commit}}", port_sha))
        if is_new_branch and res.returncode == 0:
            # e.g. the old release/vX.Y branches tracked the tip of LVGL's release/vX.Y, which can be
            # ahead of the latest tag. create stable as is and move to the next tag when it's released.
            print(LOG, f"... the LVGL submodule of origin/{base} is ahead of {tag}. "
                       f"creating {STABLE_BRANCH} without changing it.")
            push_branch(port_dir, STABLE_BRANCH, dry_run)
            return
        print(LOG, f"... the LVGL submodule of origin/{base} ({port_sha}) is not an ancestor of {tag}. "
                   "skipping to not overwrite a manual update.")
        return

    update_port_branch(port_dir, STABLE_BRANCH, lvgl_path, f"{tag}^{{commit}}", tag,
                       is_new_branch=is_new_branch, dry_run=dry_run)


def update_port_branch(port_dir, port_branch, lvgl_path, lvgl_ref, lvgl_ref_name, is_new_branch, dry_run):
    """
    The port branch must already be checked out.
    1. update the LVGL submodule to `lvgl_ref`
    2. update lv_conf.h based on lv_conf.defaults
    3. commit and push if anything changed
    """
    port_submodule_was_updated = False
    port_lv_conf_h_was_updated = False

    subprocess.check_call(("git", "-C", lvgl_path, "checkout", lvgl_ref))

    submodule_path = get_lvgl_submodule_path(port_dir)
    if submodule_path is None:
        print(LOG, "this port has no LVGL submodule")
    else:
        print(LOG, "lvgl submodule found in port at:", submodule_path)

        out = subprocess.check_output(("git", "-C", lvgl_path, "rev-parse", "--verify", "HEAD"))
        lvgl_sha = out.decode().strip()
        print(LOG, f"the SHA of LVGL {lvgl_ref_name} is:", lvgl_sha)

        port_lvgl_submodule_sha = get_submodule_sha(port_dir, submodule_path)
        print(LOG, "the SHA of LVGL in the submodule of this port is:", port_lvgl_submodule_sha)

        if lvgl_sha == port_lvgl_submodule_sha:
            print(LOG, "the submodule's version of LVGL is already up to date")
        else:
            print(LOG, "the submodule's version of LVGL is NOT up to date")
            port_submodule_was_updated = True

            # update the version of the submodule in the index. no need to `git submodule update --init` it.
            # also no need to `git add .` afterwards because it stages the change.
            # 160000 is a git file mode which means submodule.
            subprocess.check_call(("git", "-C", port_dir, "update-index", "--cacheinfo",
                                   f"160000,{lvgl_sha},{submodule_path}"))

    # update the lv_conf.h if there's an lv_conf.defaults
    out = subprocess.check_output(("find", ".", "-name", "lv_conf.defaults", "-print", "-quit"), cwd=port_dir)
    port_lv_conf_defaults = next(iter(out.decode().strip().splitlines()), None)
    if port_lv_conf_defaults is None:
        print(LOG, "this port has no lv_conf.defaults")
    else:
        out = subprocess.check_output(("find", ".", "-name", "lv_conf.h", "-print", "-quit"), cwd=port_dir)
        port_lv_conf_h = next(iter(out.decode().strip().splitlines()), None)
        if port_lv_conf_h is None:
            print(LOG, "this port has an lv_conf.defaults but no lv_conf.h")
        else:
            subprocess.check_call((sys.executable, os.path.join(lvgl_path, "scripts/generate_lv_conf.py"),
                                   "--defaults", os.path.abspath(os.path.join(port_dir, port_lv_conf_defaults)),
                                   "--config", os.path.abspath(os.path.join(port_dir, port_lv_conf_h)), ))

            # check if lv_conf.h actually changed. it will not detect the submodule change as a false positive.
            out = subprocess.check_output(("git", "-C", port_dir, "diff"))
            if not out.decode().strip():
                print(LOG, "this port's lv_conf.h did NOT change")
            else:
                print(LOG, "this port's lv_conf.h changed")
                port_lv_conf_h_was_updated = True
                subprocess.check_call(("git", "-C", port_dir, "add", port_lv_conf_h))
                out = subprocess.check_output(("git", "-C", port_dir, "diff"))
                assert not out.decode().strip()

    if not (is_new_branch or port_submodule_was_updated or port_lv_conf_h_was_updated):
        print(LOG, f"nothing to push for {port_branch}. it is up to date.")
        return

    print(LOG, "changes were made. ready to push.")
    # keep it brief for commit message 50 character limit suggestion.
    commit_msg = ("bot:"
                  + (" New branch." if is_new_branch else "")
                  + (f" Update LVGL to {lvgl_ref_name}." if port_submodule_was_updated else "")
                  + (" lv_conf.h." if port_lv_conf_h_was_updated else "")
                  )
    print(LOG, f"commit message: '{commit_msg}'")
    subprocess.check_call(("git", "-C", port_dir, "commit", "--allow-empty", "-m", commit_msg))
    push_branch(port_dir, port_branch, dry_run)


def push_branch(port_dir, port_branch, dry_run):
    if dry_run:
        print(LOG, "this is a dry run so nothing will be pushed")
    else:
        # a plain (non-force) push: it never overwrites commits pushed to the port in the meantime.
        # if that happened, it's rejected and the next run will retry.
        res = subprocess.run(("git", "-C", port_dir, "push", "origin", port_branch))
        if res.returncode == 0:
            print(LOG, "the changes were pushed.")
        else:
            print(LOG, f"failed to push {port_branch}. continuing.")


def get_lvgl_submodule_path(port_dir):
    if not os.path.exists(os.path.join(port_dir, ".gitmodules")):
        return None

    res = subprocess.run(("git", "-C", port_dir, "config", "--file",
                          ".gitmodules", "--get-regexp", "path"),
                         stdout=subprocess.PIPE)
    path = next((
        line.partition("lvgl.path ")[2]
        for line
        in res.stdout.decode().strip().splitlines()
        if "lvgl.path " in line
    ), None)
    if path is None:
        return None

    # check if the submodule is really in the index and not just a leftover in .gitmodules
    out = subprocess.check_output(("git", "-C", port_dir, "submodule", "status"))
    if not any(
        line.split(maxsplit=1)[1].rsplit(maxsplit=1)[0] == path
        for line
        in out.decode().strip().splitlines()
    ):
        return None

    return path


def get_submodule_sha(port_dir, submodule_path):
    out = subprocess.check_output(("git", "-C", port_dir, "rev-parse",
                                   "--verify", f"HEAD:{submodule_path}"))
    return out.decode().strip()


def get_lvgl_major_of_submodule(port_dir, submodule_path, lvgl_path):
    sha = get_submodule_sha(port_dir, submodule_path)
    # the define has moved around between versions
    for path in ("include/lvgl/lv_version.h", "lv_version.h", "lvgl.h"):
        res = subprocess.run(("git", "-C", lvgl_path, "show", f"{sha}:{path}"),
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if res.returncode != 0:
            continue
        m = re.search(r"#define\s+LVGL_VERSION_MAJOR\s+([0-9]+)", res.stdout.decode())
        if m:
            return int(m[1])
    return None


def remote_branch_exists(working_dir, branch):
    res = subprocess.run(("git", "-C", working_dir, "rev-parse", "--verify", "--quiet",
                          f"refs/remotes/origin/{branch}"), stdout=subprocess.PIPE)
    return res.returncode == 0


def get_latest_release_tag(working_dir):
    out = subprocess.check_output(("git", "-C", working_dir, "tag", "--list", "v*"))
    versions = []
    for tag in out.decode().strip().splitlines():
        m = re.fullmatch(r"v([0-9]+)\.([0-9]+)\.([0-9]+)", tag)
        if m:
            versions.append(tuple(int(x) for x in m.groups()))
    return max(versions) if versions else None


def get_release_branches(working_dir):

    out = subprocess.check_output(("git", "-C", working_dir, "branch", "--format", "%(refname)", "--all"))
    branches = out.decode().strip().splitlines()

    release_versions = []
    for branch_name in branches:
        release_branch = re.fullmatch(r"refs/remotes/origin/release/v([0-9]+)\.([0-9]+)", branch_name)
        if release_branch is None:
            continue
        release_versions.append((int(release_branch[1]), int(release_branch[2])))

    release_versions.sort()

    default_branch = None
    if "refs/remotes/origin/master" in branches:
        default_branch = "master"
    elif "refs/remotes/origin/main" in branches:
        default_branch = "main"

    return release_versions, default_branch


def fmt_release(release_tuple):
    return f"release/v{release_tuple[0]}.{release_tuple[1]}"


def fmt_tag(version_tuple):
    return f"v{version_tuple[0]}.{version_tuple[1]}.{version_tuple[2]}"


if __name__ == "__main__":
    main()
