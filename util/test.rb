#!/usr/bin/env ruby
$LOAD_PATH << '/home/sdimkov/Projects/OCR/lib'

require 'OCR'

pos1 = OCR::Reader.new 'lang/pos1'
pos2 = OCR::Reader.new 'lang/pos2'

for pic in ARGV do
  text = pos1.read pic
  if text
    puts "[pos1] #{pic}  ==>  #{text}"
  else
    text = pos2.read pic
    if text
      puts "[pos2] #{pic}  ==>  #{text}"
    else
      puts "#{pic} : FAIL"
    end
  end
end
