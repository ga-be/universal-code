module EliasCode
  attr_accessor :code

  def construct_code(word_size)
    code = {}

    initial = 2**word_size - 1

    (0..initial).each do |k|
      code[k] = code_entry(k)
    end

    @code = code
  end

  def add_entry(k)
    @code[k] = code_entry(k)
  end
end

class GammaCode
  include EliasCode

  def initialize(word_size)
    construct_code(word_size)
  end

  def code_entry(k)
    zeroes = '0' * Math.log(k + 1, 2).floor
    zeroes << (k + 1).to_s(2)
  end
end

class DeltaCode
  include EliasCode

  def initialize(word_size)
    @gamma = GammaCode.new(word_size)
    construct_code(word_size)
  end

  def code_entry(k)
    m_k = Math.log(k + 1, 2).floor
    v_prime = @gamma.code_entry(m_k) + (k + 1).to_s(2)
    v_prime[-(m_k + 1)] = ''
    v_prime
  end
end

def position_table(word_size)
  table = {}
  position = 0

  %w[0 1].repeated_permutation(word_size) do |word|
    table[word.join.to_sym] = position
    position += 1
  end

  table[:current_position] = position

  table
end

