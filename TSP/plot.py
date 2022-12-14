from PIL import Image, ImageDraw
import sys
import time
import cv2
import numpy

points = []
edges = []

def remap(x, max_x, img_dim):
    ratio = x / max_x
    return int(ratio * img_dim)

def redraw(show=True, save=False):
    img = Image.new("RGB", (900, 900), (0, 0, 0))
    if len(points) < 1:
        return
    max_x = max([point[0] for point in points])
    max_y = max([point[1] for point in points])

    draw = ImageDraw.Draw(img)
    for i, (x, y) in enumerate(points):
        x, y = remap(x, max_x, 800), remap(y, max_y, 800)
        draw.ellipse(((x - 6), (y - 6), (x + 6), (y + 6)))
        draw.text(((x - 2), (y - 5)), str(i))

    if len(edges) < 1:
        return

    for edge in edges:
        x1, y1, x2, y2 = edge[0][0], edge[0][1], edge[1][0], edge[1][1]
        x1, y1, x2, y2 = (
            remap(x1, max_x, 800),
            remap(y1, max_y, 800),
            remap(x2, max_x, 800),
            remap(y2, max_y, 800),
        )
        draw.ellipse(((x1 - 2), (y1 - 2), (x1 + 2), (y1 + 2)), fill="green")
        draw.ellipse(((x2 - 2), (y2 - 2), (x2 + 2), (y2 + 2)), fill="green")
        draw.line((x1, y1, x2, y2), fill="green")

    open_cv_image = cv2.cvtColor(numpy.array(img), cv2.COLOR_RGB2BGR)
    if(save):
        cv2.imwrite('last.png', open_cv_image)
    if(show):
        cv2.imshow("image", open_cv_image)
        cv2.waitKey(1)


redraw(show="--show" in sys.argv)
time.sleep(2)

for line in sys.stdin.readlines():
    if line.startswith("IP"):
        tokens = line.split(" ")
        x, y = int(tokens[1]), int(tokens[2])
        points.append((x, y))
    elif line.startswith("PATH"):
        edges.clear()
        tokens = line.split(" ")
        for i in range( 1, len(tokens)-1 ):
            p1 = points[int(tokens[i])]
            p2 = points[int(tokens[i+1])]
            edges.append((p1, p2))
    elif line.startswith("PAUSE"):
        if "--nopause" in sys.argv:
            continue
        print("pause")
        time.sleep(0.5)

    if "--show" in sys.argv:
        redraw(show=True, save=False)

redraw(show="--show" in sys.argv, save=True)
print("DONE")

if("--show" in sys.argv):
    time.sleep(100)
