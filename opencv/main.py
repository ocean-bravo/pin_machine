import numpy as np
import argparse
import cv2

def on_slider(val):
    r = cv2.getTrackbarPos("radius", "main")
    p1 = cv2.getTrackbarPos("param1", "main")
    p2 = cv2.getTrackbarPos("param2", "main") / 100.0
#    p2 = cv2.getTrackbarPos("param2", "main")
    md = cv2.getTrackbarPos("minDist", "main")
    findCircles(p1, p2, r, md)

def findCircles(p1, p2, r, md):
    image = cv2.imread("/home/mint/devel/opencv/img.jpg")
    output = image.copy()
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    print("find ", p1 , p2, r)
    circles = cv2.HoughCircles(gray, cv2.HOUGH_GRADIENT_ALT, 1.2, md, param1=p1,param2=p2, minRadius=r, maxRadius=(r+10))
    drawCircles(image, output, circles)
    print("finish")

def drawCircles(image, output, circles):
    # ensure at least some circles were found
    if circles is not None:
        # convert the (x, y) coordinates and radius of the circles to integers
        circles = np.round(circles[0, :]).astype("int")

        # loop over the (x, y) coordinates and radius of the circles
        for (x, y, r) in circles:
            # draw the circle in the output image, then draw a rectangle
            # corresponding to the center of the circle
            cv2.circle(output, (x, y), r, (255, 0, 0), 4)
            cv2.rectangle(output, (x - 5, y - 5), (x + 5, y + 5), (0, 128, 255), -1)
            #print(r)
        # show the output image
        out = np.hstack([image, output])

        resized = cv2.resize(out, (1600,700), interpolation = cv2.INTER_AREA)

        cv2.imshow("main", resized)
    else:
        out = np.hstack([image, output])
        resized = cv2.resize(out, (1600,700), interpolation = cv2.INTER_AREA)
        cv2.imshow("main", resized)
    print("draw")



# construct the argument parser and parse the arguments
#ap = argparse.ArgumentParser()
#ap.add_argument("-i", "--image", required = True, help = "Path to the image")
#args = vars(ap.parse_args())

# load the image, clone it for output, and then convert it to grayscale
#image = cv2.imread(args["image"])

cv2.namedWindow("main")

cv2.createTrackbar("radius", "main" , 80, 200, on_slider)
cv2.createTrackbar("param1", "main" , 168, 200, on_slider)
#cv2.createTrackbar("param2", "main" , 29, 200, on_slider) #HOUGH_GRADIENT
cv2.createTrackbar("param2", "main" , 50, 100, on_slider) # HOUGH_GRADIENT_ALT
cv2.createTrackbar("minDist", "main" , 70, 400, on_slider)

on_slider(100)
cv2.waitKey(0)


