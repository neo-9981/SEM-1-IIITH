import os
from PIL import Image
from datetime import datetime
import sys


def extract_pixel_values():
    ip_dir = "./data/input/"
    ip_img = sys.argv[1]
    ip_file = "rgb_in.txt"
    # Read input image
    img = Image.open(ip_dir + ip_img)
    px = img.load() # load pixel data

    W = img.size[0] # width
    H = img.size[1] # height
    C = 3 # 3 channels RGB

    # create and open input file(rgb_in.txt)
    ip_fp = open(ip_dir + ip_file, "w+") 

    # write the dimension of the image to the file
    ip_fp.write(str(H) + " " + str(W) + " " + str(C) + "\n")

    # write the individual pixel values to the file
    for i in range(0, H):
        for j in range(0, W):
            for k in range(0, C):
                ip_fp.write(str(px[j,i][k]) + " \n"[j == W-1 and k == C-1])

    # close the img file
    ip_fp.close()

# ---------------------------------------------------------------------

def compile_and_run_cpp():
    # compile the cpp file
    os.system("g++ ./src/main.cpp -o ./bin/main")
    print("[*] Compilation Successful, Executing ./bin/main \n")
    # run the binary file
    os.system("./bin/main")

# ---------------------------------------------------------------------

def create_output_image():
    op_file = "rgb_out.txt"
    op_dir = "./data/output/"
    op_img = "sample_out.jpeg"
    # open output file(rgb_out.txt)
    op_fp = open(op_dir + op_file, "r")

    # read the dimension of the image from the file
    lines = op_fp.readlines()
    for i in range(len(lines)):
        lines[i] = lines[i].strip()
    H_, W_, C_ = [int(x.strip()) for x in lines[0].split(" ")]

    # create a new image
    img_out = Image.new("RGB", (W_, H_))

    # read the individual pixel values from the file
    for i in range(0, H_):
        px_list = [int(x) for x in lines[i+1].split(" ")]
        for j in range(0, W_):
            img_out.putpixel((j, i), (px_list[j*3 + 0], px_list[j*3 + 1], px_list[j*3 + 2]))

    # save the image
    img_out.save(op_dir + op_img)

    # close the output file
    op_fp.close()

# ---------------------------------------------------------------------

def main():

    if(len(sys.argv) !=2):
        print("ERROR: Expected input file name as a command line argumnet")
        exit(0)

    # start time
    start_time = datetime.now()

    # extract pixel values from the input image
    extract_pixel_values()
    t1 = (datetime.now() - start_time).total_seconds()
    # (t1) is the time taken to extract pixel values
    print("[*] Created ./input/rgb_in.txt with RGB pixel vaues in %.2f seconds\n" % t1)

    # compile and run the cpp file
    compile_and_run_cpp()
    t2 = (datetime.now() - start_time).total_seconds()
    # (t2 - t1) is the time taken to compile and run the cpp file
    print("[*] Compiled and Executed ./src/main.cpp in %.2f seconds\n" % (t2 - t1))

    # create output image
    create_output_image()
    t3 = (datetime.now() - start_time).total_seconds() 
    # (t3 - t2) is the time taken to create output image
    print("[*] Saved ./output/sample_out.jpeg to disk in %.2f seconds\n" % (t3 - t2))

    # end time
    end_time = datetime.now()
    # print the execution time
    print("[*] Total Execution time: %.2f seconds\n" % ((end_time - start_time).total_seconds()))

if __name__ == "__main__":
    main()
