class User():
    def __init__(self, **kwargs):

        self.calendar = kwargs['cal']
        self.use_voice = kwargs['use_voice']
        self.audio_files_path = kwargs['audio_files_path']
        

    