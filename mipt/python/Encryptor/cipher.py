# for checking version
import sys
import re
import json
# python 2 has some problems with json
import ascii_json
from collections import Counter
import string


class Caesar(object):
    """ Ordinary Caesar encryption.
     A..Z, a..z -> A..Z, a..z """

    _alphabet_size = 26

    @staticmethod
    def _get_shifted_letter(letter, shift):
        if letter.isupper():
            return chr((ord(letter) - ord('A') + shift) % Caesar._alphabet_size + ord('A'))
        else:
            return chr((ord(letter) - ord('a') + shift) % Caesar._alphabet_size + ord('a'))

    @staticmethod
    def encrypt(text, key):
        if type(text) != str:
            raise TypeError("type of text must be \'str\'")
        if type(key) != int:
            raise TypeError("type of key must be \'int\'")
        _alphabet_size = Caesar._alphabet_size

        encrypted_text = []
        for letter in text:
            if letter.isalpha():
                encrypted_text.append(Caesar._get_shifted_letter(letter, key))
            else:
                encrypted_text.append(letter)

        return ''.join(encrypted_text)

    @staticmethod
    def decrypt(text, key):
        return Caesar.encrypt(text, -key)

    letter_frequency = dict()
    letter_count = 0
    default_power = 1
    frequencies_are_updated = False

    @staticmethod
    def get_frequency(text):
        letter_frequencies = Counter(text.lower())
        letter_count = 0
        elements_to_pop = []

        for letter in letter_frequencies.keys():
            if letter in string.ascii_lowercase:
                letter_count += letter_frequencies.get(letter, 0)
            else:
                elements_to_pop += [letter]

        for element in elements_to_pop:
            letter_frequencies.pop(element)

        return letter_frequencies, letter_count

    @staticmethod
    def set_frequency(text, model="letter_frequency.json", power=1):
        if type(text) != str:
            raise TypeError("string expected")
        if type(power) != int:
            raise TypeError("power must be int")

        letter_frequencies, letter_count = Caesar.get_frequency(text)

        file_of_frequencies = open(model, 'w')

        data = dict()
        data["letter_frequencies"] = letter_frequencies
        data["count"] = letter_count
        data["power"] = power
        json.dump([data], file_of_frequencies, ensure_ascii=True)

        file_of_frequencies.close()

        Caesar.frequencies_are_updated = False

    @staticmethod
    def open_frequencies(model="letter_frequency.json"):
        file_of_frequencies = open(model, 'r')

        if sys.version[0] == '2':
            data = ascii_json.loads_ascii(file_of_frequencies.read())
        else:
            data = json.loads(file_of_frequencies.read())

        Caesar.letter_frequency = data[0]["letter_frequencies"]
        Caesar.letter_count = data[0]["count"]
        Caesar.default_power = data[0]["power"]

        file_of_frequencies.close()

        Caesar.frequencies_are_updated = True

    @staticmethod
    def _count_deviation(check_frequency, letter_count, power, shift=0):
        if not isinstance(check_frequency, dict):
            raise TypeError("First argument must be dict")
        if type(power) != int:
            raise TypeError("power must be int")

        if not Caesar.frequencies_are_updated:
            Caesar.open_frequencies()

        deviation = 0
        for letter, count in Caesar.letter_frequency.items():
            deviation += (abs(float(count) / Caesar.letter_count -
                              float(check_frequency.get(Caesar._get_shifted_letter(letter, -shift), 0)) /
                              letter_count)) ** power

        return deviation

    @staticmethod
    def hack(text, model="letter_frequency.json", power=default_power):
        if type(power) != int:
            raise TypeError("power must be int")
        _alphabet_size = Caesar._alphabet_size

        Caesar.open_frequencies(model)

        best_deviation = -1
        best_shift = 0

        frequencies, letter_count = Caesar.get_frequency(text)

        for shift in range(_alphabet_size):
            # Updating best shift if less deviation found
            cur_deviation = Caesar._count_deviation(frequencies, letter_count, power, shift)
            if best_deviation == -1 or cur_deviation < best_deviation:
                best_deviation = cur_deviation
                best_shift = shift

        return Caesar.encrypt(text, best_shift)


class Vigenere(object):
    """ Ordinary Vigenere encryption.
    A..Z, a..z -> A..Z, a..z
    key is not case sensitive """

    _alphabet_size = 26

    @staticmethod
    def _get_shifted_letter(letter, shift):
        if letter.isupper():
            return chr((ord(letter) - ord('A') + shift) % Vigenere._alphabet_size + ord('A'))
        else:
            return chr((ord(letter) - ord('a') + shift) % Vigenere._alphabet_size + ord('a'))

    @staticmethod
    def _get_shift(letter):
        if type(letter) != str:
            raise TypeError("letter must be str")
        if len(letter) != 1:
            raise ValueError("letter must be of length 1")

        if letter.isupper():
            return ord(letter) - ord('A')
        else:
            return ord(letter) - ord('a')

    @staticmethod
    def encrypt(text, key):
        if type(text) != str:
            raise TypeError("type of text must be \'str\'")
        if type(key) != str:
            raise TypeError("type of key must be \'str\'")
        key_str = re.match("[A-Za-z]+", key)
        if key_str is None or key_str.group(0) != key:
            raise ValueError("key must be a string of alphas")
        _alphabet_size = Vigenere._alphabet_size

        encrypted_text = []
        key_shifts = [Vigenere._get_shift(key_letter) for key_letter in key]

        for index, letter in enumerate(text):
            if letter.isalpha():
                encrypted_text.append(Vigenere._get_shifted_letter(letter, key_shifts[index % len(key_shifts)]))
            else:
                encrypted_text.append(letter)
        return ''.join(encrypted_text)

    @staticmethod
    def decrypt(text, key):
        if type(text) != str:
            raise TypeError("type of text must be \'str\'")
        if type(key) != str:
            raise TypeError("type of key must be \'str\'")
        key_str = re.match("[A-Za-z]+", key)
        if key_str is None or key_str.group(0) != key:
            raise ValueError("key must be a string of alphas")

        key_shifts = [(-Vigenere._get_shift(key_letter)) % Vigenere._alphabet_size for key_letter in key]
        new_key = ""
        for letter_ord in key_shifts:
            new_key += chr(letter_ord + ord('a'))
        return Vigenere.encrypt(text, new_key)


class Vernam(object):
    """ Vernam encryption.
    Encrypting symbols with ascii codes 63..126 """

    _lower_border = 63
    _interval = 64
    _upper_border = _lower_border + _interval

    @staticmethod
    def encrypt(text, key):
        if type(text) != str:
            raise TypeError("type of text must be \'str\'")
        if type(key) != str:
            raise TypeError("type of key must be \'str\'")
        key_str = re.match("[?-~]+", key)
        if key_str is None or key_str.group(0) != key:
            raise ValueError("key must be a string of symbols with ascii code from [63, 126]")
        _lower_border = Vernam._lower_border
        _upper_border = Vernam._upper_border
        _interval = Vernam._interval

        encrypted_text = []

        for index, letter in enumerate(text):
            if _lower_border <= ord(letter) <= _upper_border:
                new_letter = (ord(letter) ^ ord(key[index % len(key)])) % _interval
                if new_letter < _lower_border:
                    encrypted_text.append(chr(_interval + new_letter))
                else:
                    encrypted_text.append(chr(new_letter))
            else:
                encrypted_text.append(letter)

        return ''.join(encrypted_text)

    @staticmethod
    def decrypt(text, key):
        return Vernam.encrypt(text, key)
