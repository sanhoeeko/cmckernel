import random
import sys

import pygame

from game import Game


def id_to_name(n: int):
    if n < 0:
        return 'void'
    else:
        return ['H', 'C', 'N', 'O', 'F'][n]


class Cell(pygame.sprite.Sprite):
    def __init__(self, parent: 'CardGrid'):
        super().__init__()
        self.parent = parent
        self.set_card_id(-1)

    def set_card_id(self, card_id):
        self.card_id = card_id
        self.image = self.parent.images[card_id]
        self.rect = self.image.get_rect()


class CardGrid:
    def __init__(self, rows, cols, cell_size=80, cell_margin=5):
        self.rows = rows
        self.cols = cols
        self.cell_size = cell_size
        self.cell_margin = cell_margin
        self.grid = [[Cell(self) for j in range(cols)]
                     for i in range(rows)]
        self.selected_cards = []
        self.scroll_bar_total_height = 3 * (self.cell_size + self.cell_margin)
        self.scroll_bar_slider_height = self.scroll_bar_total_height * 3 / 16
        self.scroll_y = 0

    def __getitem__(self, idx: int):
        return self.grid[idx]  # 返回一个列表

    @property
    def images(self):
        images = {}
        for i in range(-1, 16):
            try:
                images[i] = pygame.transform.scale(
                    pygame.image.load(rf'img/{id_to_name(i)}.png'),
                    (self.cell_size, self.cell_size))
            except:
                pass
        return images

    def move(self, row, col):
        target = self.selected_cards[0]
        self.grid[row][col].set_card_id(self.grid[target[0]][target[1]].card_id)
        self.grid[target[0]][target[1]].set_card_id(-1)
        self.selected_cards = []

    def add_random_card(self):
        for i in range(len(self.grid)):
            for j in range(len(self.grid[i])):
                if self.grid[i][j].card_id == -1:
                    self.grid[i][j].set_card_id(random.randint(0, 4))
                    self.selected_cards = [(i, j)]
                    return

    def collect_all_cards(self) -> list[int]:
        card_ids = []
        for i in range(len(self.grid)):
            for j in range(len(self.grid[i])):
                if self.grid[i][j].card_id != -1:
                    card_ids.append(self.grid[i][j].card_id)
        return card_ids

    def collect_selected_cards(self) -> list[int]:
        return [self.grid[row][col].card_id for row, col in self.selected_cards]

    def handle_event(self, event):
        if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
            mouse_x, mouse_y = pygame.mouse.get_pos()
            row = (mouse_y // (self.cell_size + self.cell_margin)) + self.scroll_y
            col = mouse_x // (self.cell_size + self.cell_margin)
            if self.grid[row][col].card_id != -1:  # 如果点击的格子有牌
                if (row, col) in self.selected_cards:  # 如果点击的牌已经被选中
                    self.selected_cards.remove((row, col))  # 取消选择
                else:
                    self.selected_cards.append((row, col))
            elif len(self.selected_cards) == 1:  # 如果已经选中了一张牌，并且点击的格子没有牌
                self.move(row, col)
        elif event.type == pygame.MOUSEWHEEL:
            self.scroll_y -= event.y
            self.scroll_y = min(max(self.scroll_y, 0), len(self.grid) - 3)

    def draw(self, window):
        for i in range(3):
            for j, cell in enumerate(self.grid[i + self.scroll_y]):
                # 绘制卡牌
                cell.rect.topleft = (j * (self.cell_size + self.cell_margin),
                                     i * (self.cell_size + self.cell_margin))  # 在这里计算格子的位置
                window.blit(cell.image, cell.rect)
                # 绘制选择框
                if (i + self.scroll_y, j) in self.selected_cards:
                    pygame.draw.rect(window, (255, 0, 0), cell.rect, 3)
        # 绘制滚动条
        scroll_bar_y = self.scroll_y * (self.cell_size + self.cell_margin) * (
                self.scroll_bar_total_height - self.scroll_bar_slider_height) / (len(self.grid) - 3)
        pygame.draw.rect(window, (128, 128, 128), pygame.Rect(
            window.get_width() - 20, 0, 20, self.scroll_bar_total_height))
        pygame.draw.rect(window, (255, 255, 255), pygame.Rect(
            window.get_width() - 20, scroll_bar_y, 20, self.scroll_bar_slider_height))


class Button:
    def __init__(self, x, y, width, height, text, func):
        self.rect = pygame.Rect(x, y, width, height)
        self.text = text
        self.func = func

    def draw(self, window):
        pygame.draw.rect(window, (128, 128, 128), self.rect)
        font = pygame.font.Font(None, 24)
        text_surf = font.render(self.text, True, (255, 255, 255))
        window.blit(text_surf, (self.rect.x + 5, self.rect.y + 5))

    def is_clicked(self, pos):
        return self.rect.collidepoint(pos)

    def action(self):
        self.func()


class ButtonHandler:
    def __init__(self, card_grid: CardGrid, left, top):
        self.backend = Game()
        self.card_grid = card_grid
        self.buttons = [
            Button(left + 0, top + 0, 150, 50, "Add Card", self.add_random_card),
            Button(left + 160, top + 0, 150, 50, "Play Cards", self.play_cards)
        ]

    def add_random_card(self):
        self.card_grid.add_random_card()

    def play_cards(self):
        selected_card_ids = self.card_grid.collect_selected_cards()
        self.backend.onPlayingCard(selected_card_ids)


    def handle_event(self, event):
        if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
            for button in self.buttons:
                if button.is_clicked(event.pos):
                    button.action()

    def draw(self, window):
        for button in self.buttons:
            button.draw(window)


class ButtonGrid:
    def __init__(self, right, bottom, button_width=100, button_height=30, margin=5):
        self.buttons = []
        self.right = right
        self.bottom = bottom
        self.button_width = button_width
        self.button_height = button_height
        self.margin = margin

    def create_buttons(self, labels):
        self.buttons = []
        cols = self.right // (self.button_width + self.margin)
        for i, label in enumerate(labels):
            row = i // cols
            col = i % cols
            x = self.right - (col + 1) * (self.button_width + self.margin)
            y = self.bottom - (row + 1) * (self.button_height + self.margin)
            button = Button(x, y, self.button_width,
                            self.button_height, label, lambda: print(i))
            self.buttons.append(button)

    def handle_event(self, event):
        if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
            for button in self.buttons:
                if button.is_clicked(event.pos):
                    button.action()

    def draw(self, window):
        for button in self.buttons:
            button.draw(window)


# 初始化pygame
pygame.init()

# 创建一个卡牌网格
grid = CardGrid(16, 12)
grid[0][0].set_card_id(0)
grid[2][3].set_card_id(3)
button_handler = ButtonHandler(grid, 0, 250)
# button_grid = ButtonGrid()

# 创建窗口
window_size = (12 * (grid.cell_size + grid.cell_margin), 800)
window = pygame.display.set_mode(window_size)

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()
        else:
            grid.handle_event(event)
            button_handler.handle_event(event)

    window.fill((255, 255, 255))
    grid.draw(window)
    button_handler.draw(window)
    pygame.display.update()
