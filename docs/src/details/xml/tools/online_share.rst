.. _editor_online_share:

============
Online Share
============



Overview
********

Online Share is a complementary tool for LVGL's Editor to make it easy to share the
developed UIs.

By using Online Share, GitHub repositories can be viewed and even edited in the
browser without setting up any developer environments.

Online Share supports all Editor features (Subjects, Translations, Animations, etc.)
except compiling code, so that the modifications of the C code cannot be applied.

Although in the browser it's possible to view and edit the XML files, it's not
possible to save the changes. In the future, a "Commit and Push to GitHub" and an
"Open Pull Request" button will be added to make the workflow more streamlined.



Usage
*****

Online Share is available at https://viewer.lvgl.io

On the opening screen, example and tutorial projects are available that can be
opened and tested right in the browser.

In the input field of "Open Project", the URL of a GitHub repository or a folder can
be pasted and by clicking "Load Project," in a few seconds it will be opened.

The folder pointed to by the URL needs to contain a ``project.xml`` and
``globals.xml``.

Some example URLs:

- ``https://github.com/ORG/REPO``: open the root of the repository on the default
  branch
- ``https://github.com/ORG/REPO/tree/BRANCH``: open the root of the repository from
  a given branch
- ``https://github.com/ORG/REPO/tree/BRANCH/FOLDER``: open a folder of the
  repository from a given branch



Licensing
*********

Online Share is available for public repositories for free. However, in order to use
it for private repositories, a subscription is required.

In both cases, it's required to log in with a GitHub account to avoid API rate
limits and access private repositories if needed.


Public Repositories
-------------------

Using Online Share with public repositories is very simple: just paste the URL to
the repository and hit the "Load Project" button.

It's a great way to:

- try out Online Share
- use the Editor for education
- use the Editor for open-source projects


Private Repositories
--------------------

The subscription is per-repository based and can be purchased for a month or year.
While having a valid subscription, there is no limit on the viewers. Basically,
anyone who has access to the private repository can log in with a GitHub account and
view the repository's content.
