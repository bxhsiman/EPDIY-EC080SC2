from PIL import Image
import numpy as np
import os
import math

def rgb_image_to_header_file(image_path, output_path):
    # 读取RGB图像
    image = Image.open(image_path)
    width, height = image.size

    # 获取RGB通道数据
    pixels = np.array(image, dtype=np.uint8)

    # 创建数据数组
    data = []
    for y in range(height):
        for x in range(width):
            # 获取当前像素的RGB值
            r, g, b = pixels[y, x]
            # 缩减颜色通道值到4位
            r, g, b = r // 16, g // 16, b // 16

             # 将RGB通道合并到1.5个字节
            byte1 = (r << 4) | g
            data.append(f'0x{byte1:02X}')

            byte2 = b << 4
            # 如果是最后一个像素或者下一个像素是新的一行的开始，则补充0
            if x == width - 1 or (x + 1) % width == 0:
                byte2 |= 0x0F  # 补充半个字节
            else:
                # 获取下一个像素的红色通道
                next_r = pixels[y, x+1][0] // 16
                byte2 |= next_r
            data.append(f'0x{byte2:02X}')

    # 确保数组大小是偶数
    array_size = len(data) if len(data) % 2 == 0 else len(data) + 1
    if len(data) < array_size:
        data.append('0xFF')  # 填充额外的字节

    # 写入.h文件
    base_name = os.path.splitext(os.path.basename(output_path))[0]
    with open(output_path, 'w') as file:
        file.write(f'const uint32_t {base_name}_width = {width};\n')
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
