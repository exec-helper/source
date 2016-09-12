#!/usr/bin/python

from .git import Git

vcsSystem = Git()

def initVcs():
    print("Initializing git submodules...")
    return vcsSystem.init()
