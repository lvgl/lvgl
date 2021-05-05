```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/CONTRIBUTING.md
```

# Contributing

## Introduction

Join LVGL's community and leave your footprint in the library!

There are a lot of ways to contribute to LVGL even if you are are new to the library or even new to programming. 

It might be scary to make the first step but you have nothing to be afraid of. 
A friendly and helpful community is waiting for you. Get to know like-minded people and make something great together.

So let's find which contribution option fits you the best and help you join the development of LVGL!

Before getting started here are some guidelines to make contribution smoother:
- Be kind and friendly. 
- Be sure to read the relevant part of the documentation before posting a question.
- Ask questions in the [Forum](https://forum.lvgl.io/) and use [GitHub](https://github.com/lvgl/) for development-related discussions.
- Always fill out the post or issue templates in the Forum or GitHub (or at least provide equivalent information). It makes much easier to understand your case and you will get a useful answer faster. 
- If possible send an absolute minimal but buildable code example in order to reproduce the issue. Be sure it contains all the required variable declarations, constants, and assets (images, fonts).
- Use [Markdown](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet) to format your posts. You can learn it in 10 minutes.
- Speak about one thing in one issue or topic. It makes your post easier to find later for someone with the same question.
- Give feedback and close the issue or mark the topic as solved if your question is answered. 
- For non-trivial fixes and features, it's better to open an issue first to discuss the details instead of sending a pull request directly.
- Please read and follow the <a href="https://github.com/lvgl/lvgl/blob/master/docs/CODING_STYLE.md">Coding style</a> guide.

## Pull request


Merging new code into lvgl, documentation, blog, examples, and other repositories happen via *Pull requests* (PR for short).
A PR is a notification like "Hey, I made some updates to your project. Here are the changes, you can add them if you want."
To do this you need a copy (called fork) of the original project under your account, make some changes there, and notify the original repository about your updates. 
You can see how it looks like on GitHub for lvgl here: [https://github.com/lvgl/lvgl/pulls](https://github.com/lvgl/lvgl/pulls).

To add your changes you can edit files online on GitHub and send a new Pull request from there (recommended for small changes) or
 add the updates in your favorite editor/IDE and use git to publish the changes (recommended for more complex updates).

### From GitHub
1. Navigate to the file you want to edit.
2. Click the Edit button in the top right-hand corner.
3. Add your changes to the file
4. Add a commit message on the bottom of the page
5. Click the *Propose changes* button

### From command line

The instructions describe the main `lvgl` repository but it works the same way for the other repositories.
1. Fork the [lvgl repository](https://github.com/lvgl/lvgl). To do this click the "Fork" button in the top right corner. 
It will "copy" the `lvgl` repository to your GitHub account (`https://github.com/<YOUR_NAME>?tab=repositories`)
2. Clone your forked repository.
3. Add your changes. You can create a *feature branch* from *master* for the updates: `git checkout -b the-new-feature` 
4. Commit and push your changes to the forked `lvgl` repository.
5. Create a PR on GitHub from the page of your `lvgl` repository (`https://github.com/<YOUR_NAME>/lvgl`) by clicking the *"New pull request"* button. Don't forget to select the branch where you added your changes.
7. Set the base branch. It means where you want to merge your update. In the `lvgl` repo fixes go to `master`, new features to `dev` branch. 
8. Describe what is in the update. An example code is welcome if applicable.
9. If you need to make more changes, just update your forked `lvgl` repo with new commits. They will automatically appear in the PR.

## Developer Certification of Origin (DCO)

### Overview

To ensure that all licensing criteria is met for all repositories of the LVGL project we apply a process called DCO (Developer's Certificate of Origin).

The text of DCO can be read here: [https://developercertificate.org/](https://developercertificate.org/).

By contributing to any repositories of the LVGL project you state that your contribution corresponds with the DCO.

No further action is required if your contribution fulfills the DCO. If you are not sure about it feel free to ask us in a comment.

### Accepted licenses and copyright notices

To make the DCO easier to digest, here are some practical guides about specific cases:

#### Your own work

The simplest case is when the contribution is solely your own work. 
In this case you can just send a Pull Request without worrying about any licensing issues.

#### Use code from online source

If the code you would like to add is based on an article, post or comment on a website (e.g. StackOverflow) the license and/or rules of that site should be followed.

For example in case of StackOwerflow a notice like this can be used:
```
/* The original version of this code-snippet was published on StackOverflow.
 * Post: http://stackoverflow.com/questions/12345
 * Author: http://stackoverflow.com/users/12345/username
 * The following parts of the snippet were changed:
 * - Check this or that
 * - Optimize performance here and there
 */
 ... code snippet here ...
```

#### Use MIT licensed code
As LVGL is also MIT licensed other MIT licensed code can be integrated without issues.
The MIT license requests a copyright notice be added to the derived work. So you need to copy the original work's license file or it's text to the code you want to add.


#### Use GPL licensed code
As GPL license is not compatible with MIT license so LVGL can not accept GPL licensed code. 
 



## When you get started with LVGL

Even if you're just getting started with LVGL there are plenty of ways to get your feet wet. Most of these options don't even require knowing a single line of code of LVGL. 

### Give LVGL a Star

Show that you like LVGL by giving it star on GitHub!
<!-- Place this tag in your head or just before your close body tag. -->
<script async defer src="https://buttons.github.io/buttons.js"></script>
<!-- Place this tag where you want the button to render. -->
<a class="github-button" href="https://github.com/lvgl/lvgl" data-icon="octicon-star" data-size="large" data-show-count="true" aria-label="Star lvgl/lvgl on GitHub">Star</a>

This simple click makes LVGL more visible on GitHub and makes it more attractive to other people. 
So with this, you already helped a lot! 

### Tell what you have achieved

Have you already started using LVGL in a [Simulator](/get-started/pc-simulator), a development board, or your custom hardware? 
Was it easy or were there some obstacles? Are you happy with the result?

If so why don't you tell it to your friends? You can post it on Twitter, Facebook, LinkedIn, or create a YouTube video.

Any of these helps a lot to spread the word of LVGL and familiarize it with new developers.

Only thing: don't forget to add a link to `https://lvgl.io` or `https://github.com/lvgl` and `#lvgl`. Thank you! :) 

### Write examples
As you learn LVGL probably you will play with the features of widgets. But why don't you publish your experiments?

Every widgets' documentation contains some examples. For example here are the examples of the [Drop-down list](#example). 
The examples are directly loaded from the [lv_examples](https://github.com/lvgl/lv_examples/tree/master/src/lv_ex_widgets) repository. 

So all you need to do is send a [Pull request](#pull-request) to the [lv_examples](https://github.com/lvgl/lv_examples/tree/master/src/lv_ex_widgets) repository and follow some conventions:
- Name the examples like `lv_ex_<widget_name>_<id>`
- Make the example as short and simple as possible
- Add comments to explain what the example does
- Use 320x240 resolution
- Create a screenshot about the example 
- Update `index.rst` in the example's folder with your new example. See how the other examples are added.


### Improve the docs

As you read the documentation you might see some typos or unclear sentences. 
For typos and straightforward fixes, you can simply edit the file on GitHub. There is an `Edit on Github` link on the top right-hand corner of all pages.
Click it to see the file on GitHub, hit the Edit button, and add your fixes as described in [Pull request - From GitHub](#from-github) section.

Note that the documentation is also formatted in [Markdown](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet). 

### Translate the docs

If you have more free time you can even translate the documentation. The currently available languages are shown in the [locals](https://github.com/lvgl/docs/tree/latest/locales) folder.

If your chosen language is still not added, please write a comment [here](https://forum.lvgl.io/t/translate-the-documentation/238).

To add your translations:
- Find the *.po* in `<language_code>/LC_MESSAGES/<section_name>.po`. E.g. the widgets translated to German should be in `de/LC_MESSAGES/widgets.po`.
- Open a po file and fill the `msgstr` fields with the translation
- Send a [Pull request](#pull-request)

To display a translation in the public documentation page at least these sections should be translated:
- Get started: Quick overview
- Overview: Objects, Events, Styles
- Porting: System overview, Set-up a project, Display interface, Input device Interface, Tick interface
- 5 widgets of your choice

### Write a blog post

The [LVGL Blog](https://blog.lvgl.io) welcomes posts from anyone. It's a good place to talk about a project you created with LVGL, write a tutorial, or share some nice tricks.
The latest blog posts are shown on the [homepage of LVGL](https://lvgl.io) to make your work more visible.

The blog is hosted on GitHub. If you add a post GitHub automatically turns it into a website. See the [README](https://github.com/lvgl/blog) of the blog repo to see how to add your post.


## When you already use LVGL

### Give feedback
Let us know what you are working on! You can open a new topic in the [My projects](https://forum.lvgl.io/c/my-projects/10) category of the Forum.
Showing your project to others is a win-win situation because it increases your and LVGL's reputation at the same time.

If you don't want to speak about it publicly feel free to use [Contact form](https://lvgl.io/#contact) on lvgl.io to private message to us. 

### Report bugs
As you use LVGL you might find bugs. Before reporting them be sure to check the relevant parts of the documentation.

If it really seems like a bug feel free to open an [issue on GitHub](https://github.com/lvgl/lvgl/issues). 

When filing the issue be sure to fill the template. It helps a lot to find the root of the problems and helps to avoid a lot of questions.

### Send fixes
The beauty of open-source software is you can easily dig in to it to understand how it works. You can also fix or adjust it as you wish.

If you found and fixed a bug don't hesitate to send a [Pull request](#pull-request) with the fix.

In your Pull request please also add a line to [`CHANGELOG.md`](https://github.com/lvgl/lvgl/blob/master/CHANGELOG.md).

### Join the conversations in the Forum
It feels great to know you are not alone if something is not working. It's even better to help others when they struggle with something.

While you were learning LVGL you might have questions and used the Forum to get answers. As a result, you probably have more knowledge about how LVGL works.

One of the best ways to give back is to use the Forum and answer the questions of newcomers - like you were once.

Just read the titles and if you are familiar with the topic don't hesitate to share your thoughts and suggestions.

Participating in the discussions is one of the best ways to become part of the project and get to know like-minded people!
 

### Add features
We collect the planned features in GitHub issues tracker and mark them with [Help wanted](https://github.com/lvgl/lvgl/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22) label.
If you are interested in any of them feel free to share your opinion and/or participate in the the implementation.

Other features which are (still) not on the road map are listed in the [Feature request](https://forum.lvgl.io/c/feature-request/9) category of the Forum. 
If you have a feature idea for LVGL please use the Forum to share it! Make sure to check that there isn't an existing post; if there is, you should comment on it instead to show that there is increased interest in an existing request.

When adding a new features the followings also needs to be updated:
- Add a line to [CHANGELOG.md](https://github.com/lvgl/lvgl/blob/master/CHANGELOG.md).
- Update the documentation. See this [guide](#improve-the-docs).
- Add an example if applicable.  See this [guide](#write-examples).

## When you are confident with LVGL

### Become a maintainer

If you want to become part of the core development team, you can become a maintainer of a repository.

By becoming a maintainer:
- you get write access to that repo:
   - add code directly without sending a pull request
   - accept pull requests
   - close/reopen/edit issues
- your name will be added in the credits section of lvgl.io/about (will be added soon) and lvgl’s README.
- you can join the [Core_contributor](https://forum.lvgl.io/g/Core_contributor) group in the Forum and get the LVGL logo on your avatar.
- your word has higher impact when we make decisions

You can become a maintainer by invitation, however the following conditions need to met
1. Have > 50 replies in the Forum. You can look at your stats [here](https://forum.lvgl.io/u?period=all)
2. Send > 5 non-trivial pull requests to the repo where you would like to be a maintainer


If you are interested, just send a message (e.g. from the Forum) to the current maintainers of the repository. They will check is the prerequisites are met. 
Note that meeting the prerequisites is not a guarantee of acceptance, i.e. if the conditions are met you won't automatically become a maintainer. It's up to the current maintainers to make the decision.


### Move your project repository under LVGL organization
Besides the core `lvgl` repository there are other repos for ports to development boards, IDEs or other environment. If you ported LVGL to a new platform we can host it under the LVGL organization among the other repos.

This way your project will become part of the whole LVGL project and can get more visibility. If you are interested in this opportunity just open an [issue in lvgl repo](https://github.com/lvgl/lvgl/issues) and tell what you have! 

If we agree that your port is useful, we will open a repository for your project where you will have admin rights.  

To make this concept sustainable there a few rules to follow:
- You need to add a README to your repo.
- We expect to maintain the repo to some extent:
  - Follow at least the major versions of lvgl 
  - Respond to the issues (in a reasonable time)
- If there is no activity in a repo for 6 months it will be archived
