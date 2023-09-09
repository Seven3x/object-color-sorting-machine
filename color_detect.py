import sensor, image, time
from pyb import UART
import ustruct

uart = UART(3, 115200)  #串口3，波特率115200
uart.init(115200,8,None,1) #8个数据位，无奇偶校验，1个停止位


# 为了使色彩追踪效果真的很好，你应该在一个非常受控制的照明环境中。
green_threshold   = (0, 26, -128, -15, -128, 127)
red_threshold = (24, 100, 4, 127, -63, 113)
blue_threshold = (0, 100, -128, 5, -128, -11)
yellow_threshold = (18, 100, -46, 27, 31, 69)
# 设置绿色的阈值，括号里面的数值分别是L A B 的最大值和最小值（minL, maxL, minA,
# maxA, minB, maxB），LAB的值在图像左侧三个坐标图中选取。如果是灰度图，则只需
# 设置（min, max）两个数字即可。

def sending_data(color):
    global uart;
    data = ustruct.pack("<bbb",      #格式为俩个字符俩个短整型(2字节)
                   int(color), # up sample by 4   #数据1
                   0x0d,
                   0x0a)
    uart.write(data);   #必须要传入一个字节数组


sensor.reset() # 初始化sensor

sensor.set_pixformat(sensor.RGB565) # use RGB565.
#设置图像色彩格式，有RGB565色彩图和GRAYSCALE灰度图两种

sensor.set_framesize(sensor.QVGA) # 使用QQVGA的速度。
#设置图像像素大小

sensor.skip_frames(10) # 让新的设置生效。
sensor.set_auto_gain(False) # 在颜色识别中，不一定关闭自动增益
# 关闭自动增益的图像比开自动增益的图像更暗，跟不容易识别颜色
sensor.set_auto_whitebal(False) # turn this off.
#关闭白平衡。白平衡是默认开启的，在颜色识别中，需要关闭白平衡。
clock = time.clock() # 跟踪FPS帧率

thresholds = [green_threshold, red_threshold, blue_threshold, yellow_threshold]

while(True):
    color = 0
    clock.tick()
    img = sensor.snapshot().lens_corr(1.8)
    for blob in img.find_blobs(thresholds, pixels_threshold=200, area_threshold=200):
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
#        print(blob.code())

        color  = blob.code()
    # print(clock.fps()) # 当连接电脑后，OpenMV会变成一半的速度。当不连接电脑，帧率会增加。

    sending_data(color)
    a = uart.read(3)
    print(a)
    time.sleep_ms(10)
