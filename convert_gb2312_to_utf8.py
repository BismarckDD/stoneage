import os
import chardet

def detect_encoding(file_path):
    with open(file_path, 'rb') as file:
        raw_data = file.read()
        result = chardet.detect(raw_data)
        return result['encoding']

def convert_to_utf8(file_path):
    with open(file_path, 'rb') as file:
        raw_data = file.read()
        encoding = chardet.detect(raw_data)['encoding']
        try:
           if encoding == 'GB2312' or encoding == 'GBK':
               text = raw_data.decode('gbk')  # GBK 是 GB2312 的超集，统一使用 GBK 解码
           else:
               return
           with open(file_path, 'w', encoding='utf-8') as utf8_file:
               utf8_file.write(text)
           print(f"Converted {file_path} to UTF-8")
        except Exception as e:
           print(f"Error converting {file_path}: {e}")

def scan_and_convert(folder_path):
    for root, dirs, files in os.walk(folder_path):
        for file in files:
            file_path = os.path.join(root, file)
            encoding = detect_encoding(file_path)
            if encoding == 'GB2312' or encoding == 'GBK':
                convert_to_utf8(file_path)

if __name__ == "__main__":
    import sys
    if len(sys.argv) > 1:
        folder_path = sys.argv[1]
    else:
        folder_path = input("Enter the folder path to scan: ")
    scan_and_convert(folder_path)

