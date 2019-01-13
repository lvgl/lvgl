# Contributing to Littlev Graphics Library

**Welcome!  It's glad to see that you are interested in contributing to LittlevGL! There are several types of task where you can help to build a better library! Let's see how to get started!**


There are many different possibilities to join the community. If you have some time to work with us I'm sure you will find something that fits you! You can:
- answer other's questions
- report and/or fix bugs
- suggest and/or implement new features
- improve and/or translate the documentation
- write a blog post about your experiences

But first, start with the most Frequently Asked Questions.

## FAQ about contributing

### Where can I write my question and remarks?

We use [GitHub's issue tracker](https://github.com/littlevgl/lvgl/issues) to ask questions., report bugs and suggest features. But there are some simple rules:
- Be kind and friendly.
- Speak about one thing in one issue.
- Give feedback and close the issue if your question is answered. 
- Tell what you experience or expect. _"The button is not working"_ is not enough info to get help.
- If possible send an absolute minimal code example in order to reproduce the issue
- Use [Markdown](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet) to format your post.
- If you don't get any answer in a week write a comment like "Can somebody help?". Maybe your issue wasn't noticed.

### How can I send fixes and improvements?
Merging new code happens via Pull Requests. If you are still not familiar with the Pull Requests (PR for short) here is a quick guide about them:
1. **Fork** the [lvgl repository](https://github.com/littlevgl/lvgl). To do this click the "Fork" button in the top right corner. It will "copy" the `lvgl` repository to your GitHub account (`https://github.com/your_name?tab=repositories`)
2. **Clone**  the forked repository and add your updates
3. **Create a PR** on the GitHub on the page of you `lvgl` repository(`https://github.com/your_name/lvgl`) by hitting the "New pull request" button 
4. **Set the base branch**. It means where you want to merge your update. Fixes go to `master`, new features to the actual `dev-x.y` branch. 
5. **Describe** what is in the update. An example code is welcome if applicable.

Some advice:
- If you are not sure about your fix or feature it's better to open an issue first, and discuss the details there.
- Maybe your fix or update won't be perfect at first. Don't be afraid, just improve it and push the new commits. The PR will be updated accordingly. 
- If your update needs some extra work it's okay to say: _"I'm busy now and I will improve it soon"_ or _"Sorry, I don't have time to improve it, I hope it helps in this form too"_. So it's better to say don't have time to continue then saying nothing.
- Please read and follow this [guide about the coding style](https://docs.littlevgl.com/#Coding-Style-Guide)


### Where is the documentation?

You can read the documentation here: https://docs.littlevgl.com/  
You can edit the documentation here: https://github.com/littlevgl/doc  

### Where is the blog?

You can read the blog here: https://blog.littlevgl.com/  
You can edit the blog here: https://github.com/littlevgl/blog  


## So how and where can I contribute?

### Answering other's questions

It's a great way to contribute to the library if you already use it. Just go the [issue tracker](https://github.com/littlevgl/lvgl/issues), read the titles and if you are already familiar with a topic, don't be shy, and write your suggestion.

### Reporting and/or fixing bugs
For simple bugfixes (typos, missing error handling, fixing a warning) is fine to send a Pull request directly. However, for more complex bugs it's better to open an issue first. In the issue, you should describe how to reproduce the bug and even add the minimal code snippet.

### Suggesting and/or implementing new features
If you have a good idea don't hesitate to share with us. It's even better if you have time to deal with its implementation. Don't be afraid if you still don't know LittlevGL well enough. We will help you to get started. 

During the implementation don't forget the [Code style guide](https://docs.littlevgl.com/#Coding-Style-Guide).

### Improving and/or translating the documentation

The documentation of LittlevGL is written in Markdown and available [here](https://github.com/littlevgl/doc) for editing. If you find some parts of the documentation obscure or insufficient just search the related `.md` file, hit the edit icon and add your updates. This way a  new Pull request will be generated automatically.

If you can devote more time to improve the documentation you can translate it! 
1. Just copy the English `.md` files from the root folder to `locale/LANGUAGE_CODE` (language code is e.g. DE, FR, ES etc)
2. Append the language code the end of files (e.g. Welcome_fr.md)
3. Update the filenames in `_Sidebar.md`
4. Translate the page(s) you want
5. Create a Pull request

### Writing a blog post about your experiences

Have ported LittlevGL to a new platform? Have you created a fancy GUI? Do you know a great trick? 
You can share your knowledge on LittelvGL's blog! It's super easy to add your own post:
- Fork and clone the [blog repository](https://github.com/littlevgl/blog)
- Add your post in Markdown to the `_posts` folder. 
- Store the images and other resources in a dedicated folder in `assets`
- Create a Pull Request

The blog uses [Jekyll](https://jekyllrb.com/) to convert the `.md` files to a webpage. You can easily [run Jekyll offline](https://jekyllrb.com/docs/) to check your post before creating the Pull request

## Summary

I hope you have taken a liking to contribute to LittelvGL. A helpful and friendly community is waiting for you! :) 

