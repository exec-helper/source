import json
import os

class Profile:
    def __init__(self, id, suffix, targetDirectory):
        self.id = id
        self.suffix = suffix
        self.targetDirectory = targetDirectory

def parseProfileMap(profileMapFile):
    with open(profileMapFile) as jsonFile:    
        profileMapData = json.load(jsonFile)
    return profileMapData