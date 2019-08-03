# Contributing to Littlev Graphics Library

**Do you have some free time to spend with programming?
Are you working on an Embedded GUI project with LittlevGL?
See how can you help to improve the graphics library!**

There are many ways to join the community. If you have some time to work with us I'm sure you will find something that fits you! You can:
- help others in the [Forum](https://forum.littlevgl.com/)
- improve and/or translate the documentation
- write a blog post about your experiences
- report and/or fix bugs
- suggest and/or implement new features

But first, start with the most Frequently Asked Questions.

# FAQ about contributing

## Where can I write my question and remarks?

We use the [Forum](https://forum.littlevgl.com/) to ask and answer questions and [GitHub's issue tracker](https://github.com/littlevgl/lvgl/issues) for development-related discussion.

But there are some simple rules:
- Be kind and friendly.
- Speak about one thing in one issue/topic.
- Give feedback and close the issue or mark the topic as solved if your question is answered. 
- Tell what you experience or expect. _"The button is not working"_ is not enough info to get help.
- If possible send an absolute minimal code example in order to reproduce the issue
- Use [Markdown](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet) to format your post.

## How can I send fixes and improvements?

Merging new code happens via Pull Requests. If you are still not familiar with the Pull Requests (PR for short) here is a quick guide:
1. **Fork** the [lvgl repository](https://github.com/littlevgl/lvgl). To do this click the "Fork" button in the top right corner. It will "copy" the `lvgl` repository to your GitHub account (`https://github.com/your_name?tab=repositories`)
2. **Clone**  the forked repository and add your changes
3. **Create a PR** on GitHub from the page of your `lvgl` repository (`https://github.com/your_name/lvgl`) by hitting the "New pull request" button 
4. **Set the base branch**. It means where you want to merge your update. Fixes go to `master`, new features to the actual `dev-x.y` branch. 
5. **Describe** what is in the update. An example code is welcome if applicable.

Some advice:
- If you are not sure about your fix or feature it's better to open an issue first and discuss the details there.
- Maybe your fix or update won't be perfect at first. Don't be afraid, just improve it and push the new commits. The PR will be updated accordingly. 
- If your update needs some extra work it's okay to say: _"I'm busy now and I will improve it soon"_ or _"Sorry, I don't have time to improve it, I hope it helps in this form too"_. 
So it's better to say don't have time to continue than saying nothing.
- Please read and follow this [guide about the coding style](https://github.com/littlevgl/lvgl/blob/master/docs/CODING_STYLE.md)


## Where is the documentation?

You can read the documentation here: <https://docs.littlevgl.com/>  
You can edit the documentation here: <https://github.com/littlevgl/doc>  

## Where is the blog?

You can read the blog here: <https://blog.littlevgl.com/>  
You can edit the blog here: <https://github.com/littlevgl/blog>  

# So how and where can you contribute?

## Help others in the Forum

It's a great way to contribute to the library if you already use it. 
Just go to [https://forum.littlevgl.com/](https://forum.littlevgl.com/) a register (Google and GitHub login also works).
Log in, read the titles and if you are already familiar with a topic, don't be shy, and write your suggestion.

## Improving and/or translating the documentation

If you would like to contribute to LittlevGL the documentation is the best place to start.

### Fix typos, add missing parts

If you find a typo, an obscure sentence or something which is not explained well enough in the [English documentation](https://docs.littlevgl.com/en/html/index.html) 
click the *"Edit on GitHub"* button in the top right corner and fix the issue by sending a Pull Request.

### Translate the documentation

If you have time and interest you can translate the documentation to your native language or any language you speak. 
You can join others to work on an already existing language or you can start a new one.  

To translate the documentation we use [Zanata](https://zanata.org) which is an online translation platform. 
You will find the LittlevGL project here: [LittlevGL on Zanata](https://translate.zanata.org/iteration/view/littlevgl-docs/v6.0-doc1?dswid=3430) 

To get started you need to:
- register at [Zanata](https://zanata.org) which is an online translation platform.  
- comment to [this post](https://forum.littlevgl.com/t/translate-the-documentation/238?u=kisvegabor)
- tell your username at *Zanata* and your selected language(s) to get permission the edit the translations

Note that a translation will be added to the documentation only if at least the [Porting section](https://docs.littlevgl.com/en/html/porting/index.html) is translated.


## Writing a blog post about your experiences

Have you ported LittlevGL to a new platform? Have you created a fancy GUI? Do you know a great trick? 
You can share your knowledge on LittlevGL's blog! It's super easy to add your own post:
- Fork and clone the [blog repository](https://github.com/littlevgl/blog)
- Add your post in Markdown to the `_posts` folder. 
- Store the images and other resources in a dedicated folder in `assets`
- Create a Pull Request

The blog uses [Jekyll](https://jekyllrb.com/) to convert the `.md` files to a webpage. You can easily [run Jekyll offline](https://jekyllrb.com/docs/) to check your post before creating the Pull request

## Reporting and/or fixing bugs
For simple bugfixes (typos, missing error handling, fixing a warning) is fine to send a Pull request directly. However, for more complex bugs it's better to open an issue first. In the issue, you should describe how to reproduce the bug and even add the minimal code snippet.

## Suggesting and/or implementing new features
If you have a good idea don't hesitate to share with us. It's even better if you have time to deal with its implementation. Don't be afraid if you still don't know LittlevGL well enough. We will help you to get started. 

During the implementation don't forget the [Code style guide](https://github.com/littlevgl/lvgl/blob/master/docs/CODING_STYLE.md).

# Summary

I hope you have taken a liking to contribute to LittlevGL. A helpful and friendly community is waiting for you! :) 
