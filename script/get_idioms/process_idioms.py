#!/usr/bin/env python3
# -*- coding:utf-8 -*-
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
"""
Author : xuechengyun
E-mail : xuechengyun@gmail.com
Date   : 2025/01/10 22:50:28
Desc   :
"""

import sys
import os
import logging

CUR_DIR = os.path.dirname(os.path.abspath(__file__))
LOG_FORMAT = '''[%(levelname)s] [%(asctime)s] [%(threadName)s] [%(name)s] '''
LOG_FORMAT += '''[%(filename)s:%(funcName)s:%(lineno)d]: %(message)s'''
logging.basicConfig(
    stream=sys.stderr,
    level=logging.INFO,
    format=LOG_FORMAT
)

import re

def main():
    results = list()
    for line in sys.stdin:
        line = line.strip()
        parts = line.split('. ')
        number = parts[0]
        text = parts[1]
        text = re.sub(r'\(.*\)', '', text)
        todo_part = 'TODO'
        if todo_part in text:
            text = text.replace(todo_part, '').strip()  # 移除 TODO
        else:
            todo_part = ''
        text = text.lower().strip()
        text = text.replace(' ', '_').replace('-', '_')
        result = [text, number]
        if todo_part != '':
            result.append(todo_part)
        #result = f"{text} {todo_part} {number}".strip()
        results.append("\t".join(result))
    results.sort()
    for elem in results:
        print(elem)

if __name__ == '__main__':
    main()


