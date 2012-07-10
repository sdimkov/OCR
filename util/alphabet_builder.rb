require File.expand_path File.dirname(__FILE__) + '/lib/OCR'

all_chars = []
new_chars_folder = 'new_chars'

ARGV.each do |png_file|
	new_chars_count = 0
	begin
		img = OCR::Image.from_file png_file
	rescue
		puts "Unable to open file \"#{png_file}\"."
	end
	if img
		img.filter_color
		img.trim
		img.split(1).each do |ch|
			unless all_chars.include? ch
				all_chars << ch
				new_chars_count += 1
			end
		end
		puts "#{new_chars_count} new chars fould in file \"#{png_file}\"."
	end
end
puts 'PNG files processing completed.'

i = 0
Dir.mkdir new_chars_folder unless File.directory? new_chars_folder
all_chars.each do |ch|
	i += 1
	ch.save "#{new_chars_folder}/#{i}.png"
end
puts "Saved #{i} chars in folder \"#{new_chars_folder}\"."
