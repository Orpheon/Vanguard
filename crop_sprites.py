from PIL import Image

import sys

# run with: python crop_sprites.py `find sprites/ -name *.png -print`
# (on linux, obviously)

if sys.argv[1] == "help":
    print "run with: python crop_sprites.py `find sprites/ -name *.png -print`"
else:
    for filename in sys.argv[1:]:
        im = Image.open(filename)
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

        print('"{0}": [{1}, {2}],'.format(filename[8:], w/2-l, h/2-t))
        im2 = im.crop((l, t, r+1, b+1))
        im2.save(filename)
