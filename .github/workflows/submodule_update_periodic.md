# Automatic submodule updates for LVGL port (`lv_port_*`) repos

This workflow automatically updates the submodule pointer and the configuration file in selected parent repositories of LVGL upon:
- When a release tag was added
- Periodically
- Manual trigger from GitHub

## Prerequisites

To be able to write to the parent repositories, we need a Personal Access Token (PAT) which looks like `ghp_****`.
\
More information can be found [here](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/managing-your-personal-access-tokens).

To securely store the token **we must create an environment**, called `env-submodule-update`, in the LVGL repository.
\
This must have a secret, `ACCESS_TOKEN`, for our token; and an environmental variable, `ORGANIZATION`, which should have the name of the organization, that owns the LVGL and it's parent repositories.
\
(So `lvgl` for production, and the developer's username for testing.)

![setup_env](https://github.com/user-attachments/assets/6c37355b-cab7-4677-af60-e93ef5a9b803)


## How it's working / maintenance

The whole logic was implemented in the `.yml` file, so have a single and latest version of the logic. (If if was a `.sh` file in LVGL, the release branches would have and outdated version upon change or fix in the script.) 

GitHub's built in matrix strategy is being used to iterate over target repositories, the list can be extended with the `parent_repo` field.
\
`fail-fast` was set to false, so a failure in one repo won't abort others (whom might already crated the tag)

The submodule folder is automatically detected, so `lvgl` and `src/lvgl` will both work.


### Periodic updates
Periodic updates are being run on the default branch of parent repositories and track the master branch of LVGL, selecting the latest available commit.
\
We trigger on the 1st and 15th day of every month, as bi-weekly logic is not supported by chron.

Steps:
- Update the submodule
- If `lv_conf.defaults` exists, config will be generated with `scripts/generate_lv_conf.py`
- Make a commit containing all changes and push

### Release updates

Upon making a tag in LVGL repo, the release logic is triggered (same script, but extra logic):
- Branch mirroring
  - If the release branch doesn't exist in the parent repo yet, it will be created at the current HEAD of the default branch.
  - Otherwise it will be checked out
- `[Same steps as periodic update]`
- Tag the commit with the same tag as in LVGL repo

The tag must have the following format: `vX.X.X`
\
Thee release branch will be derived from that and has the following format: `release/vX.X`.

### Manual trigger

The workflow can be manually triggered both for the master branch and for any tag. See more info [here](https://docs.github.com/en/actions/using-workflows/manually-running-a-workflow#configuring-a-workflow-to-run-manually)

>**Note:** Even if we trigger on older tags (like 7.0) the release branch still will be crated at the current HEAD. So the workflow can't be used to retrospectively build up the history.