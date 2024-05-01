import ast

import kernel
import utils as ut
from kernel import cmc_kernel as ker


class OutOfStrategy(BaseException):
    def __init__(self, msg):
        self.msg = msg


class Strategy:
    def __init__(self, target_matter: str, card_id_list: list):
        self.target_matter = target_matter.strip()
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

    def matters(self):
        ms = [x.target_matter for x in self.lst]
        ms = list(set(ms))
        return ms

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
        # it is certain that all cards to play are in hand
        self.hand = ut.removeSubsequence(self.hand, card_names)

    def win(self):
        return len(self.hand) == 0

    def showHand(self):
        print(self.hand)

    def getLegalChoices(self, matter_to_react: str):
        """
        return StrategyList object or throw exceptions
        """
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
            raise OutOfStrategy(status)


class Player(PlayerBase):
    def __init__(self):
        super().__init__()

    def execCommand(self, cmd: str, facing_matter: str):
        # if a string is start with '.', it is a command
        if not cmd.startswith('.'):
            return
        if cmd == '.draw':
            self.draw()
            self.showHand()
        elif cmd == '.suggest':
            self.suggest(facing_matter)
        elif cmd.split(' ')[0] == '.help':
            element_name = cmd.split(' ')[1]
            matters = ker.ShowAllMattersHasElement(element_name)
            print(matters)

    def suggest(self, matter):
        try:
            stl = self.getLegalChoices(matter)
            matters = stl.matters()
            print(matters)
        except OutOfStrategy as e:
            if e.msg == "Terminal matter on the field":
                print("这个物质无法反应，pass吧")
            elif e.msg == "I need element cards" or e.msg == "I need auxiliary cards":
                print("你需要更多的手牌来构造反应物")

    def act(self, matter):
        self.showHand()
        while True:
            try:
                cards_to_play = input("请输入要出的牌").strip()
                self.execCommand(cards_to_play, matter)
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
                    self.playCard(cards_lst)
                    return matter_to_play
                else:
                    reaction_list = ut.toListOfStr(ker.QueryReactions(','.join([matter_to_play, matter])))
                    if len(reaction_list) == 0:
                        raise ValueError
                    for i in range(len(reaction_list)):
                        print(f'[{i}]', reaction_list[i])
                    reaction_idx = input("请选择反应式的编号")
                    try:
                        reaction_idx = int(reaction_idx)
                    except:
                        raise ValueError
                    reaction = reaction_list[reaction_idx]
                    resultants = ut.getResultantsFromEq(reaction)
                    matter_to_obtain = input(f"请从{resultants}中选择一个生成物")
                    if matter_to_obtain not in resultants:
                        raise ValueError
                    self.playCard(cards_lst)
                    return matter_to_obtain
                # return
            except ValueError:
                pass


class Robot(PlayerBase):
    def __init__(self):
        super().__init__()

    def act(self, matter):
        if matter is not None:
            while True:
                try:
                    stl = self.getLegalChoices(matter)
                    strategy = stl.longest()
                    # play card by id list
                    card_names = ker.CardIdsToNames(strategy.card_id_list_toString()).split(',')[:-1]
                    self.playCard(card_names)
                    print('AI出牌：', card_names)
                    print('AI出的物质是：', strategy.target_matter)
                    # react with the chosen matter
                    reaction_list = ut.toListOfStr(ker.QueryReactions(','.join([matter, strategy.target_matter])))
                    # choose the minimum reaction (the first reaction as default)
                    min_reaction = reaction_list[0]
                    print('AI选择的反应是：', min_reaction)
                    resultants = ut.getResultantsFromEq(min_reaction)
                    chosen_resultant = ker.QueryWhichHasMinH(','.join(resultants))
                    print('AI选择的生成物是：', chosen_resultant)
                    return chosen_resultant
                except OutOfStrategy as e:
                    if e.msg == "Terminal matter on the field":
                        return None
                    elif e.msg == "I need element cards" or e.msg == "I need auxiliary cards":
                        print("AI抽了一张牌")
                        self.draw()
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
            if self.current == 0:
                print(" -- 轮到你出牌了！ --")
                print("你的牌数：", len(self.players[0].hand))
                print("AI的牌数：", len(self.players[1].hand))


Game().start()
