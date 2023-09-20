import sensor, image, time, math
from pyb import UART
import ustruct
import pyb

uart = UART(3, 115200)  #串口3，波特率115200
uart.init(115200,8,None,1) #8个数据位，无奇偶校验，1个停止位


f_x = (2.8 / 3.984) * 160 # find_apriltags defaults to this if not set
f_y = (2.8 / 2.952) * 120 # find_apriltags defaults to this if not set
c_x = 160 * 0.5 # find_apriltags defaults to this if not set (the image.w * 0.5)
c_y = 120 * 0.5 # find_apriltags defaults to this if not set (the image.h * 0.5)

def degrees(radians):
    return (180 * radians) / math.pi

def sending_data(color):
    global uart;
    data = ustruct.pack("<bbb",      #格式为俩个字符俩个短整型(2字节)
                   int(color), # up sample by 4   #数据1
                   0x0d,
                   0x0a)
    uart.write(data);   #必须要传入一个字节数组

def write_thres(file, thresholds):
    """
    写入图像阈值到文件 threshold.txt
    """
    file.write('{: >4},{: >4},{: >4},{: >4},{: >4},{: >4}\n'.format(thresholds[0][0],\
                                                                    thresholds[0][1],\
                                                                    thresholds[0][2],\
                                                                    thresholds[0][3],\
                                                                    thresholds[0][4],\
                                                                    thresholds[0][5]))

red_led = pyb.LED(1)
blue_led = pyb.LED(3)


sensor.reset() # 初始化sensor
sensor.set_pixformat(sensor.RGB565) # use RGB565.
#设置图像色彩格式，有RGB565色彩图和GRAYSCALE灰度图两种
sensor.set_framesize(sensor.QQVGA) # 使用QQVGA图像大小
roi = [30, 16, 100 ,60]
sensor.set_windowing(roi) #设置roi
sensor.skip_frames(10) # 让新的设置生效。
sensor.set_auto_gain(False) # 在颜色识别中，不一定关闭自动增益
# 关闭自动增益的图像比开自动增益的图像更暗，跟不容易识别颜色
sensor.set_auto_whitebal(False) # turn this off.
#关闭白平衡。白平衡是默认开启的，在颜色识别中，需要关闭白平衡。
clock = time.clock() # 跟踪FPS帧率

# 创建阈值对象
red_threshold = [0,0,0,0,0,0]
green_threshold = [0,0,0,0,0,0]
blue_threshold = [0,0,0,0,0,0]
yellow_threshold = [0,0,0,0,0,0]
thresholds = [red_threshold, green_threshold, blue_threshold, yellow_threshold]

# 计时量，当扫描到tag时触发
count_t = 0;
count_lock = 0;

# 红 绿 蓝 黄对应位置相对于tag的误差
pos_error = [(130 - 82, 67 - 70),
             (112 - 82, 67 - 70),
             (43  - 82, 67 - 71),
             (22  - 86, 67 - 71)]

# 读取颜色阈值
f = open('threshold.txt', 'r+')
for i in range(len(thresholds)):
    thres = f.readline()
    lmax = eval(str.strip(thres[0:4]))
    lmin = eval(str.strip(thres[5:9]))
    amax = eval(str.strip(thres[10:14]))
    amin = eval(str.strip(thres[15:19]))
    bmax = eval(str.strip(thres[20:24]))
    bmin = eval(str.strip(thres[25:29]))
    thresholds[i] = [lmax, lmin, amax, amin, bmax, bmin]
f.close()
# 矫正时颜色统计信息
#
color_uq = [[0,0,0,0] for i in range(3)]
color_lq = [[0,0,0,0] for i in range(3)]

# 颜色矫正时roi大小
color_roi = (8, 8)

while(True):
    color = 0
    clock.tick()

    img = sensor.snapshot().lens_corr(1.2)
#    for tag in img.find_apriltags(fx=f_x, fy=f_y, cx=c_x, cy=c_y): # defaults to TAG36H11

#        img.draw_rectangle(tag.rect(), color = (255, 0, 0))
#        cx = tag.cx()
#        cy = tag.cy()

#        if count_lock == 0 and count_t == 0:
#            blue_led.on()
#            time.sleep_ms(7000);
#            count_t = 200
#            count_lock = 1
#        elif count_lock != 0 and count_t == 0:
#            count_lock = 0;
#            img.draw_cross(cx, cy, color = (0, 255, 255))

#        img.draw_cross(cx, cy, color = (0, 255, 0))
##        print(cx, cy)
##        print(tag.rect())

#        # 提取统计信息并矫正
#        if count_lock == 1 and count_t == 190:
#            blue_led.off()
#            for i in range(0,4):
#                try:
#                    statistics=img.get_statistics(roi=(cx + pos_error[i][0],\
#                                                    cy + pos_error[i][1],\
#                                                    color_roi[0], \
#                                                    color_roi[1]))
#                    color_uq[0][i] = statistics.l_uq()
#                    color_uq[1][i] = statistics.a_uq()
#                    color_uq[2][i] = statistics.b_uq()
#                    color_lq[0][i] = statistics.l_lq()
#                    color_lq[1][i] = statistics.a_lq()
#                    color_lq[2][i] = statistics.b_lq()
#                except:
#                    red_led.on()
#                    continue
##                    exit(0)


##            for i in range(0,3):
##                print('t')
##                print(color_lq[i])
##                print(color_uq[i])
##                templ = color_lq[i]
##                templ.sort()
##                print(templ)
##                tempu = color_uq[i]
##                tempu.sort()
##                print(tempu)
#            for i in range(0,4):
#                for j in range(0,3):
#                    thresholds[i][2*j] = color_lq[j][i] - 5
#                    thresholds[i][2*j+1] = color_uq[j][i] +5
##            f = open('threshold.txt', 'w+')

##            write_thres(f, thresholds)
##            f.close()


##            print(color_lq);

#        print(thresholds)
#        try:
#            # 红色位置
#            img.draw_rectangle((cx + pos_error[0][0], cy + pos_error[0][1], color_roi[0], color_roi[1]), color = (255, 0, 0))
#            # 绿色位置
#            img.draw_rectangle((cx + pos_error[1][0], cy + pos_error[1][1], color_roi[0], color_roi[1]), color = (255, 0, 0))
#            # 蓝色位置
#            img.draw_rectangle((cx + pos_error[2][0], cy + pos_error[2][1], color_roi[0], color_roi[1]), color = (255, 0, 0))
#            # 黄色位置
#            img.draw_rectangle((cx + pos_error[3][0], cy + pos_error[3][1], color_roi[0], color_roi[1]), color = (255, 0, 0))
#        except:
#            pass


    if count_t >= 1:
        count_t -= 1;

    for blob in img.find_blobs(thresholds, pixels_threshold=100, area_threshold=100):

        if blob.code() == 1:
            print('红')
        if blob.code() == 2:
            print('绿')
        if blob.code() == 4:
            print('蓝')
        if blob.code() == 8:
            print('黄')
        img.draw_rectangle(blob.rect())
#        print(blob.code())

        color  = blob.code()
#    print(clock.fps()) # 当连接电脑后，OpenMV会变成一半的速度。当不连接电脑，帧率会增加。
    if count_t == 0:
        sending_data(color)
    a = uart.read(3)
    if a is not None:
#        print(a)
        pass
#    time.sleep_ms(10)
