# -*- coding: utf-8 -*-
"""
Created on Sat Apr  2 02:37:49 2022

@author: Bradley
"""

game_file = "coordFile.txt"
values_file = "board_values.txt"

import chess

def increment_values(games, board, result):
    if not (board in games.keys()):
        games[board] = [0, 0, 0]
    if result == -1:
        games[board][2] += 1
    if result == 0:
        games[board][1] += 1
    if result == 1:
        games[board][0] += 1
    return games

def save_games(games, filename):
    writer = open(filename, "w+")
    
    for key in games.keys():
        count = 0
        count += games[key][0]
        count += games[key][1]
        count += games[key][2]
        
        value = 0
        value += games[key][0]
        value -= games[key][2]
        
        if count <= 0:
            continue
        
        value = value / float(count)
        
        writer.write(str(key) + " " + str(value) + "\n")
    
    writer.close()

def main():
    game_values = {}
    count = 1
    failure_count = 1
    games = open(game_file, "r")
    
    try:
        for game in games:
            board = chess.Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
            line_split = game.split(' ')
            # get winner
            if line_split[0] == "\n":
                continue
            result = int(line_split[0])
            board_string = board.fen().split(" ")[0]
            
            
            game_values = increment_values(game_values, board_string, result)
            
            # for each move
            try:
                for move in line_split[1:-1]:
                    board.push(chess.Move.from_uci(move))
                    board_string = board.fen().split(" ")[0]
                    game_values = increment_values(game_values, board_string, result)
            except:
                print("Failed game on game \'{}\'".format(game))
                failure_count += 1
                return
                continue
            
            count += 1
            
            if count % 10000 == 0:
                print("Saving {} games to file".format(count))
                save_games(game_values, values_file)
    except:
        print("Handled {} games before failing".format(count))
        print("Handled {} failed games before failing".format(failure_count))
        games.close()
        return
    
    print("Handled {} games".format(count))
    print("Handled {} failed games".format(failure_count))
    games.close()
    
if __name__ == '__main__':
    main()