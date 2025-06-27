from PIL import Image
import numpy as np

def tempConversion(path):
    img = Image.open(path).convert("RGB")
    pixels = img.load()
    w, h = img.size

    tempImage = np.empty(shape=(h,w)).astype(np.uint8)
    tempImage.fill(255)

    for y in range(h):
        for x in range(w):
            r, g, b = pixels[x, y]
            if (r + g + b)/3 > 100:
                continue
            tempImage[y][x] = 0

    img = Image.fromarray(tempImage, mode='L')
    img.save("trackTemp.png")

def producePixel(path, start, direction):
    img = Image.open(path).convert("RGB")
    pixels = img.load()
    w, h = img.size

    tempImage = np.empty(shape=(h,w)).astype(np.uint8)
    tempImage.fill(255)

    coords = []

    for y in range(h):
        for x in range(w):
            r, g, b = pixels[x, y]
            if (r + g + b)/3 > 100:
                continue
            tempImage[y][x] = 0

    startX, startY = start

    if tempImage[startY][startX] != 0:
        return coords
    else:
        coords.append(start)
        tempImage[startY][startX] = 255

    coord = (startX, startY)
    while True:
        s = scan(tempImage, coord, 'u')
        if s == False:
            break
        tempImage[coord[1]][coord[0]] = 255
        coord = s
        coords.append(coord)      
    return coords

def scan(currentState, coords, dir):
    x, y = coords
    if dir == 'u':
        if(currentState[y-1][x] == 0):
            return (x, y-1)
        if(currentState[y][x+1] == 0):
            return (x+1, y)
        if(currentState[y+1][x] == 0):
            return (x, y+1)
        if(currentState[y][x-1] == 0):
            return (x-1, y)
    if dir == 'r':
        if(currentState[y][x+1] == 0):
            return (x+1, y)
        if(currentState[y+1][x] == 0):
            return (x, y+1)
        if(currentState[y][x-1] == 0):
            return (x-1, y)
        if(currentState[y-1][x] == 0):
            return (x, y-1)
    if dir == 'd':
        if(currentState[y+1][x] == 0):
            return (x, y+1)
        if(currentState[y][x-1] == 0):
            return (x-1, y)
        if(currentState[y-1][x] == 0):
            return (x, y-1)
        if(currentState[y][x+1] == 0):
            return (x+1, y)
    if dir == 'l':
        if(currentState[y][x-1] == 0):
            return (x-1, y)   
        if(currentState[y-1][x] == 0):
            return (x, y-1)
        if(currentState[y][x+1] == 0):
            return (x+1, y)
        if(currentState[y+1][x] == 0):
            return (x, y+1)
    return False

def checkList(lst):
    tempImage = np.empty(shape=(280,240)).astype(np.uint8)
    tempImage.fill(255)
    for x,y in lst:
        tempImage[y][x] = 0
    
    img = Image.fromarray(tempImage, mode='L')
    img.save("trackTempCheck.png")
    
tempConversion("austria.png")
l = producePixel("trackTemp.png", (132, 223), "u")
print(l)
checkList(l)

xFin = [x[0] for x in l]
yFin = [y[1] for y in l]

print(len(l))
print(xFin)
print(yFin)