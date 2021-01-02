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

def validate_word_size(word_size)
  return word_size if (2..16).include?(word_size)
  
  puts 'Error: bits parameter must be a positive integer in the range 2-16!'
  exit(1)
end

def parse_word_size(args)
  if args[/-b (\d+)?/]
    validate_word_size($1.to_i)
  else
    # If user hasn't specified bits flag, return default 
    8 
  end
end

def parse_file_name(file_name)
  return file_name, File.size(file_name) if File.file?(file_name)
  
  puts "Error: file doesn't exist!"
  exit(1)
end

def parse_args
  check_for_help

  args = ARGV.join(' ')
  code_type = parse_code_type(args)
  word_size = parse_word_size(args)
  file, file_size = parse_file_name(ARGV[-1])

  if word_size > file_size
    word_size = file_size
  end

  {type: code_type, word_size: word_size, file: file}
end

