require File.expand_path File.dirname(__FILE__) + '/OCR-Image'
require File.expand_path File.dirname(__FILE__) + '/OCR-Scanner'

s = OCR::Scanner.new 'pos2'

if ARGV.empty?
  puts "Usage OCRDemo.exe textfile.png [another-textfile.png ..]"
else
  for pic in ARGV do
    puts "#{pic}  ==>  #{s.scan pic}"
  end
end
