#!/usr/bin/python
import argparse
import sys
from cipher import Caesar, Vigenere, Vernam


def add_encode(subs):
    encoding_parser = subs.add_parser("encode", description="Encode the text with key")

    encoding_parser.set_defaults(method="encode")

    encoding_parser.add_argument("--cipher", type=str, help="caesar, vigenere or vernam")
    encoding_parser.add_argument("--key", required=True, type=str, help="Key for encoding")
    encoding_parser.add_argument("--input-file", type=str, help="File to encode. If not given stdin will be encoded")
    encoding_parser.add_argument("--output-file", type=str,
                                 help="File to put encoded text. If not given stdout will be used")


def add_decode(subs):
    decoding_parser = subs.add_parser("decode", description='Decode the text with key')

    decoding_parser.set_defaults(method="decode")

    decoding_parser.add_argument("--cipher", type=str, help="caesar, vigenere or vernam")
    decoding_parser.add_argument("--key", required=True, type=str, help="Key for decoding")
    decoding_parser.add_argument("--input-file", type=str, help="File to decode. If not given stdin will be used")
    decoding_parser.add_argument("--output-file", type=str,
                                 help="File to put decoded text. If not given stdout will be used")


def add_hack(subs):
    hack_parser = subs.add_parser("hack", description='Hack the text without key. (only for Caesar encryption)')

    hack_parser.set_defaults(method="hack")

    hack_parser.add_argument("-p", type=int, help="Power to use in deviation")
    hack_parser.add_argument("--input-file", type=str, help="File to hack. If not given stdin will be used")
    hack_parser.add_argument("--output-file", type=str,
                              help="File to put hacked text. If not given stdout will be used")
    hack_parser.add_argument("--model-file", type=str, help="Model file to hack")


def add_train(subs):
    train_parser = subs.add_parser("train",
                                   description='Training to hack the text without key. (only for Caesar encryption)')

    train_parser.set_defaults(method="train")

    train_parser.add_argument("-p", type=int, help="Default power to use")
    train_parser.add_argument("--text-file", type=str,
                              help="File for training to hack. If not given stdin will be used")
    train_parser.add_argument("--model-file", type=str, help="Model file to hack")


# Checking if var can be casted to int
def is_int(var):
    try:
        int(var)
        return True
    except:
        return False


# Getting dict of arguments for hack and train
def get_kwargs(given_args):
    kwargs = {}
    if given_args.p is not None:
        kwargs["power"] = given_args.p
    if given_args.model_file is not None:
        kwargs["model"] = given_args.model_file
    return kwargs


def write(given_args, text):
    if given_args.output_file is not None:
        with open(given_args.output_file, 'w') as output_f:
            output_f.write(text)
    else:
        print(text)


def read(given_args):
    text = ""
    is_read = False
    try:
        if given_args.input_file is not None:
            with open(given_args.input_file, 'r') as input_f:
                text += input_f.read()
            is_read = True
    except AttributeError:
        pass

    try:
        if not is_read and given_args.text_file is not None:
            with open(given_args.text_file, 'r') as input_f:
                text += input_f.read()
            is_read = True
    except AttributeError:
        pass

    if not is_read:
        for line in sys.stdin:
            text += line
    return text


def get_args():
    parser = argparse.ArgumentParser()
    subs = parser.add_subparsers()
    add_encode(subs)
    add_decode(subs)
    add_hack(subs)
    add_train(subs)
    return parser.parse_args()


def main():
    args = get_args()

    if args.method == "encode" or args.method == "decode":
        ciphers = ["caesar", "vigenere", "vernam", None]
        if args.cipher not in ciphers:
            raise ValueError("Waited for caesar, vigenere or vernam")

        text = read(args)

        # Printing encoded/decoded text
        if args.cipher == "caesar" or is_int(args.key):
            if args.method == 'encode':
                write(args, Caesar.encrypt(text, int(args.key)))
            else:
                write(args, Caesar.decrypt(text, int(args.key)))
        elif args.cipher == "vernam":
            if args.method == 'encode':
                write(args, Vernam.encrypt(text, args.key))
            else:
                write(args, Vernam.decrypt(text, args.key))
        else:
            if args.method == "encode":
                write(args, Vigenere.encrypt(text, args.key))
            else:
                write(args, Vigenere.decrypt(text, args.key))

    elif args.method == "hack":
        text = read(args)

        # Printing hacked text
        write(args, Caesar.hack(text, **get_kwargs(args)))

    elif args.method == "train":
        text = read(args)

        Caesar.set_frequency(text, **get_kwargs(args))

    else:
        print("No such methods found")


if __name__ == '__main__':
    main()
