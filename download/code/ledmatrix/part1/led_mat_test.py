__author__ = 'Just_CJ'

import time
import thread
import RPi.GPIO

# gpio_down = [16, 20, 19, 26, 13, 6, 5, 11]
# gpio_up = [9, 10, 22, 27, 17, 4, 3, 2]

gpio_col = [27, 19, 26, 23, 6, 4, 10, 9]
gpio_row = [24, 22, 11, 17, 16, 5, 20, 13]

mat = 8*[8*[0]]

def init_gpio():
    RPi.GPIO.setmode(RPi.GPIO.BCM)
    RPi.GPIO.setwarnings(False)
    for i in gpio_col:
        RPi.GPIO.setup(i,RPi.GPIO.OUT) # sets GPIO to output

    for i in gpio_row:
        RPi.GPIO.setup(i,RPi.GPIO.OUT) # sets GPIO to output

def set_row(values):
    for i in range(len(gpio_row)):
        RPi.GPIO.output(gpio_row[i], values[i]) # set value

def set_col(values):
    for i in range(len(gpio_col)):
        RPi.GPIO.output(gpio_col[i], 1-values[i]) # set value

def get_font_mat(ascii):
    f = open('font', 'r')
    f.seek(ascii*8*10)
    res = []
    for i in range(8):
        res.append(map(int, list(f.readline().replace('\r\n', ''))))
    f.read()
    f.close()
    return res

def display():
    init_gpio()

    while True:
        row = [1,0,0,0,0,0,0,0] # scan from the first line
        for i in range(8):
            set_col([0,0,0,0,0,0,0,0])
            set_row(row)
            set_col(mat[i])
            row.pop(7)
            row.insert(0, 0)
            time.sleep(0.001)



if __name__ == '__main__':
    thread.start_new_thread(display, ()) # display thread

    while True:
        c = raw_input('Input a digit or char: ').replace('\n', '')

        mat = get_font_mat(ord(c))

