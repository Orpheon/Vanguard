from PIL import Image

import sys

orig_im = Image.open(sys.argv[1])
orig_im.convert("RGBA")
w, h = orig_im.size
scaled_im = Image.new("RGBA", (w+2, h+1))
scaled_im.paste(orig_im, (1, 1))

scaled_pixels = scaled_im.load()
im = Image.new("RGBA", (w+2, h+1))
pixels = im.load()

for x in range(0, w+2):
    for y in range(0, h+1):
        # If pixel is transparent
        if scaled_pixels[x, y][3] == 0:
            isborder = False
            for i, j in ([-1, 0], [1, 0], [0, -1], [0, 1]):
                if scaled_pixels[max(0, min(w+1, x+i)), max(0, min(h, y+j))][3] != 0:
                    isborder = True
                    break
            if isborder:
                # Make red
                pixels[x, y] = (225, 17, 17)
        else:
            pixels[x, y] = scaled_pixels[x, y]

im.save(sys.argv[1].replace(".", "_enemy."))
