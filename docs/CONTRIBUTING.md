# Contributing to LVGL

**Do you have some free time to spend with programming?
Are you working on an embedded GUI project with LVGL?
See how can you help to improve the graphics library and help others!**


- [Overview](#overview)
- [How to send a pull request?](#how-to-send-a-pull-request) 
- [Help others in the Forum](help-others-in-the-forum)
- [Improve or translate the documentation](#improve-or-translate-the-documentation)
- [Write a blog post](#write-a-blog-post)
- [Report or fix bugs](#report-or-fix-bugs)
- [Suggest or implement new features](#suggest-or-implement-new-features)
- [Summary](#summary)


## Overview

There are many ways to join the community. If you have some time to work with us you will surely find something that fits you! You can:
- **Help others** in the [Forum](https://forum.lvgl.io).
- **Inspire people** by speaking about your project in [My project](https://forum.lvgl.io/c/my-projects) category in the Forum.
- **Improve and/or translate the documentation.** Go to the [Documentation](https://github.com/lvgl/docs) repository to learn more
- **Write a blog post** about your experiences. See how to do it in the [Blog](https://github.com/lvgl/blog) repository
- **Report and/or fix bugs** in [GitHub's issue tracker](https://github.com/lvgl/lvgl/issues)
- **Help the development**. Check the [Open issues](https://github.com/lvgl/lvgl/issues) especially the ones with [Help wanted](https://github.com/lvgl/lvgl/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22) label and tell your ideas about a topic or implement a feature.

We have some simple rules:
- Be kind and friendly.
- Speak about one thing in one issue/topic.
- Give feedback and close the issue or mark the topic as solved if your question is answered. 
- Tell what you experience or expect. _"The button is not working"_ is not enough info to get help.
- Use [Markdown](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet) to format your post.
- We use the [Forum](https://forum.lvgl.io/) to ask and answer questions and [GitHub's issue tracker](https://github.com/lvgl/lvgl/issues) for development-related discussion.
- If possible send an absolute minimal code example in order to reproduce the issue


## How to send a pull request?

Merging new code into LVGL, documentation, blog, and examples happens via *Pull requests*. If you are still not familiar with  Pull Requests (PR for short) here is a short guide. It's about the `lvgl` repository but it works the same way for other repositories too.
1. **Fork** the [lvgl repository](https://github.com/lvgl/lvgl). To do this click the "Fork" button in the top right corner. It will "copy" the `lvgl` repository to your GitHub account (`https://github.com/your_name?tab=repositories`)
2. **Clone**  the forked repository and add your changes
3. **Create a PR** on GitHub from the page of your `lvgl` repository (`https://github.com/your_name/lvgl`) by hitting the "New pull request" button 
4. **Set the base branch**. It means where you want to merge your update. Fixes go to `master`, new features to feature branch. 
5. **Describe** what is in the update. An example code is welcome if applicable.
6. **Update** your `lvgl` branch with new commits. They will appear in the PR too.

Some advice:
- For non-trivial fixes and features it's better open an issue first to discuss the details.
- Maybe your fix or update won't be perfect at first. Don't be afraid, just improve it and push the new commits. The PR will be updated accordingly. 
- If your update needs some extra work it's okay to say: _"I'm busy now and I will improve it soon"_ or _"Sorry, I don't have time to improve it, I hope it helps in this form too"_. 
So it's better to say don't have time to continue than saying nothing.
- Please read and follow this [guide about the coding style](https://github.com/lvgl/lvgl/blob/master/docs/CODING_STYLE.md)

## Help others in the Forum

It's a great way to contribute to the library if you already use it. 
Just go to [https://forum.lvgl.io/](https://forum.lvgl.io/) a register (Google and GitHub login also works).
Log in, read the titles and if you are already familiar with a topic, don't be shy, and write your suggestion.

## Improve or translate the documentation

If you would like to contribute to LVGL the documentation is the best place to start.

### Fix typos, add missing parts

If you find a typo, an obscure sentence or something which is not explained well enough in the [English documentation](https://docs.lvgl.io/en/html/index.html) 
click the *"Edit on GitHub"* button in the top right corner and fix the issue by sending a Pull Request.

### Translate the documentation

If you have time and interest you can translate the documentation to your native language or any language you speak well. 
You can join others to work on an already existing language or you can start a new one.  

To translate the documentation we use [Zanata](https://zanata.org) which is an online translation platform. 
You will find the LVGL project here: [LVGL on Zanata](https://translate.zanata.org/iteration/view/littlevgl-docs/v6.0-doc1?dswid=3430) 

To get started you need to:
- register at [Zanata](https://zanata.org) which is an online translation platform.  
- comment to [this post](https://forum.lvgl.io/t/translate-the-documentation/238?u=kisvegabor)
- tell your username at *Zanata* and your selected language(s) to get permission the edit the translations

Note that a translation will be added to the documentation only if the following parts are translated: 
- [Home page](https://docs.lvgl.io/en/v7/)
- [Porting section](https://docs.lvgl.io/en/v7/html/porting/index.html) 
- [Quick overview](https://docs.lvgl.io/v7/en/html/get-started/quick-overview.html)

## Write a blog post

Have you ported LVGL to a new platform or created a fancy GUI? Do you know a great trick? 
You can share your knowledge on LVGL's blog! It's super easy to add your own post:
- Fork and clone the [blog repository](https://github.com/lvgl/blog)
- Add your post in Markdown to the `_posts` folder. 
- Store the images and other resources in a dedicated folder in `assets`
- Create a Pull Request

The blog uses [Jekyll](https://jekyllrb.com/) to convert the `.md` files to a webpage. You can easily [run Jekyll offline](https://jekyllrb.com/docs/) to check your post before creating the Pull request

## Report or fix bugs
For simple bugfixes (typos, missing error handling, fixing a warning) it's fine to send a Pull request directly. However, for more complex bugs it's better to open an issue first. In the issue, you should describe how to reproduce the bug and add the minimal code snippet.

## Suggest or implement new features
If you have a good idea don't hesitate to share with us. It's even better if you have time to deal with its implementation. Don't be afraid if you still don't know LVGL well enough. We will help you to get started. 

To share your ideas use [Feature request](https://forum.lvgl.io/c/feature-request/9) category of the forum.

If you are ready to get involved into the development of this faetures feel free to pen a [new issue](https://github.com/lvgl/lvgl/issues) for it on GitHub.

During the implementation don't forget the [Code style guide](https://github.com/lvgl/lvgl/blob/master/docs/CODING_STYLE.md).

If you implemented a new feature it's important to record it in the documentation and if applicable create an example for it:
- Go to the [docs](https://github.com/lvgl/docs/tree/master/v7/en) repository and update the relevant part of the English documentation.
- Go to the [examples](https://github.com/lvgl/lv_examples) repository and add a new file about the new feature in the related directory. 

## Summary

I hope you have taken a liking to contribute to LVGL. A helpful and friendly community is waiting for you! :) 
