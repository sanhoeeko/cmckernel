import ast
import random as rd

import kernel
import utils as ut
from kernel import cmc_kernel as ker


class Strategy:
    def __init__(self, target_matter: str, card_id_list: list):
        self.target_matter = target_matter
        self.card_id_list = card_id_list
        
    def __repr__(self):
        return f'({self.target_matter}, {str(self.card_id_list)})'
    
    def __len__(self):
        return len(self.card_id_list)
    
    def card_id_list_toString(self):
        return ','.join(list(map(str, self.card_id_list)))

class StrategyList:
    def __init__(self, dic: dict):
        self.lst = []
        for k, v in dic.items():
            for item in v:
                self.lst.append(Strategy(k, item))
                
    def __repr__(self):
        return str(self.lst)
    
    def longest(self):
        self.lst.sort(key=len, reverse=True)
        return self.lst[0]
        

class PlayerBase:
    def __init__(self):
        self.hand = []

    def draw(self, num=1):
        self.hand.extend(kernel.randCard(num))
        self.hand.sort()
        
    def playCard(self, card_names: list):
        # it is sure that all cards to play are in hand
        self.hand = ut.removeSubsequence(self.hand, card_names)

    def win(self):
        return len(self.hand) == 0

    def showHand(self):
        print(self.hand)

    def getLegalChoices(self, matter_to_react: str):
        hand_str = ','.join(list(map(str, self.hand)))
        hand_str = ker.CardNamesToIds(hand_str)[:-1]
        query = matter_to_react + ';' + hand_str
        result = ker.QueryLegalStrategies(query)
        status, choices = result.split(';')
        if status == "Success":
            dic = {}
            choices = choices.split('\n')
            for c in choices:
                if len(c) == 0:
                    break
                matter, strategy = c.split(':')
                dic[matter] = ast.literal_eval(strategy)
            stl = StrategyList(dic)
            return stl
        else:
            raise NotImplementedError


class Player(PlayerBase):
    def __init__(self):
        super().__init__()

    def execCommand(self, cmd: str):
        # if a string is start with '.', it is a command
        if not cmd.startswith('.'):
            return
        if cmd == '.draw':
            self.draw()
            self.showHand()
        elif cmd == '.suggest':
            self.suggest()

    def suggest(self):
        pass

    def act(self, matter):
        self.showHand()
        while True:
            try:
                cards_to_play = input("请输入要出的牌").strip()
                self.execCommand(cards_to_play)
                # check if the player has these cards
                cards_lst = cards_to_play.split(' ')
                cards_lst.sort()
                if not ut.isSubsequence(cards_lst, self.hand):
                    raise ValueError
                matter_list = ut.toListOfStr(ker.QueryPossibleMatters(cards_to_play))
                if len(matter_list) == 0:
                    raise ValueError
                print(matter_list)
                matter_to_play = input("请输入要出的物质").strip()
                if matter_to_play not in matter_list:
                    raise ValueError
                if matter is None:
                    # if there is no matter to react
                    return matter_to_play
                else:
                    reaction_list = ut.toListOfStr(ker.QueryReactions(','.join([matter_to_play, matter])))
                    if len(reaction_list) == 0:
                        raise ValueError
                    print(reaction_list)
                return
            except ValueError:
                pass


class Robot(PlayerBase):
    def __init__(self):
        super().__init__()

    def act(self, matter):
        if matter is not None:
            stl = self.getLegalChoices(matter)
            strategy = stl.longest()
            # play card by id list
            card_names = ker.CardIdsToNames(strategy.card_id_list_toString()).split(',')[:-1]
            self.playCard(card_names)
            print('AI出牌：', card_names)
            print('AI出的物质是：', strategy.target_matter)
            # react with the chosen matter
            reaction_list = ut.toListOfStr(ker.QueryReactions(','.join([matter, strategy.target_matter])))
            print(reaction_list)
        else:
            pass


class Game:
    def __init__(self):
        self.players = [Player(), Robot()]
        self.current = None
        self.current_matter = None

    def win(self):
        for i in range(len(self.players)):
            if self.players[i].win():
                return i
        return -1

    def start(self):
        # self.current = rd.randint(0, 1)
        self.current = 0
        current_player = self.players[self.current]
        for p in self.players:
            p.draw(16)
        current_player.draw(5)
        # game loop
        while True:
            self.current_matter = current_player.act(self.current_matter)
            print("当前需要应对的物质：", self.current_matter)
            self.current = (self.current + 1) % len(self.players)
            current_player = self.players[self.current]
            if self.win() != -1:
                break


Game().start()