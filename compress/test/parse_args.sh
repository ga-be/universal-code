#!/usr/bin/env sh


# Expected usage message to be printed out
../compress.rb
../compress.rb -c1 -c2 -b 15 20b.file
../compress.rb --help

# Expected invalid code parameter error message
../compress.rb -c -b 16 20b.file
../compress.rb -c-1 -b 12 20b.file
../compress.rb -c3 -b 25 20b.file

# Expected invalid bits parameter error message
../compress.rb -c1 -b -5 20b.file
../compress.rb -c1 -b number 20b.file
../compress.rb -c1 -b 20b.file

# Expected invalid file format error
../compress.rb -c1 -b 5 bad_format
../compress.rb -c2 -b 12 badformat
../compress.rb -c1 -b 24 bad-format
../compress.rb -c1 -b 2 bad_format.

# Expected file doesn't exist error
../compress.rb -c1 -b 12 25b.file
../compress.rb -c2 -b 24 1gb.file

# Expected no error and right values
../compress.rb -c2 -b 15 20b.file # expected 2 15 20b.file 20
../compress.rb -b 3 1kb.file # expected 1 3 1kb.file 1024
../compress.rb 1mb.file # expected 1 8 1mb.file 1048576

