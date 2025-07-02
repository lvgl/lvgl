.. _contributing_pull_requests:

=============
Pull Requests
=============

Merging new code into the ``lvgl/lvgl`` and other
repositories happens via *Pull Requests* (PR for short). A PR is a
notification like "Hey, I made some updates to your project. Here are
the changes, you can add them if you want." To do this you need a copy
(called fork) of the original project under your account, make some
changes there, and notify the original repository about your updates.
You can see what it looks like on GitHub for LVGL here:
https://github.com/lvgl/lvgl/pulls.

To add your changes you can edit files online on GitHub and send a new
Pull request from there (recommended for small changes) or add the
updates in your favorite editor/IDE and use ``git`` to publish the changes
(recommended for more complex updates).



From GitHub
***********

1. Navigate to the file you want to edit.
2. Click the Edit button in the top right-hand corner.
3. Add your changes to the file.
4. Add a commit message at the bottom of the page.
5. Click the *Propose changes* button.



From Your Local Workstation
***************************

These instructions describe the main ``lvgl`` repository but it works the
same way any remote Git repository.

1. Fork the `lvgl repository <https://github.com/lvgl/lvgl>`__. To do this click the
   "Fork" button in the top right corner. It will "copy" the ``lvgl``
   repository to your GitHub account (``https://github.com/<YOUR_NAME>?tab=repositories``)
2. Clone your forked repository.
3. Add your changes. You can create a *feature branch* from the ``master`` branch for the updates: ``git checkout -b <new-feature-branch-name>``
4. Commit and push your changes to your forked ``lvgl`` repository.
5. Create a PR on GitHub from the page of your forked ``lvgl`` repository (``https://github.com/<YOUR_NAME>/lvgl``) by
   clicking the *"New pull request"* button. Don't forget to select the branch where you added your changes.
6. Set the base branch where you want to merge your update. In the ``lvgl`` repo both fixes
   and new features should be directed to the ``master`` branch.
7. Describe what is in the update.  Example code is welcome if applicable.
8. If you need to make more changes, just update your forked ``lvgl`` repo with new commits.
   They will automatically appear in the PR.



.. _contributing_commit_message_format:

Commit Message Format
*********************

The commit messages format is inspired by `Angular Commit
Format <https://github.com/angular/angular/blob/main/CONTRIBUTING.md#commit>`__.

The following structure should be used:

.. code-block:: text

    <type>(<scope>): <subject>
                <--- blank line
    <body>
                <--- blank line
    <footer>

Possible ``<type>``\ s:

- ``fix`` bugfix in LVGL source code
- ``feat`` new feature
- ``arch`` architectural changes
- ``perf`` changes that affect performance
- ``example`` anything related to examples (including fixes and new examples)
- ``docs`` anything related to documentation (including fixes, formatting, and new pages)
- ``test`` anything related to tests (new and updated tests or CI actions)
- ``chore`` any minor formatting or style changes that would make the changelog noisy

``<scope>`` is the name of the module, file, or subsystem affected by the
commit. It's usually one word and can be chosen freely. For example
``img``, ``layout``, ``txt``, ``anim``. The scope can be omitted.

``<subject>`` contains a short description of the change following these guidelines:

- use the imperative mood:  e.g. present tense "change", not "changed" nor "changes";
- don't capitalize the first letter;
- no period (``.``) at the end;
- max 90 characters.

``<body>`` optional and can be used to describe the details of this
change.

``<footer>`` shall contain:

- begin it with "BREAKING CHANGE" if the changes break the API;
- reference to the GitHub issue or Pull Request if applicable.
  (See `Linking a pull request to an issue <https://docs.github.com/en/get-started/writing-on-github/working-with-advanced-formatting/using-keywords-in-issues-and-pull-requests#linking-a-pull-request-to-an-issue>`__
  for details.)

Some examples:

.. code-block:: text

    fix(image): update size when a new source is set

.. code-block:: text

    fix(bar): fix memory leak

    The animations weren't deleted in the destructor.

    Fixes: #1234

.. code-block:: text

    feat: add span widget

    The span widget allows mixing different font sizes, colors and styles.
    It's similar to HTML <span>

.. code-block:: text

    docs(porting): fix typo
