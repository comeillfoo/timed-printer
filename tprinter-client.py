#!/usr/bin/python3
from errno import EINVAL, ENOENT
from typing import List
from sys import argv, stderr
from os.path import exists


module = {
    'name':             'timed_printer',
    'common-file-path': 'common_file_path',
    'timeout':          'secs'
}

module_path = f'/sys/module/{ module[ "name" ] }'

def is_int( s ):
    try:
        int( s )
        return True
    except ValueError:
        return False


def set_parameters( common_file_path: str, timeout: int ):
    common_file_sys_path = f'{ module_path }/parameters/{ module[ "common-file-path" ] }'
    with open( common_file_sys_path, 'w' ) as file:
        file.write( common_file_path )

    timeout_sys_path = f'{ module_path }/parameters/{ module[ "timeout" ] }'
    with open( timeout_sys_path, 'w' ) as file:
        file.write( str( timeout ) )


def main( argc: int, argv: List ):
    if not exists( module_path ):
        print( f'Module { module[ "name" ] } not loaded', out = stderr )
        return -ENOENT

    new_common_file_path = None
    new_timeout = None
    if argc == 3:
        _, new_common_file_path, new_timeout = argv

        if not is_int( new_timeout ):
            print( f'timeout is not an integer [ { new_timeout } ]', out = stderr )
            return -EINVAL
        
        new_timeout = int( new_timeout )
        if new_timeout <= 0:
            print( f'timeout is not a positive value [ { new_timeout } ]', out = stderr )
            raise -EINVAL
    else:
        new_common_file_path = input( 'Enter the new common file path: ' )
        
        is_prompt = True
        new_timeout = 1
        while ( is_prompt ):
            try:
                new_timeout = int( input( 'Enter the new timeout (positive integer): ') )
                is_prompt = False
            except ValueError:
                continue

    set_parameters( new_common_file_path, new_timeout )
    
        


if __name__ == '__main__':
    err = 0
    try:
        err = main( len( argv ), argv )
    except KeyboardInterrupt:
        pass
    finally:
        exit( err )