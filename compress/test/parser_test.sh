#!/usr/bin/env sh

test() {
    eval $1 | grep -q "$2"

    if [ $? -eq 0 ]; then
	echo '\033[0;32mPassed'
    else
	echo '\033[0;31mFailed'
    fi
}

# Expected usage message to be printed out
test '../compress.rb' 'Usage:'
test '../compress.rb -c1 -c2 -b 15 20b.file' 'Usage:'
test '../compress.rb --help' 'Usage:'

# Expected invalid code parameter error message
test '../compress.rb -c -b 16 20b.file' "Error: code parameter"
test '../compress.rb -c-1 -b 12 20b.file' "Error: code parameter"
test '../compress.rb -c3 -b 3 20b.file' "Error: code parameter"

# Expected invalid bits parameter error message
test '../compress.rb -c1 -b 0 20b.file' "Error: bits parameter"
test '../compress.rb -c1 -b -5 20b.file' "Error: bits parameter"
test '../compress.rb -c1 -b 17 20b.file' "Error: bits parameter"
test '../compress.rb -c1 -b number 20b.file' "Error: bits parameter" 
test '../compress.rb -c1 -b 20b.file' "Error: bits parameter"

# Expected file doesn't exist error
test '../compress.rb -c1 -b 13 ../../bad.file' "Error: file doesn't exist!"
test '../compress.rb -c1 -b 2 ~/top.file' "Error: file doesn't exist!"
test '../compress.rb -c1 -b 12 25b.file' "Error: file doesn't exist!"
test '../compress.rb -c2 -b 11 1gb.file' "Error: file doesn't exist!"

# Expected no error and right values
test '../compress.rb -c2 -b 15 20b.file' ":word_size=>15"
test '../compress.rb -b 3 1kb.file' ":file_size=>1024"
test '../compress.rb 1mb.file' "{:type=>1"

