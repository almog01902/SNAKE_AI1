
class EntropyScheduler:
    def __init__(self, start_ent, end_ent, start_episode, duration):
        self.start_ent = start_ent
        self.end_ent = end_ent
        self.start_episode = start_episode
        self.duration = duration

    def get_ent_coeff(self, current_episode):

        if current_episode < self.start_episode:
            return self.start_ent
        
        # חישוב כמה התקדמנו בתוך טווח הדעיכה (מ-0 עד 1)
        progress = (current_episode - self.start_episode) / self.duration
        progress = min(1.0, progress)  # מוודא שלא נעבור את ה-100%
        
        # חישוב הערך החדש
        new_ent = self.start_ent - (self.start_ent - self.end_ent) * progress
        
        return max(self.end_ent, new_ent)