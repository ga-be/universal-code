#!/usr/bin/env ruby

def check_for_help
  argc = ARGV.length
  help_message = 'Usage: compress.rb [-c1|-c2] [-b bits] file'
  
  if argc == 0 || argc > 4
    puts help_message
    exit(1)
  end

  if ARGV[0] == '--help'
    puts help_message
    exit(0)
  end  
end

def check_word_size(word_size, file_size)
    if word_size > file_size
      word_size = file_size
    elsif word_size <= 0 || word_size > 16
      word_size = 8
    end
end

def parse_code_type(args)
  if args[/-c(\d+)?/]
    code_type = get_code_type($1)
  else
    # If user hasn't specified code flag, return default (c1 or Elias gamma-code)
    code_type = 1
  end
end

def get_code_type(flag)
  code_type = flag.to_i

  return code_type unless (code_type == 0 || code_type >= 3)

  puts 'Error: code parameter is not a valid value!'
  exit(1)
end

def check_byte_count(count)
  if !/^\d+$/.match(count)
    puts 'Error: bits parameter must be a positive integer!'
    exit(1)
  end

  true
end

def parse_word_size(args)
  if args[/-b (\w+)?/]
    $1.to_i if check_byte_count($1)
  else
     8 # Default word size
  end
end

def parse_file(args)
  if args[/(\w+\.\w+)/]
    return File.open($1), File.size($1) if File.file?($1)

    puts "Error: file doesn't exist!"
    exit(1)
  else
    puts 'Error: invalid file name format!'
    exit(1)
  end
end

def parse_args
  check_for_help

  args = ARGV.join(' ')
  code_type = parse_code_type(args)
  word_size = parse_word_size(args)
  file, file_size = parse_file(args)
  
  check_word_size(word_size, file_size)

  return {type: code_type, word_size: word_size, file: file, file_size: file_size}
end


puts parse_args
