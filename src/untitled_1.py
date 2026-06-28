import sensor, time, math, pyb
from pyb import UART
def pix_to_real(r_px):
    return (-0.19292354634084388
            + 0.9854734061269065*r_px
            - 0.000013340676293767154*(r_px**2))
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_gainceiling(32)
sensor.set_auto_whitebal(False)
sensor.set_auto_exposure(False, exposure_us=8000)
sensor.set_auto_gain(False, gain_db=4)
sensor.skip_frames(time=200)
clock = time.clock()
# led2 = pyb.LED(2)
# led2.on()
window_x = 196
window_y = 125
window_width = 280
mask_radius = 100
roi_width=280
# sensor.set_windowing(window_x - int((window_width/2)), window_y - int((window_width/2)), window_width, window_width)
centre_x = 158
centre_y = 128
thresh_ball = (43, 76, 27, 53, 18, 57)
thresh_yellow_goal = (41, 50, 1, 14, 26, 50)
thresh_blue_goal = (18, 33, 19, -3, -128, -24)
uart = UART(3, 115200)
uart.init(115200, bits=8, parity=None, stop=1, timeout_char=1000)
count = 0
no_ball = False
target = None

img = sensor.snapshot()

if target is None:
    candidates = []
    while len(candidates) < 1:
        img = sensor.snapshot()
        blobs = img.find_blobs(
            [thresh_blue_goal, thresh_yellow_goal],
            merge=True)
        candidates = [b for b in blobs if b.cx() < centre_x]

    if candidates:
        largest = max(candidates, key=lambda b: b.pixels())

        if largest.code() == 1:
            target = 'blue'
        elif largest.code() == 2:
            target = 'yellow'

while True:
    clock.tick()
    # led2.on()
    img = sensor.snapshot()
    print(target)


    if target == 'blue':
        goal = img.find_blobs([thresh_blue_goal], merge=True)
        led3 = pyb.LED(3)
        led3.on()
    elif target == 'yellow':
        goal = img.find_blobs([thresh_yellow_goal], merge=True)
        led1 = pyb.LED(1)
        led1.on()
    else:
        goal = []

    img.draw_cross(centre_x, centre_y)
    # goal = img.find_blobs([thresh_yellow_goal], merge=True)
    if len(goal)>0:
        no_goal = False
        b = max(goal, key = lambda b:b.pixels())
        img.draw_rectangle(b.rect())
        img.draw_cross(b.cx(), b.cy())
        goal_x = b.cx() - centre_x
        goal_y = b.cy() - centre_y
        # goal_angle = math.atan2(goal_x, goal_y) * 180 / math.pi
        goal_angle = math.atan2(goal_y, 90) * 180 / math.pi #90 is tuned somehow
        # goal_angle += 90
        if b.cx() > centre_x:
            goal_angle = 180 - goal_angle
        if goal_angle<0:
            goal_angle += 360
        if goal_angle>360:
            goal_angle -= 360
        goal_dist = (goal_x ** 2 + goal_y ** 2) ** 0.5
        print("angle ", goal_angle, "dist ", goal_dist)
        actual_dist = pix_to_real(goal_dist) +2
        # print("actual dist: ", actual_dist)
        # print(goal_y)

        # angle_uart = round(goal_angle * 128)
        # dist_uart = round(actual_dist * 128)
        angle_uart = round(goal_angle / 360 * 255)
        uart.writechar(angle_uart)
    else:
        no_goal = True
        uart.writechar(0)

    uart.sendbreak()
    # print("fps", clock.fps())
