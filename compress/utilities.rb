def pack_data(data)
  data_arr = []
  data.scan(/\d{8}/) { |byte| data_arr << byte.to_i(2) }
  data_arr.pack('C*')
end

def unpack_data(data)
  data.unpack1('B*')
end

def read_block(file_name, word_size)
  # This ensures that block read is always divisible by the word size
  # so that we can encode and don't worry that a few bits will be left
  # hanging and is also cleanly divisible by 8 => could be read from a file as a byte block
  block_size = 4096
  remainder = ((block_size * 8) % (word_size * 64)) / 8
  block_size -= remainder

  File.open(file_name) do |file|
    until file.eof?
      block = file.read(block_size)
      yield block
    end
  end
end

def write_block(file, block)
  block = pack_data(block)
  file.write(block)
end

def add_padding(buffer)
  padding = 8 - (buffer.size % 8)
  buffer << '0' * padding
end

def print_compression_info(in_name, out_name, seconds)
  # Compression ratio formatting referenced from: https://github.com/vikram7/vkrunch
  compression_ratio =  format('%.1f', (1 - File.size(out_name) / File.size(in_name).to_f) * 100)
  puts "Input file: #{in_name}"
  puts "Output file: #{out_name}"
  puts "Compression ratio: #{compression_ratio} %"
  puts "Compression took " + format('%.4f', seconds.to_s) + " seconds"
end
