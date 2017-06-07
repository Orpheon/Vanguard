from __future__ import print_function

from PIL import Image
import os
import json
import shutil

# Delete the old sprites folder if it exists
if os.path.exists("sprites"):
    shutil.rmtree("sprites")

filelist = []
spritedata = {}
# Get a list of all the sprites we're handling
for (dirpath, dirnames, filenames) in os.walk("sprites template"):
    nframes = 0
    for filename in filenames:
        if filename.endswith('.png'):
            filelist.append(os.sep.join([dirpath, filename]))
            nframes += 1
    spritedata[dirpath.partition(os.path.sep)[2]+"/ number of frames"] = nframes


for filepath in filelist:
    # Crop sprite to remove whitespace around and remember how much we cropped it by for rendering offset
    im = Image.open(filepath)
    im = im.convert(mode="RGBA")
    w, h = im.size
    r = 0
    l = w
    t = h
    b = 0

    pix = im.load()
    for x in range(w):
        for y in range(h):
            if pix[x, y][3] != 0:
                if l > x:
                    l = x
                if r < x:
                    r = x
                if t > y:
                    t = y
                if b < y:
                    b = y

    spritedata[filepath.partition(os.path.sep)[2]] = [w/2-l, h/2-t]
    im2 = im.crop((l, t, r+1, b+1))

    outputfile = filepath.replace("sprites template", "sprites")
    if not os.path.exists(os.path.dirname(outputfile)):
        os.makedirs(os.path.dirname(outputfile))
    im2.save(outputfile)

    # Generate outline file if we're not in the ui folder and it's an actual sprite
    if os.path.sep+"ui"+os.path.sep not in filepath and "_sprite.png" in filepath:
        # Outlines are preferably generated from hitmask sprites if one exists
        if os.path.exists(filepath.replace("_sprite.png", "_hitmask.png")):
            im = Image.open(filepath.replace("_sprite.png", "_hitmask.png"))
        else:
            im = Image.open(filepath)

        im = im.convert(mode="RGBA")
        w, h = im.size
        outline = Image.new("RGBA", im.size)
        inputpix = im.load()
        outlinepix = outline.load()
        for x in range(w):
            for y in range(h):
                if inputpix[x, y][3] != 0:
                    try:
                        for i,j in ([-1, 0], [1, 0], [0, -1], [0, 1]):
                            if inputpix[x+i, y+j][3] == 0:
                                outlinepix[x, y] = (255, 255, 255, 177)
                    except IndexError:
                        # We're at the edge of the sprite image, that also counts as an edge of the sprite
                        outlinepix[x, y] = (255, 255, 255, 177)

        outline.crop((l, t, r+1, b+1)).save(filepath.replace("_sprite.png", "_outline.png").replace("sprites template", "sprites"))


# Export the cropping data and the animation lengths
with open("sprites/spritedata.json", "w") as f:
    json.dump(spritedata, f, sort_keys=True, indent=4)
