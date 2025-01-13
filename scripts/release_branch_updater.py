import subprocess
import re
import argparse
import os
import shutil
import sys

# v10.0.0 -> fail if release/v10.0 is not there
# v10.0.1 -> update release/v10.0
# v10.1.0 -> create release/v10.1 from release/v10.0 and update it
# v10.1.1 -> update release/v10.1

LOG = "[release_branch_updater.py]"

def main():

    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("--port-clone-tmpdir", default="port_tmpdir")
    arg_parser.add_argument("--port-urls-path", default=os.path.join(os.path.dirname(__file__), "release_branch_updater_port_urls.txt"))
    arg_parser.add_argument("--lvgl-path", default=os.path.join(os.path.dirname(__file__), ".."))
    arg_parser.add_argument("--dry-run", action="store_true")
    arg_parser.add_argument("--oldest-major", type=int)
    args = arg_parser.parse_args()

    port_clone_tmpdir = args.port_clone_tmpdir
    port_urls_path = args.port_urls_path
    lvgl_path = args.lvgl_path
    dry_run = args.dry_run
    oldest_major = args.oldest_major

    lvgl_release_branches = get_release_branches(lvgl_path)
    print(LOG, "LVGL release branches:", ", ".join(fmt_release(br) for br in lvgl_release_branches) or "(none)")
    if oldest_major is not None:
        lvgl_release_branches = [br for br in lvgl_release_branches if br[0] >= oldest_major]
        print(LOG, 'LVGL release branches after "oldest-major" filter:',
              ", ".join(fmt_release(br) for br in lvgl_release_branches) or "(none)")

    with open(port_urls_path) as f:
        urls = f.read()
    urls = [url for url in map(str.strip, urls.splitlines()) if url]

    # ensure this script creates the directory i.e. it doesn't belong to the user since it will rm -rf at the end
    assert not os.path.exists(port_clone_tmpdir), "the port clone tmpdir should not exist yet"

    for url in urls:
        print(LOG, "working with port:", url)

        subprocess.check_call(("git", "clone", url, port_clone_tmpdir))

        port_release_branches = get_release_branches(port_clone_tmpdir)
        print(LOG, "port release branches:", ", ".join(fmt_release(br) for br in port_release_branches) or "(none)")

        # we want to
        # 1. create (if necessary) the port's release branch
        # 2. update the LVGL submodule to match the LVGL's release branch version
        # 3. update the lv_conf.h based on the lv_conf.defaults

        # from oldest to newest release...
        for lvgl_branch in lvgl_release_branches:
            print(LOG, f"attempting to update release branch {fmt_release(lvgl_branch)} ...")

            port_does_not_have_the_branch = False
            port_submodule_was_updated = False
            port_lv_conf_h_was_updated = False

            # if the branch does not exist in the port, create it from
            # the closest minor of the same major.
            if lvgl_branch in port_release_branches:
                print(LOG, "... this port has a matching release branch.")
                subprocess.check_call(("git", "-C", port_clone_tmpdir, "branch", "--track",
                                       fmt_release(lvgl_branch),
                                       f"origin/{fmt_release(lvgl_branch)}"))
            else:
                print(LOG, "... this port does not have this release branch minor ...")
                port_does_not_have_the_branch = True

                # get the port branch with this major and the next smallest minor
                create_from = next((
                    br
                    for br in reversed(port_release_branches) # reverse it to get the newest (largest) minor
                    if br[0] == lvgl_branch[0]     # same major
                       and br[1] < lvgl_branch[1]  # smaller minor because exact minor does not exist
                ), None)
                if create_from is None:
                    # there are no branches in the port that are this major
                    # version. One must be created manually.
                    print(LOG, "... this port has no major from which to create the minor. one must be created manually. continuing to next.")
                    continue

                print(LOG, f"... creating the new branch {fmt_release(lvgl_branch)} "
                                             f"from {fmt_release(create_from)}")
                subprocess.check_call(("git", "-C", port_clone_tmpdir, "branch",
                                       fmt_release(lvgl_branch),   # new branch name
                                       fmt_release(create_from)))  # start point

                port_release_branches.append(lvgl_branch)
                port_release_branches.sort()

            # checkout the same release in both LVGL and the port
            subprocess.check_call(("git", "-C", lvgl_path, "checkout", f"origin/{fmt_release(lvgl_branch)}"))
            subprocess.check_call(("git", "-C", port_clone_tmpdir, "checkout", fmt_release(lvgl_branch)))

            # update the submodule in the port if it exists
            out = subprocess.check_output(("git", "-C", port_clone_tmpdir, "config", "--file",
                                           ".gitmodules", "--get-regexp", "path"))
            port_lvgl_submodule_path = next((
                line.partition("lvgl.path ")[2]
                for line
                in out.decode().strip().splitlines()
                if "lvgl.path " in line
            ), None)
            if port_lvgl_submodule_path is None:
                print(LOG, "this port has no LVGL submodule")
            else:
                print(LOG, "lvgl submodule found in port at:", port_lvgl_submodule_path)

                # get the SHA of LVGL in this release of LVGL
                out = subprocess.check_output(("git", "-C", lvgl_path, "rev-parse", "--verify", "--quiet", "HEAD"))
                lvgl_sha = out.decode().strip()
                print(LOG, "the SHA of LVGL in this release should be:", lvgl_sha)

                # get the SHA of LVGL this port wants to use in this release
                out = subprocess.check_output(("git", "-C", port_clone_tmpdir, "rev-parse",
                                               "--verify", "--quiet", f"HEAD:{port_lvgl_submodule_path}"))
                port_lvgl_submodule_sha = out.decode().strip()
                print(LOG, "the SHA of LVGL in the submodule of this port is:", port_lvgl_submodule_sha)

                if lvgl_sha == port_lvgl_submodule_sha:
                    print(LOG, "the submodule's version of LVGL is already up to date")
                else:
                    print(LOG, "the submodule's version of LVGL is NOT up to date")
                    port_submodule_was_updated = True

                    # update the version of the submodule in the index. no need to `git submodule update --init` it.
                    # also no need to `git add .` afterwards because it stages the change.
                    # 160000 is a git file mode which means submodule.
                    subprocess.check_call(("git", "-C", port_clone_tmpdir, "update-index", "--cacheinfo",
                                           f"160000,{lvgl_sha},{port_lvgl_submodule_path}"))

            # update the lv_conf.h if there's an lv_conf.defaults
            out = subprocess.check_output(("find", ".", "-name", "lv_conf.defaults", "-print", "-quit"), cwd=port_clone_tmpdir)
            port_lv_conf_defaults = next(iter(out.decode().strip().splitlines()), None)
            if port_lv_conf_defaults is None:
                print(LOG, "this port has no lv_conf.defaults")
            else:
                out = subprocess.check_output(("find", ".", "-name", "lv_conf.h", "-print", "-quit"), cwd=port_clone_tmpdir)
                port_lv_conf_h = next(iter(out.decode().strip().splitlines()), None)
                if port_lv_conf_h is None:
                    print(LOG, "this port has an lv_conf.defaults but no lv_conf.h")
                else:
                    subprocess.check_call((sys.executable, os.path.join(lvgl_path, "scripts/generate_lv_conf.py"),
                                           "--defaults", os.path.abspath(os.path.join(port_clone_tmpdir, port_lv_conf_defaults)),
                                           "--config", os.path.abspath(os.path.join(port_clone_tmpdir, port_lv_conf_h)), ))

                    # check if lv_conf.h actually changed. it will not detect the submodule change as a false positive.
                    out = subprocess.check_output(("git", "-C", port_clone_tmpdir, "diff"))
                    diff = out.decode().strip()
                    if not diff:
                        print(LOG, "this port's lv_conf.h did NOT change")
                    else:
                        print(LOG, "this port's lv_conf.h changed")
                        port_lv_conf_h_was_updated = True
                        subprocess.check_call(("git", "-C", port_clone_tmpdir, "add", port_lv_conf_h))
                        out = subprocess.check_output(("git", "-C", port_clone_tmpdir, "diff"))
                        diff = out.decode().strip()
                        assert not diff

            if port_does_not_have_the_branch or port_submodule_was_updated or port_lv_conf_h_was_updated:
                print(LOG, "changes were made. ready to push.")
                # keep it brief for commit message 50 character limit suggestion.
                # max length will be 50 characters in this case: "CI release edit: new branch. submodule. lv_conf.h."
                commit_msg = ("CI release edit:"
                              + (" new branch." if port_does_not_have_the_branch else "")
                              + (" submodule." if port_submodule_was_updated else "")
                              + (" lv_conf.h." if port_lv_conf_h_was_updated else "")
                             )
                print(LOG, f"commit message: '{commit_msg}'")
                subprocess.check_call(("git", "-C", port_clone_tmpdir, "commit", "-m", commit_msg))
                if dry_run:
                    print(LOG, "this is a dry run so nothing will be pushed")
                else:
                    subprocess.check_call(("git", "-C", port_clone_tmpdir, "push",
                                           *(("-u", "origin") if port_does_not_have_the_branch else ()),
                                           fmt_release(lvgl_branch),
                                          ))
                    print(LOG, "the changes were pushed.")
            else:
                print(LOG, "nothing to push for this release. it is up to date.")

        shutil.rmtree(port_clone_tmpdir)

        print(LOG, "port update complete:", url)

def get_release_branches(working_dir):

    out = subprocess.check_output(("git", "-C", working_dir, "branch", "--quiet", "--format", "%(refname)", "--all"))
    branches = out.decode().strip().splitlines()

    release_versions = []
    for branch_name in branches:
        release_branch = re.fullmatch(r"refs/remotes/origin/release/v([0-9]+)\.([0-9]+)", branch_name)
        if release_branch is None:
            continue
        release_versions.append((int(release_branch[1]), int(release_branch[2])))

    release_versions.sort()

    return release_versions

def fmt_release(release_tuple):
    return f"release/v{release_tuple[0]}.{release_tuple[1]}"

if __name__ == "__main__":
    main()
