import atexit
import datetime
import re
import os
from os import path

atexit.register(lambda: os.system('pause'))

VERSION = '0.1 alpha'

ROOT_DIR = path.normpath(path.join(path.dirname(__file__), '..'))
INCLUDE_DIR = path.join(ROOT_DIR, 'include')
DST_FILE_NAME = path.join(ROOT_DIR, 'single_include', 'json-cpp.hpp')

INCLUDE_RE = re.compile(r'#include\s*<(.*)>')

def get_include_path(line):
    return INCLUDE_RE.match(line).group(1)

g_IncludedFiles = set()
def process_file(name, dst):
    if name in g_IncludedFiles:
        return

    print('Processing', name)
    g_IncludedFiles.add(name)
    for line in open(path.join(INCLUDE_DIR, name), 'r'):
        if line.startswith('//'):
            continue
        
        if line.startswith('#pragma once'):
            continue

        if line.startswith('#include'):
            name = get_include_path(line)
            if name.startswith('json-cpp'):
                process_file(name, dst)
                continue
            
        dst.write(line)

def get_revision():
    return os.popen('git rev-parse HEAD').read().strip()

rev = get_revision()
print('current revision:', rev)

with open(DST_FILE_NAME, 'w') as dst:
    dst.write('''//
// DO NOT EDIT !!! This file was generated with a script.
//
// JSON for C++
// Version {ver}, rev. {rev}
// Generated {time} UTC
//
// Belongs to the public domain

#pragma once
    '''.format(rev = rev, ver = VERSION, time = datetime.datetime.utcnow()))
    process_file('json-cpp.hpp', dst)
