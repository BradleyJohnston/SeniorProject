# -*- coding: utf-8 -*-
"""
Created on Wed Mar 30 01:31:24 2022

@author: Bradley
"""

import chess
board_states = { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" : [0, 0, 0] }

def algebToCoordMove(algebraeic, result):
    board = chess.Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
    split_game = algebraeic.split(" ")
    print(len(split_game))
    try:
        for i in range(2, len(split_game), 3):
            fileName = board.fen().split(" ")[0]
            print(fileName)
            if not board_states.has_key(fileName):
                board_states[fileName] = [0, 0, 0]
            if result == -1:
                board_states[fileName][2] += 1
            if result == 0:
                board_states[fileName][1] += 1
            if result == 1:
                board.states[fileName][0] += 1
            board.push_san(split_game[i])
    except:
        return
    return

def main():
    f = open("timelessFile.txt", "r")
    result = ""
    count = 1
    
    for line in f:
        count += 1
        if count % 10 == 0:
            break
        if line[0] == '[':
            result = line[9:-3]
            if result == "1-0":
                result = 1
            elif result == "0-1":
                result = -1
            else:
                result = 0
        if line[0:3] == " 1.":
            print("Result: {}".format(result))
            print("Line: {}".format(line))
            algebToCoordMove(line, result)
    
    writer = open("board_values.txt", "w+")
    
    print(board_states)
    
    for key in board_states.keys():
        writer.write(key + " " + str(board_states[key][0]) + ":" + str(board_states[key][1]) + ":" + str(board_states[key][2]))
    
    writer.close()
        
        
if __name__ == '__main__':
    main()