import atexit
import datetime
import itertools
import re
import os
from os import path

atexit.register(lambda: os.system('pause'))

VERSION = '0.1 alpha'

HEADER = '''//
// DO NOT EDIT !!! This file was generated with a script.
//
// JSON for C++
// https://github.com/ascheglov/json-cpp
// Version {ver}, rev. {rev}
// Generated {time} UTC
//
// Belongs to the public domain

#pragma once

'''

ROOT_DIR = path.normpath(path.join(path.dirname(__file__), '..'))
INCLUDE_DIR = path.join(ROOT_DIR, 'include')
DST_FILE_NAME = path.join(ROOT_DIR, 'single_include', 'json-cpp.hpp')

INCLUDE_RE = re.compile(r'#include\s*<(.*)>')

def get_include_path(line):
    return INCLUDE_RE.match(line).group(1)

g_IncludedFiles = set()
def process_file(fileName, dst):
    if fileName in g_IncludedFiles:
        return

    g_IncludedFiles.add(fileName)
    print('Processing', fileName)

    dst.write('//' + '-' * 70 + '\n// ' + fileName + ' begin\n\n')
    prevLineWasEmpty = True

    with open(path.join(INCLUDE_DIR, fileName), 'r') as file:
        tail1 = itertools.dropwhile(lambda ln: not ln.startswith('#pragma once'), file)
        tail2 = itertools.islice(tail1, 1, None)
        
        for line in tail2:
            if line.startswith('#include'):
                name = get_include_path(line)
                if name.startswith('json-cpp'):
                    process_file(name, dst)
                    continue

            isEmptyLine = line.strip() == ''
            if isEmptyLine and prevLineWasEmpty:
                continue

            prevLineWasEmpty = isEmptyLine
            
            dst.write(line)

    if not prevLineWasEmpty:
        dst.write('\n')

    dst.write('// ' + fileName + ' end\n//' + '-' * 70 + '\n\n')

def get_revision():
    return os.popen('git rev-parse HEAD').read().strip()

rev = get_revision()
print('Current revision:', rev)

with open(DST_FILE_NAME, 'w') as dst:
    dst.write(HEADER.format(rev = rev, ver = VERSION, time = datetime.datetime.utcnow()))
    process_file('json-cpp.hpp', dst)

print('Testing...\n')
os.system('g++ -std=c++11 \
    -Wall -Werror -Wno-parentheses \
    -I../single_include \
    ../tests/tests_main.cpp \
    ../tests/examples.cpp \
    ../tests/variant_example.cpp \
    && a.exe')
print('\n-----\nDone.')
