from __future__ import print_function

from PIL import Image
import os
import json

filelist = []
output = {}
for (dirpath, dirnames, filenames) in os.walk("sprites template"):
    if filenames != []:
        output[dirpath.partition(os.path.sep)[2]+"/ number of frames"] = len(filenames)
    for filename in filenames:
        if filename.endswith('.png'):
            filelist.append(os.sep.join([dirpath, filename]))



for filepath in filelist:
    im = Image.open(filepath)
    im.convert(mode="RGBA")
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

    output[filepath.partition(os.path.sep)[2]] = [w/2-l, h/2-t]
    im2 = im.crop((l, t, r+1, b+1))

    outputfile = filepath.replace("sprites template", "sprites")
    if not os.path.exists(os.path.dirname(outputfile)):
        os.makedirs(os.path.dirname(outputfile))
    im2.save(outputfile)

with open("sprites/spritedata.json", "w") as f:
    json.dump(output, f)
