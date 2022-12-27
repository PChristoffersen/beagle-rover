#!/usr/bin/env python3

from argparse import ArgumentParser

from robotweb.__main__ import main

if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument("--port", help="Web server port", type=int, default=5000)
    args = parser.parse_args()

    main(args.port)
