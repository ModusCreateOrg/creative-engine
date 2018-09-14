# ModusCreateOrg GitHub Guidelines
This repository serves as a collection of useful scripts, configuration files, and settings for GitHub repositories.

> Modus Create team members should refer to [ModusCreateOrg GitHub Guidelines](https://docs.google.com/document/d/1eBFta4gP3-eZ4Gcpx0ww9SHAH6GrOoPSLmTFZ7R8foo/edit#heading=h.sjyqpqnsjmjl)

[![Modus Create](./images/modus.logo.svg)](https://moduscreate.com)

## Repository set up for development

1) FORK this repository using the fork button at GitHub.
2) clone your fork - do not clone this repository!
3) cd to your working directory
4) ```$ git remote add upstream git@github.com:ModusCreateOrg/creative-engine```
5) ```$ git fetch upstream```

To keep your master branch up to date with this repo:
```
$ git checkout master
$ git merge upstream/master
```

When you work on an issue/ticket:
```
$ git checkott master
$ git merge upstream/master # up to date!
$ git checkout -b branch-name  # branchname is ticket-number hyphen description
```

When you commit and push to your branch, you will have the opportunity to create a pull 
request from your fork against the official/upstream repo.  

When you create a pull request, put a link to the GitHub issue in the first post, along 
with a description of what the PR does.

