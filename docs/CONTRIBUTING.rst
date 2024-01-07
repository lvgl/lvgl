.. _contributing:

Contributing
============

Introduction
------------

Join LVGL's community and leave your footprint in the library!

There are a lot of ways to contribute to LVGL even if you are new to the
library or even new to programming.

It might be scary to make the first step but you have nothing to be
afraid of. A friendly and helpful community is waiting for you. Get to
know like-minded people and make something great together.

So let's find which contribution option fits you the best and help you
join the development of LVGL!

Ways to contribute
-------------------

- **Spread the Word**: Share your LVGL experience with friends or on social media to boost its visibility.
- **Star the LVGL `repository <https://github.com/lvgl/lvgl>`__** Give us a star on GitHub! It helps a lot to LVGL more appealing for newcomers.
- **Report a bug**: Open a `GitHub Issue <https://github.com/lvgl/lvgl/issues>`__ if something is not working.
- **Join our `Forum <https://forum.lvgl.io/>`__**: Meet fellow developers and discuss questions
- **Tell your ideas**: If you miss something from LVGL we would love to hear about it in a `GitHub Issue <https://github.com/lvgl/lvgl/issues>`__
- **Develop features**: Develop a pitch! Learn more below.


Feature development workflow
----------------------------

Terminology
~~~~~~~~~~~

- **Shaping**: Convert raw ideas into something that can be developed. It includes examining opportunities, finding similar feature requests, roughly planning how it shall be implemented, tested, and taught through docs and examples.
- **Pitch**: A document that is the result of shaping.

Schedule
~~~~~~~~

We are working in 8-week cycles, each cycle starting at the beginning of odd months (e.g, a cycle could be from March to April).

- **Weeks 1-2: Cool Down** (A relaxed period to step back, plan, fix bugs, and make a release):
  - Close the previous development phase: Feature stop, bug fixes, release, discuss anything that can be improved.
  - Prepare for the next development phase: Ideas, shaping, pitch creation.
- **Weeks 3-8: Develop** Work on the planned pitches.


Feature Development Issue
~~~~~~~~~~~~~~~~~~~~~~~~~

For each raw idea, there is a dedicated issue where it's being "shaped". Anyone can open an idea/feature request issue and anyone can help the shaping process with comments.

We area using GitHub Labels to show the state of the issue:

- ``shaping``: Still discussing how to approach it.
- ``pitched``: We have come to a conclusion and created a pitch.
- ``request-change(<user-name>)``: Each core member has a dedicated label, and when they request a change, they add their label.
- ``under development``: It's being developed by someone (can be developed only if no change is requested by the end of the cool-down phase).
- ``ready``: The pitch is developed and merged into master.
- ``stale``: Inactive, can be warmed up.
- ``rejected``: Not interested in this feature for some reason.

Pitch Files
~~~~~~~~~~~

There is a `pitches <https://github.com/lvgl/lvgl/tree/pitches>`__ branch in the LVGL repository which contains the already finished and the planned pitch Markdown files.

See the `Template Pitch <https://github.com/lvgl/lvgl/blob/pitches/__pitch_template.md>`__ file to see how a pitch looks like.

Workflow
~~~~~~~~

1. Someone opens a new issue with an idea.
2. Others jump in to comment on it. Core members play an important role in really understanding the problem and possible solutions, and request changes if needed. This is the shaping phase.
3. It might take any length of time to shape an idea, but finally, it will be either converted to a pitch (created by a core member or a community member) or will become stale and closed.
4. Pitches are typically created during the cool-down phase. Once we have a pitch, the development of the pitch can be undertaken by anyone (original author of the idea, other community member, core member). If someone applies for the development of the pitch, he/she will be marked as Responsible in the issue.
5. Create a new branch for the developed pitch and continuously merge there via PRs.
6. A pitch is ready if tests, docs, and examples are added.
7. Finally, a PR is created against the ``master`` branch, which needs to be approved by the core members.
8. Anything merged to the ``master`` branch by the end of the Development phase will be part of the release. Anything not ready can be continued; however, if it takes too long, it should be reviewed and shaped again.
9. If a pitch is not taken by anyone in the next cool-down phase, it will be closed as stale.


.. _contributing_pull_request:

Pull request
------------

Merging new code into the lvgl, documentation, blog, examples, and other
repositories happen via *Pull requests* (PR for short). A PR is a
notification like "Hey, I made some updates to your project. Here are
the changes, you can add them if you want." To do this you need a copy
(called fork) of the original project under your account, make some
changes there, and notify the original repository about your updates.
You can see what it looks like on GitHub for LVGL here:
https://github.com/lvgl/lvgl/pulls.

To add your changes you can edit files online on GitHub and send a new
Pull request from there (recommended for small changes) or add the
updates in your favorite editor/IDE and use git to publish the changes
(recommended for more complex updates).

From GitHub
~~~~~~~~~~~

