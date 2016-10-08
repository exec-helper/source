class UniqueOrderedList:
    """ Class that makes sure the added values are unique, while preserving the order of the elements """
    def __init__(self, initial_list = []):
        self.sortedList = self.removeDuplicates(initial_list)

    def append(self, element):
        if self.count(element) == 0:
            self.sortedList.append(element)

    def extend(self, elements):
        for element in elements:
            self.append(element)

    def remove(self, element):
        if self.sortedList.count(element) > 0:
            self.sortedList.remove(element)

    def index(self, element):
        return self.sortedList.index(element)

    def count(self, element):
        return self.sortedList.count(element)

    def sort(self, cmp=None, key=None, reverse=False):
        self.sortedList.sort(cmp, key, reverse)
    
    def reverse(self):
        self.sortedList.reverse()

    def __iter__(self):
        """ Enables iteration over this object """
        # We simply let them iterate over the resulting list. No need to override the next() or __next__() operator this way
        return self.sortedList.__iter__()

    @staticmethod
    def removeDuplicates(duplicateList):
        if duplicateList is None:
            return None

        duplicateList.reverse()
        for item in duplicateList:
            while duplicateList.count(item) > 1:
                duplicateList.remove(item)

        duplicateList.reverse()
        return duplicateList
