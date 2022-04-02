# -*- coding: utf-8 -*-
"""
Created on Sun Mar 27 13:12:35 2022

@author: Bradley
"""

import os

def read_file(fileName):
    f = open(fileName, "r")
    text = f.read()
    f.close()
    return text

def write_file(outFile, inFile):
    text = read_file(inFile)
    f = open(outFile, "a")
    f.write(text)
    f.write("\n\n\n")
    f.close()

def get_list_of_files(path):
    files = os.listdir(path)
    
    result = []
    for file in files:
        result.append(path + "/" + file)
    
    return result

def main():
    
    files = get_list_of_files("C:/Users/Bradley/Desktop/pgnDownloads/GameStates/")
    out = open("BoardValues.txt", "w+")
    board_state = ""
    count = 0
    
    for file in files:
        count += 1
        if count % 10000 == 0:
            print("Count: {}".format(count))
        read = open(file, "r")
        board_state = file.split("/")[-1]
        board_state = board_state.split(".")[0]
        board_state = board_state.replace(",", "/")
        out.write(board_state + " " + read.read() + "\n")
        read.close()
    out.close()
    

if __name__ == "__main__":
    main()