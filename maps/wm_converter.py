from PIL import Image

mapname = "conflict_wm"

image = Image.open(mapname+".png")
data = [bin(x==(0, 0, 0, 255))[2:] for x in list(image.getdata())]
datastr = "".join(data)

with open(mapname+".wallmask", "wb") as f:
    f.write(datastr)
