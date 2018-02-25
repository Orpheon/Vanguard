from PIL import Image, ImageDraw
import json
import os
import sys
import math
import shutil

class Map:
    ENTITY_SIZE = 42

    def __init__(self, name):
        if os.path.exists(name):
            self.loadmap(name)
        else:
            print("Error: Could not find", name)

    def loadmap(self, name):
        background = Image.open(name)
        if not "Gang Garrison 2 Level Data" in background.text:
            print("Error: No level data found in {}.".format(name))
            return

        leveldata = background.text["Gang Garrison 2 Level Data"]
        wallmask = self.loadwallmask(leveldata)
        background = background.resize((background.width*6, background.height*6))
        wallmask = wallmask.resize((wallmask.width*6, wallmask.height*6))
        entities = self.loadentities(leveldata)
        if "spawnroom" not in entities:
            print("Error: {} doesn't have any spawnroom entities or could not be parsed.".format(name))
            return

        spawnrects = self.clusterstackedentities(entities["spawnroom"])
        redspawns, bluespawns = self.associateteamswspawns(spawnrects, entities["redspawn"], entities["bluespawn"])
        if len(redspawns) == 0:
            print("Error: {} does not have any red spawns".format(name))
        if len(bluespawns) == 0:
            print("Error: {} does not have any blue spawns".format(name))

        if "CapturePoint" not in entities:
            print("Error: {} does not have capture point entities.".format(name))
            return

        capturepointrects = self.clusterstackedentities(entities["CapturePoint"])
        if len(capturepointrects) > 1:
            print("More than one capture point rect exists on", name)
            print("Merging them all together, this might result in an oversized control point")
            x1 = min([x[0] for x in capturepointrects])
            y1 = min([x[1] for x in capturepointrects])
            x2 = max([x[2] for x in capturepointrects])
            y2 = max([x[3] for x in capturepointrects])
            capturepointrects = [(x1, y1, x2, y2)]

        background = background.convert("RGBA")
        emptyim = Image.new("RGBA", (background.width, background.height), color=(0, 0, 0, 0))
        if background.width != wallmask.width or background.height != wallmask.height:
            print("Error: Background of {} is not the same size as the wallmask.".format(name))
            return
        
        emptyim.paste(background, wallmask)

        spawns1 = []
        spawns2 = []
        for (x1, y1, x2, y2) in redspawns:
            spawns1.append((x1, y1, x2-x1, y2-y1))
        for (x1, y1, x2, y2) in bluespawns:
            spawns2.append((x1, y1, x2-x1, y2-y1))
        cprect = [capturepointrects[0][0], capturepointrects[0][1], capturepointrects[0][2]-capturepointrects[0][0],
                  capturepointrects[0][3]-capturepointrects[0][1]]
        self.export(name, background, emptyim, spawns1, spawns2, cprect)

    def loadwallmask(self, leveldata):
        wmtag = "{WALKMASK}"
        endwmtag = "{END WALKMASK}"
        data = leveldata[leveldata.find(wmtag) : leveldata.find(endwmtag)]
        data = data[len(wmtag)+1:]
        width = int(data[:data.find("\n")])
        data = data[data.find("\n")+1:]
        height = int(data[:data.find("\n")])
        data = data[data.find("\n")+1:]

        wm = Image.new("RGBA", (width, height), color=(0, 0, 0, 0))
        bitmask = 2**(len(data) * 6 - width*height)
        value = ord(data[-1]) - 32
        for y in range(height-1, -1, -1):
            for x in range(width-1, -1, -1):
                if bitmask >= 64:
                    bitmask = 1
                    data = data[:-1]
                    value = ord(data[-1]) - 32
                if value & bitmask:
                    wm.putpixel((x, y), (0, 0, 0, 255))
                bitmask *= 2

        return wm

    def loadentities(self, leveldata):
        entitytag = "{ENTITIES}"
        endentitytag = "{END ENTITIES}"
        entities = {}
        data = leveldata[leveldata.find(entitytag) : leveldata.find(endentitytag)]
        data = data[len(entitytag)+1:]
        if "{" in data:
            if data[0] == "{" or data[0] == "[":
                data = data[1:-1]
            if "layer" in data:
                # Forget it, no chance of parsing embedded fucking images
                return entities
            substrings = []
            while "{" in data:
                startindex = data.index("{")
                endindex = data.index("}")
                substrings.append(data[startindex+1:endindex])
                data = data[endindex+1:]
            subdata = []
            for substring in substrings:
                subdata.append(substring.split(","))
            for entity in subdata:
                entitydata = {}
                for item in entity:
                    itemtype = item[:item.index(":")]
                    itemvalue = item[item.index(":")+1:]
                    try:
                        entitydata[itemtype] = float(itemvalue)
                    except ValueError:
                        entitydata[itemtype] = itemvalue
                if "x" in entitydata:
                    type = entitydata["type"]
                    if type in entities:
                        entities[type].append((entitydata["x"], entitydata["y"]))
                    else:
                        entities[type] = [(entitydata["x"], entitydata["y"])]
        else:
            while len(data) > 0:
                type = data[:data.find("\n")]
                data = data[len(type)+1:]
                x = data[:data.find("\n")]
                data = data[len(x)+1:]
                y = data[:data.find("\n")]
                data = data[len(y)+1:]
                x = int(x)
                y = int(y)
                if type in entities:
                    entities[type].append((x, y))
                else:
                    entities[type] = [(x, y)]

        return entities

    def clusterstackedentities(self, entities):
        maxdist = math.hypot(self.ENTITY_SIZE, self.ENTITY_SIZE)
        groups = []
        entities = entities[:]
        while len(entities) > 0:
            foundcluster = False
            spawnx = entities[0][0]
            spawny = entities[0][1]
            for group in groups:
                for (x, y) in group:
                    if math.hypot(x-spawnx, y-spawny) <= maxdist:
                        foundcluster = True
                        group.append(entities.pop(0))
                        break
                if foundcluster:
                    break
            if not foundcluster:
                groups.append([entities.pop(0)])

        # Merge groups that touch
        mergegroups = True
        while mergegroups:
            mergegroups = False
            for mainindex,maingroup in enumerate(groups):
                remaininggroups = groups[mainindex+1:]
                for otherindex,othergroup in enumerate(remaininggroups):
                    for (x1, y1) in maingroup:
                        for (x2, y2) in othergroup:
                            if math.hypot(x1-x2, y1-y2) <= maxdist:
                                mergegroups = True
                                for entity in othergroup:
                                    maingroup.append(entity)
                                groups.pop(otherindex+mainindex+1)
                                break
                        if mergegroups: break
                    if mergegroups: break
                if mergegroups: break

        entityrects = []
        for group in groups:
            x1 = 2**32
            y1 = 2**32
            x2 = 0
            y2 = 0
            for (x, y) in group:
                if x < x1:
                    x1 = x
                if y < y1:
                    y1 = y
                if x+self.ENTITY_SIZE > x2:
                    x2 = x+self.ENTITY_SIZE
                if y+self.ENTITY_SIZE > y2:
                    y2 = y+self.ENTITY_SIZE
            entityrects.append((x1, y1, x2, y2))
        return entityrects

    def associateteamswspawns(self, spawnrects, redspawns, bluespawns):
        reds = []
        blues = []
        for (x1, y1, x2, y2) in spawnrects:
            isred = False
            for spawn in redspawns:
                if x1 <= spawn[0] and spawn[0] <= x2:
                    if y1 <= spawn[1] and spawn[1] <= y2:
                        isred = True
            isblue = False
            for spawn in bluespawns:
                if x1 <= spawn[0] and spawn[0] <= x2:
                    if y1 <= spawn[1] and spawn[1] <= y2:
                        isblue = True

            if isred and isblue:
                pass
            elif isred:
                reds.append((x1, y1, x2, y2))
            elif isblue:
                blues.append((x1, y1, x2, y2))
        return reds, blues

    def export(self, name, background, wallmask, spawns1, spawns2, capturezone):
        filename = os.path.split(name)[-1]
        filename = filename.replace(".png", "")
        dirpath = os.path.join("outputs", filename)
        if os.path.exists(dirpath):
            shutil.rmtree(dirpath)
        os.mkdir(dirpath)

        background.save(os.path.join(dirpath, "background.png"))
        wallmask.save(os.path.join(dirpath, "wallmask.png"))
        mapdata = {
            "background": "background.png",
            "wallmask": "wallmask.png",
            "wallmask foreground": "wallmask.png",
            "gamemodes": [
                {
                    "type": "KOTH",
                    "spawn 1": spawns1,
                    "spawn 2": spawns2,
                    "cp": capturezone
                }
            ]
        }
        with open(os.path.join(dirpath, "mapdata.json"), "w") as f:
            json.dump(mapdata, f, indent=4)


for (dirpath, dirnames, filenames) in os.walk("maps"):
    for filename in filenames:
        Map(os.path.join(dirpath, filename))