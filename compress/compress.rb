#!/usr/bin/env ruby

require_relative 'parser.rb'


def build_header(code_type, file_size, word_size)
  code_flag = code_type == 1 ? '0' : '1'
  padding = "%04d" % (file_size % word_size).to_s(2)

  code_flag + padding
end

parameters = parse_args
puts build_header(parameters[:type], parameters[:file_size], parameters[:word_size])

