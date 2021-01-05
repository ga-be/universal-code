#!/usr/bin/env ruby

require_relative 'parser'
require_relative 'coding'
require_relative 'utilities'

def build_header(parameters)
  code_flag = parameters[:code_type] == 1 ? '0' : '1'
  # To encode our word size in 4 bits, we subtract 1, so that 1111 denote length of 16 and so on
  word_flag = format('%04d', (parameters[:word_size] - 1).to_s(2))

  code_flag << word_flag
end

def encode_block(block, word_size, position_table, coding)
  encoded = ''
  current_position = position_table[:current_position]
  data = unpack_data(block)

  code = coding.code

  data.scan(/\d{#{word_size}}/) do |word|
    word = word.to_sym
    # Subtract 1 to normalize gap for our code table, which starts from 0
    gap = (current_position - position_table[word]) - 1

    coding.add_entry(gap) if code[gap].nil?

    encoded << code[gap]
    position_table[word] = current_position
    current_position += 1
  end

  position_table[:current_position] = current_position
  encoded
end

def encode(parameters)
  start_time = Time.now

  word_size = parameters[:word_size]
  in_name = parameters[:file_name]
  out_name = "#{in_name}.elias"

  coding = if parameters[:code_type] == 1
             GammaCode.new(word_size)
           else
             DeltaCode.new(word_size)
           end

  out_file = File.open(out_name, 'w')
  encoded = build_header(parameters)

  position_table = position_table(word_size)

  read_block(in_name, word_size) do |block|
    encoded << encode_block(block, word_size, position_table, coding)

    # Guard statement if encoded block is not a multiple of 8
    next if encoded.size % 8 != 0

    write_block(out_file, encoded)
    encoded = ''
  end

  # If we finished reading and encoding blocks and still have unwritten output,
  # it means it's not a multiple of 8 and we need to add padding
  unless encoded.empty?
    encoded = add_padding(encoded)
    write_block(out_file, encoded)
  end

  end_time = Time.now

  print_compression_info(in_name, out_name, end_time - start_time)
end

parameters = parse_args
encode(parameters)
