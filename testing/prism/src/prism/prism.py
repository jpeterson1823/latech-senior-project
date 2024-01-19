class Prism:
    def __init__(self, id, priority):
        self.id = id
        self.priority = priority
        self.ap_ssid = f'prism{id}'
    
    @property
    def id(self):
        return self.id
    @property
    def priority(self):
        return self.priority