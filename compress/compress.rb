#!/usr/bin/env ruby

require_relative 'parser.rb'


def build_header(code_type, word_size)
  code_flag = code_type == 1 ? '0' : '1'
  # To encode our word size in 4 bits, we subtract 1, so that 1111 denote length of 16 and so on
  word_flag = "%04d" % (word_size - 1).to_s(2)

  code_flag << word_flag
end

def pack_data(data)
  data.pack("C*")
end

def unpack_data(data)
  data.unpack("B*")[0]
end

def read_block(file)
  File.open(file) do |file|
    until file.eof?
      block = file.read(1048576)
      yield block
    end
  end
end

def encode_block(block, word_size, position_table, gamma_code)
  encoded = ''
  # Current position is a pointer to a position of a word being encoded, in
  # regards to a position table.
  current_position = position_table.size

  data = unpack_data(block)
  remainder = data.size % word_size
  if remainder != 0
    padding = word_size - remainder
    data << ('0' * padding)
  end

  max = 0

  data.scan(/\d{#{word_size}}/) do |word|
    word = word.to_sym    
    gap = (current_position - position_table[word])
    if gap > max
      max = gap
      p max
    end
    encoded << gamma_code[gap]
    position_table[word] = current_position
    current_position += 1
  end

  p data.size
  p encoded.size
end

def encode(file, word_size)
  position_table = position_table(word_size)
  gamma_code = elias_gamma_code

  
  read_block(file) do |block|
    encode_block(block, word_size, position_table, gamma_code)
  end
end

def position_table(word_size)
  table = {}
  position = 0
  
  ['0', '1'].repeated_permutation(word_size) do |word|
    table[word.join.to_sym] = position
    position += 1
  end
  
  table
end

def elias_gamma_code
  # Blank spot at 0th index,
  # as our code indexes start from 1
  code = [nil]
  
  (1..4096).each do |k|
    zeroes = '0' * Math.log(k, 2).floor
    code[k] = zeroes + k.to_s(2)
  end

  code
end

parameters = parse_args
encode(parameters[:file], parameters[:word_size])

# build_header(parameters[:type], parameters[:word_size])

