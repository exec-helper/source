#!/usr/bin/python

from ..util.util import executeInShell, isSuccess

class Git:
    def __init__(self):
        pass

    def init(self):
        return isSuccess(executeInShell(["git","submodule", "init"])) and isSuccess(executeInShell(["git","submodule", "update"]))
