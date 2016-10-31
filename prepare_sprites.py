from __future__ import print_function

from PIL import Image
import os
import json

filelist = []
output = {}
for (dirpath, dirnames, filenames) in os.walk("sprites"):
    if dirnames == []:
        output[dirpath[8:]+"/ number of frames"] = len(filenames)
    for filename in filenames:
        if filename.endswith('.png'):
            filelist.append(os.sep.join([dirpath, filename]))

for filepath in filelist:
    if not "/ui/" in filepath:
        orig_im = Image.open(filepath)
        orig_im.convert("RGBA")
        w, h = orig_im.size
        scaled_im = Image.new("RGBA", (w+2, h+2))
        scaled_im.paste(orig_im, (1, 1))

        scaled_pixels = scaled_im.load()
        im = Image.new("RGBA", (w+2, h+2))
        pixels = im.load()

        for x in range(0, w+2):
            for y in range(0, h+2):
                # If pixel is transparent
                if scaled_pixels[x, y][3] == 0:
                    isborder = False
                    for i, j in ([-1, 0], [1, 0], [0, -1], [0, 1]):
                        if scaled_pixels[max(0, min(w+1, x+i)), max(0, min(h+1, y+j))][3] != 0:
                            isborder = True
                            break
                    if isborder:
                        # Make white
                        pixels[x, y] = (255, 255, 255)
                else:
                    pixels[x, y] = (0, 0, 0, 0)

        im.save(filepath.replace(".", "_outline."))


filelist = []
for (dirpath, dirnames, filenames) in os.walk("sprites"):
    for filename in filenames:
        if filename.endswith('.png'):
            filelist.append(os.sep.join([dirpath, filename]))


for filepath in filelist:
    im = Image.open(filepath)
    im.convert("RGBA")
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

    output[filepath[8:]] = [w/2-l, h/2-t]
    im2 = im.crop((l, t, r+1, b+1))
    im2.save(filepath)

with open("sprites/spritedata.json", "w") as f:
    json.dump(output, f)
