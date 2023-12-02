from PIL import Image
import numpy as np
import re
import os

def read_image_properties(file_path):
    base_name = os.path.basename(file_path)
    array_name = os.path.splitext(base_name)[0]  # 移除扩展名

    with open(file_path, 'r') as file:
        content = file.read()

        # 使用正则表达式查找宽度和高度
        width_match = re.search(rf'{array_name}_width\s*=\s*(\d+);', content)
        height_match = re.search(rf'{array_name}_height\s*=\s*(\d+);', content)

        if width_match and height_match:
            width = int(width_match.group(1))
            height = int(height_match.group(1))
        else:
            return None, None, None

        # 查找图像数据数组
        data_match = re.search(rf'{array_name}_data\[\s*\(\s*\d+\s*\*\s*\d+\s*\)\s*/\s*2\s*\]\s*=\s*{{(.+?)}};', content, re.DOTALL)
        if not data_match:
            return None, None, None

        # 将数组数据转换为整数列表
        data_str = data_match.group(1)
        data_str = re.sub(r'[^0-9A-Fa-fx,]', '', data_str)
        data = [int(x, 16) for x in data_str.split(',') if x.strip()]

        return width, height, data

def create_image_from_data(data, width, height):
    pixels = []
    for byte in data:
        high, low = byte >> 4, byte & 0x0F
        pixels.append(high * 16)
        pixels.append(low * 16)

    total_pixels = width * height
    if len(pixels) < total_pixels:
        pixels += [0] * (total_pixels - len(pixels))

    pixels = np.array(pixels, dtype=np.uint8).reshape((height, width))
    return Image.fromarray(pixels, mode='L')

# 替换为你的文件路径
file_path = input("请输入文件路径：")

# 读取图像属性和数据
width, height, img_data = read_image_properties(file_path)
if width and height and img_data is not None:
    # 创建并显示图像
    image = create_image_from_data(img_data, width, height)
    image.show()
else:
    print("图像数据未找到或文件格式不正确。")
