class Pattern:
    def __init__(self, id, default_values):
        self._id = id
        self._defaults = default_values
        self._values = default_values

    @property
    def id(self):
        return self._id

    @property
    def default_values(self):
        return self._defaults

    @property
    def values(self):
        return self._values
