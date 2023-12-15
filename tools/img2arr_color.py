from PIL import Image
import numpy as np
import os
import math

def rgb_image_to_header_file(image_path, output_path):
    # 读取RGB图像
    image = Image.open(image_path)
    #将图像顺时针旋转90度
    # 将图像规范化到600*800、
    image = image.resize((600, 800))
    width, height = image.size
    # 分通道二值化并扩散
    r, g, b = image.split()
    # r = np.array(r.convert('1').convert('L'), dtype=np.uint8)
    # g = np.array(g.convert('1').convert('L'), dtype=np.uint8)
    # b = np.array(b.convert('1').convert('L'), dtype=np.uint8)
    # 此时 r, g, b 都是二维数组

    # 将三个二值化后的通道堆叠成一个多维数组
    pixels = np.stack((r, g, b), axis=-1)
    # 获取RGB通道数据
    # pixels = np.array(image, dtype=np.uint8)

    # 转换为CMY数据
    # pixels = 255 - pixels

    # 创建数据数组
    data = []
    for y in range(height):
        # 每行的像素数必须是偶数
        for x in range(width // 2):
            # 获取当前像素的RGB值
            r1, g1, b1 = pixels[y, 2*x]
            r2, g2, b2 = pixels[y, 2*x+1]
            
            # 缩减颜色通道值到4位
            r1, g1, b1 = r1 // 16, g1 // 16, b1 // 16
            r2, g2, b2 = r2 // 16, g2 // 16, b2 // 16
            # print(f"第一个像素颜色如下{r1:02X},{g1:02X},{b1:02X}")
            # print(f"第二个像素颜色如下{r2:02X},{g2:02X},{b2:02X}")
            
            # 将像素转为CMY
            # r1, g1, b1 = 15 - r1, 15 - g1, 15 - b1
            # r2, g2, b2 = 15 - r2, 15 - g2, 15 - b2

            #将像素转为CYM
            # r1, g1, b1 = r1, b1, g1
            # r2, g2, b2 = r2, b2, g2

            # 取反操作
            # r1 = 15 - r1
            # g1 = 15 - g1
            # b1 = 15 - b1
            # r2 = 15 - r2
            # g2 = 15 - g2
            # b2 = 15 - b2
        
            # 将R1G1B1合并到1.5字节
            byte1 = (r1 << 4) | g1
            byte2 = (b1 << 4) | r2
            byte3 = (g2 << 4) | b2
            data.append(f'0x{byte1:02X}')
            data.append(f'0x{byte2:02X}')
            data.append(f'0x{byte3:02X}')
            # print(f"三个字节分别为{byte1:02x},{byte2:02x},{byte3:02x}")
    array_size = len(data)

    # 写入.h文件
    base_name = os.path.splitext(os.path.basename(output_path))[0]
    with open(output_path, 'w') as file:
        file.write(f'const uint32_t {base_name}_width = {width * 3};\n')
        file.write(f'const uint32_t {base_name}_height = {height};\n')
        file.write(f'const uint8_t {base_name}_data[{array_size}] = {{\n    ')

        # 每行写入16个数据元素
        for i, byte in enumerate(data):
            file.write(byte + ', ')
            if (i + 1) % 16 == 0:
                file.write('\n    ')

        file.write('\n};')

# 调用函数
image_path = input('输入图像路径: ')  # 替换为图像文件路径
output_path = input('输出文件路径: ')  # 输出文件的路径
rgb_image_to_header_file(image_path, output_path)