1. Navigate to the file you want to edit.
2. Click the Edit button in the top right-hand corner.
3. Add your changes to the file.
4. Add a commit message on the bottom of the page.
5. Click the *Propose changes* button.

From command line
~~~~~~~~~~~~~~~~~

The instructions describe the main ``lvgl`` repository but it works the
same way for the other repositories.

1. Fork the `lvgl repository <https://github.com/lvgl/lvgl>`__. To do this click the
   "Fork" button in the top right corner. It will "copy" the ``lvgl``
   repository to your GitHub account (``https://github.com/<YOUR_NAME>?tab=repositories``)
2. Clone your forked repository.
3. Add your changes. You can create a *feature branch* from *master* for the updates: ``git checkout -b <the-new-feature-branch-name>``
4. Commit and push your changes to the forked ``lvgl`` repository.
5. Create a PR on GitHub from the page of your ``lvgl`` repository (``https://github.com/<YOUR_NAME>/lvgl``) by
   clicking the *"New pull request"* button. Don't forget to select the branch where you added your changes.
6. Set the base branch. It means where you want to merge your update. In the ``lvgl`` repo both the fixes
   and new features go to ``master`` branch.
7. Describe what is in the update. An example code is welcome if applicable.
8. If you need to make more changes, just update your forked ``lvgl`` repo with new commits.
   They will automatically appear in the PR.

.. _contributing_commit_message_format:

Commit message format
~~~~~~~~~~~~~~~~~~~~~

The commit messages format is inspired by `Angular Commit
Format <https://gist.github.com/brianclements/841ea7bffdb01346392c>`__.

The following structure should be used:

.. code-block::

   <type>(<scope>): <subject>
   <BLANK LINE>
   <body>
   <BLANK LINE>
   <footer>

Possible ``<type>``\ s:

- ``fix`` bugfix in the source code.
- ``feat`` new feature
- ``arch`` architectural changes
- ``perf`` changes that affect the performance
- ``example`` anything related to examples (even fixes and new examples)
- ``docs`` anything related to the documentation (even fixes, formatting, and new pages)
- ``test`` anything related to tests (new and updated tests or CI actions)
- ``chore`` any minor formatting or style changes that would make the changelog noisy

``<scope>`` is the module, file, or sub-system that is affected by the
commit. It's usually one word and can be chosen freely. For example
``img``, ``layout``, ``txt``, ``anim``. The scope can be omitted.

``<subject>`` contains a short description of the change:

- use the imperative, present tense: "change" not "changed" nor "changes"
- don't capitalize the first letter
- no dot (``.``) at the end
- max 90 characters

``<body>`` optional and can be used to describe the details of this
change.

``<footer>`` shall contain

- the words "BREAKING CHANGE" if the changes break the API
- reference to the GitHub issue or Pull Request if applicable.

Some examples:

- fix(img): update size if a new source is set
- fix(bar): fix memory leak
  The animations weren't deleted in the destructor.

   Fixes: #1234
- feat: add span widget

   The span widget allows mixing different font sizes, colors and styles.
   It's similar to HTML <span>
- docs(porting): fix typo

.. _contributing_dco:

Developer Certification of Origin (DCO)
---------------------------------------

Overview
~~~~~~~~

To ensure all licensing criteria are met for every repository of the
LVGL project, we apply a process called DCO (Developer's Certificate of
Origin).

The text of DCO can be read here: https://developercertificate.org/.

By contributing to any repositories of the LVGL project you agree that
your contribution complies with the DCO.

If your contribution fulfills the requirements of the DCO no further
action is needed. If you are unsure feel free to ask us in a comment.

Accepted licenses and copyright notices
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To make the DCO easier to digest, here are some practical guides about
specific cases:

Your own work
^^^^^^^^^^^^^

The simplest case is when the contribution is solely your own work. In
this case you can just send a Pull Request without worrying about any
licensing issues.

Use code from online source
^^^^^^^^^^^^^^^^^^^^^^^^^^^

If the code you would like to add is based on an article, post or
comment on a website (e.g. StackOverflow) the license and/or rules of
that site should be followed.

For example in case of StackOverflow a notice like this can be used:

.. code-block::

   /* The original version of this code-snippet was published on StackOverflow.
    * Post: http://stackoverflow.com/questions/12345
    * Author: http://stackoverflow.com/users/12345/username
    * The following parts of the snippet were changed:
    * - Check this or that
    * - Optimize performance here and there
    */
    ... code snippet here ...

Use MIT licensed code
^^^^^^^^^^^^^^^^^^^^^

As LVGL is MIT licensed, other MIT licensed code can be integrated
without issues. The MIT license requires a copyright notice be added to
the derived work. Any derivative work based on MIT licensed code must
copy the original work's license file or text.

Use GPL licensed code
^^^^^^^^^^^^^^^^^^^^^

The GPL license is not compatible with the MIT license. Therefore, LVGL
can not accept GPL licensed code.
