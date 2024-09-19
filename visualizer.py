import cv2
import numpy as np

# Function to draw the Sudoku board


def decode(x):
    side = x//(9*9*9)  # 0 for weak, 1 for strong
    rest = x % (9*9*9)
    xx = rest // 81
    yy = rest // 9 % 9
    candidate = rest % 9
    return side, xx, yy, candidate


def draw_sudoku(a, candidate, involved_candidates, removed):
    removed_s, removed_x, removed_y, removed_c = decode(removed)
    # Constants for the board
    board_size = 900  # 450x450 pixels
    cell_size = board_size // 9  # Each cell is 50x50 pixels
    board = np.ones((board_size, board_size, 3),
                    dtype=np.uint8) * 255  # White background

    # Drawing the grid lines
    for i in range(10):
        thickness = 2 if i % 3 == 0 else 1
        # Horizontal lines
        cv2.line(board, (0, i * cell_size), (board_size,
                 i * cell_size), (0, 0, 0), thickness)
        # Vertical lines
        cv2.line(board, (i * cell_size, 0),
                 (i * cell_size, board_size), (0, 0, 0), thickness)

    # Drawing the numbers in the grid
    font = cv2.FONT_HERSHEY_SIMPLEX
    for i in range(9):
        for j in range(9):
            if a[i][j] != 0:
                text = str(a[i][j])
                # Calculate position to center the text
                text_scale = 2
                text_size = cv2.getTextSize(text, font, 2, 2)[0]
                text_x = j * cell_size + (cell_size - text_size[0]) // 2
                text_y = i * cell_size + (cell_size + text_size[1]) // 2
                cv2.putText(board, text, (text_x, text_y),
                            font, 2, (0, 0, 0), 2, cv2.LINE_AA)
            else:
                # Draw candidate numbers in smaller font
                for k in range(9):
                    # If the k-th candidate is possible
                    if candidate[i][j] & (1 << k):
                        candidate_text = str(k + 1)
                        candidate_font_scale = 1
                        candidate_text_size = cv2.getTextSize(
                            candidate_text, font, candidate_font_scale, 1)[0]
                        candidate_x = j * cell_size + \
                            (k % 3) * cell_size // 3 + 5
                        candidate_y = i * cell_size + \
                            (k // 3 + 1) * cell_size // 3 - 5
                        color = (200, 200, 200)
                        thickness = 1
                        if (1 << k) & involved_candidates:
                            color = (255, 0, 0)
                            thickness = 2
                        if removed_x == i and removed_y == j and removed_c == k:
                            color = (0, 0, 255)
                            thickness = 1
                        cv2.putText(board, candidate_text, (candidate_x, candidate_y),
                                    font, candidate_font_scale, color, thickness)

    return board


def drawLink(img, pos1, pos2):
    s1, x1, y1, c1 = decode(pos1)
    s2, x2, y2, c2 = decode(pos2)
    assert s1 != s2
    # get the center of candidate c1 in cell (x1, y1)
    cell_size = 900 // 9
    center1 = (y1 * cell_size + (c1 % 3) * cell_size // 3 + 10,
               x1 * cell_size + (c1 // 3 + 1) * cell_size // 3 - 10)
    # get the center of candidate c2 in cell (x2, y2)
    center2 = (y2 * cell_size + (c2 % 3) * cell_size // 3 + 10,
               x2 * cell_size + (c2 // 3 + 1) * cell_size // 3 - 10)
    color = (0, 255, 0) if s1 == 0 else (0, 0, 255)
    cv2.line(img, center1, center2, color, 2)
    return img


a = np.zeros((9, 9), dtype=int)
bits = np.zeros((9, 9), dtype=int)
with open("data.in", "r") as f:
    for i in range(9):
        a[i] = list(map(int, f.readline().split()))
    for i in range(9):
        bits[i] = list(map(int, f.readline().split()))
    # Draw the sudoku board

    # Show the Sudoku board
    # cv2.imshow('Sudoku', sudoku_board)
    # cv2.waitKey(0)
    cnt = 0
    while True:
        z = f.readline().strip()
        if not z:
            break
        path = list(map(int, z.split()))
        assert path[0] == len(path) - 1
        path = path[1:]
        involved_candidates = 0
        for i in path:
            s, x, y, c = decode(i)
            involved_candidates |= 1 << c
        _, xx, yy, c = decode(path[-1])
        sudoku_board = draw_sudoku(a, bits, involved_candidates, path[-1])
        bits[xx][yy] ^= 1 << c
        for x, y in zip(path, path[1:]):
            drawLink(sudoku_board, x, y)
        # cv2.imshow('Sudoku', sudoku_board)
        cv2.imwrite(f"tmp/{cnt:02}.png", sudoku_board)
        cnt+=1
        # cv2.waitKey(0)

cv2.destroyAllWindows()
