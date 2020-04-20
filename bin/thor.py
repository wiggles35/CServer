#!/usr/bin/env python3

import concurrent.futures
import os
import requests
import sys
import time

# Functions

def usage(status=0):
    progname = os.path.basename(sys.argv[0])
    print(f'''Usage: {progname} [-h HAMMERS -t THROWS] URL
    -h  HAMMERS     Number of hammers to utilize (1)
    -t  THROWS      Number of throws per hammer  (1)
    -v              Display verbose output
    ''')
    sys.exit(status)

def hammer(url, throws, verbose, hid):
    ''' Hammer specified url by making multiple throws (ie. HTTP requests).

    - url:      URL to request
    - throws:   How many times to make the request
    - verbose:  Whether or not to display the text of the response
    - hid:      Unique hammer identifier

    Return the average elapsed time of all the throws.
    '''
    headers = {'user-agent': 'testing-{}'.format(os.environ.get('USER', 'cse-20289-sp20'))}
    time_list = []
    for i in range(throws):
        start_time = time.time()
        request = requests.get(url, headers=headers)
        if verbose:
            print(request.text)
        time_list.append(time.time()-start_time)
        print(f'Hammer: {hid:>2}, Throw: {i:>5}, Elapsed Time: {time_list[len(time_list)-1]:.2f}')
    average = sum(time_list)/throws
    print(f'Hammer: {hid:>2}, AVERAGE:\t, Elapsed Time: {average:.2f}')

    return average

def do_hammer(args):
    ''' Use args tuple to call `hammer`'''
    #print(args)
    return hammer(*args)

def main():
    hammers = 1
    throws  = 1
    verbose = False


    # Parse command line arguments
    arguments = sys.argv[1:]
    while len(arguments) and arguments[0].startswith('-'):
        arg = arguments.pop(0)
        if arg == '-h':
            hammers = int(arguments.pop(0))
        elif arg == '-t':
            throws = int(arguments.pop(0))
        elif arg == '-v':
            verbose = True
        else:
            usage(1)
    try:
        url = arguments.pop(0)
    except IndexError:
        usage(1)

    args = [(url, throws, verbose, x) for x in range(hammers)]
    average_list = []
    # Create pool of workers and perform throws
    with concurrent.futures.ProcessPoolExecutor(hammers) as executor:
        average_list = list(executor.map(do_hammer, args))
    average = sum(average_list)/hammers
    print(f'TOTAL AVERAGE ELAPSED TIME: {average:.2f}')


# Main execution

if __name__ == '__main__':
    main()

# vim: set sts=4 sw=4 ts=8 expandtab ft=python:
