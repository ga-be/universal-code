#!/usr/bin/env ruby

require_relative 'parser.rb'


def build_header(code_type, word_size)
  code_flag = code_type == 1 ? '0' : '1'
  # To encode our word size in 4 bits, we subtract 1, so that 1111 denote length of 16 and so on
  word_flag = "%04d" % (word_size - 1).to_s(2)

  code_flag + word_flag
end

parameters = parse_args
puts build_header(parameters[:type], parameters[:word_size])

