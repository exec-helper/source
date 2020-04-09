class Pattern:
    def __init__(self, id, default_values, long_options):
        self._id = id
        self._defaults = default_values
        self._long_options = long_options
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

    @property
    def long_options(self):
        return self._long_options
