import transitions as tr

from kernel import Kernel


class GameState:
    states = ['None', 'PlayerSelectCard', 'PlayerSelectMatter', 'PlayerSelectResultant', 'AISuggest', 'AIThink']
    transitions = [
        ['start_player', 'None', 'PlayerSelectCard'],
        ['start_AI', 'None', 'AISuggest'],
        ['play_card_non_unique_matter', 'PlayerSelectCard', 'PlayerSelectMatter'],
        ['play_card_unique_matter', 'PlayerSelectCard', 'PlayerSelectResultant'],
        ['select_matter', 'PlayerSelectMatter', 'PlayerSelectResultant'],
        ['AI_suggest_finished', 'AISuggest', 'AIThink']
    ]

    def __init__(self):
        self.machine = tr.Machine(self, GameState.states, initial='None', transitions=GameState.transitions)


class Player:
    def __init__(self):
        self.hand = []


class Game:
    def __init__(self):
        self.kernel = Kernel()
        self.state = GameState()
        self.human = Player()
        self.ai = Player()

    def onPlayingCard(self, card_ids: list[int]):
        # 判断是否只有一种可能的物质，是->直接选择生成物；否->选择反应物
        print(card_ids)
        matter_ids = self.kernel.possibleMatters(card_ids)
        print(matter_ids)
        '''
        if len(matter_ids) == 0:
            pass
        elif len(matter_ids) == 1:
            self.state.play_card_unique_matter()
        else:
            self.state.play_card_non_unique_matter()
        '''
