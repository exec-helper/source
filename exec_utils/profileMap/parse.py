import json
import os

def parseProfileMap(profileMapFile):
    with open(profileMapFile) as jsonFile:    
        profileMapData = json.load(jsonFile)
    return profileMapData
