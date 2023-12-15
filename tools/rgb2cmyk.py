from PIL import Image

def convert_to_4bit_cmyk(input_image_path):
    # Open the image
    with Image.open(input_image_path) as img:
        # 确保图片是RGB模式
        if img.mode != 'RGB':
            img = img.convert('RGB')

        r = img.split()[0]
        g = img.split()[1]
        b = img.split()[2]

        #二值化
        r = r.convert('1')
        g = g.convert('1')
        b = b.convert('1')
        
        r = r.convert('L')
        g = g.convert('L')
        b = b.convert('L')
        #合并
        img = Image.merge('RGB', (r, g, b))
        img.save('output.png')
# Example usage
image_path = input('输入图像路径')  # 替换为图像文件路径
convert_to_4bit_cmyk(image_path)
