from PIL import Image
import numpy as np
import os

def image_to_header_file(image_path, output_path):
    # 读取图像并转换为灰度
    image = Image.open(image_path).convert('L')
    width, height = image.size

    # 将图像数据转换为4位格式
    pixels = np.array(image, dtype=np.uint8)
    pixels = pixels // 16  # 将像素值从8位缩减到4位

    # 创建数据数组
    data = []
    for y in range(height):
        for x in range(0, width, 2):
            # 每个字节包含两个像素
            high = pixels[y, x]
            low = pixels[y, x+1] if x+1 < width else 0
            byte = (high << 4) | low
            data.append(f'0x{byte:02X}')

    # 写入.h文件
    base_name = os.path.splitext(os.path.basename(image_path))[0]
    with open(output_path, 'w') as file:
        file.write(f'const uint32_t {base_name}_width = {width};\n')
        file.write(f'const uint32_t {base_name}_height = {height};\n')
        file.write(f'const uint8_t {base_name}_data[({width} * {height}) / 2] = {{\n    ')

        # 每行写入16个数据元素
        for i, byte in enumerate(data):
            file.write(byte + ', ')
            if (i+1) % 16 == 0:
                file.write('\n    ')

        file.write('\n};')

# 调用函数
image_path = input('输入图像路径')  # 替换为图像文件路径
output_path = input('输出文件路径')  # 输出文件的路径
image_to_header_file(image_path, output_path)
