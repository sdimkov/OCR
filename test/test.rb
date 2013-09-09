#!/usr/bin/env ruby
require 'rubygems'
require 'ffi'
require 'yaml'

LANG_PATH = ENV['OCR_TEST_LANG']
LIB_PATH  = ENV['OCR_TEST_LIB']

# Add text effects String class
class String
  def apply_code(code) "\e[#{code}m#{self}\e[0m" end
  def red()        apply_code(31)  end
  def green()      apply_code(32)  end
  def yellow()     apply_code(33)  end
end

# Load shared OCR library
module LibOCR
  extend FFI::Library
  ffi_lib LIB_PATH
  attach_function :ocr_create,            [],                                 :pointer
  attach_function :ocr_delete,            [:pointer],                         :void
  attach_function :ocr_get_window,        [:pointer],                         :int
  attach_function :ocr_set_window,        [:pointer, :int],                   :void
  attach_function :ocr_add_language,      [:pointer, :string],                :void
  attach_function :ocr_add_color,         [:pointer, :int, :int, :int],       :void
  attach_function :ocr_remove_color,      [:pointer, :int, :int, :int],       :void
  attach_function :ocr_remove_all_colors, [:pointer],                         :void
  attach_function :ocr_read_text,         [:pointer, :int, :int, :int, :int], :string
  attach_function :ocr_read_image,        [:pointer, :string],                :string

  attach_function :img_screenshot,      [:string,:int],                                      :void
  attach_function :img_screenshot_area, [:string,:int, :int, :int, :int, :int],              :void
  attach_function :img_draw_rect,       [:string, :int, :int, :int, :int, :int, :int, :int], :void
end

# Run a single test (PNG image) against the OCR engine
def run_test entry, path
  img = "#{path}#{entry[0]}.png"
  result = LibOCR.ocr_read_image $ocr, img
  if result == entry[1]
    $pass += 1
  else
    $fail += 1 
    puts "FAIL: #{img}   expected=\"#{entry[1]}\"   actual=\"#{result}\"".red
  end
end

# Recursively process all folder and sub-folder items
def process_folder folder, path
  for subfolder, entries in folder do
    for entry in entries do
      if entry.is_a? Hash 
        process_folder entry, path + subfolder + '/'
      elsif entry.is_a? Array
        run_test entry, path + subfolder + '/'
      else 
        raise "Invalid data in test's yaml file"
      end
    end
  end
end

# Create OCR object and load languages
$ocr = LibOCR.ocr_create
LibOCR.ocr_add_language $ocr, "#{LANG_PATH}/lce-receipt/"
LibOCR.ocr_add_language $ocr, "#{LANG_PATH}/lce-prompt/"
LibOCR.ocr_add_language $ocr, "#{LANG_PATH}/lce-msg/"
LibOCR.ocr_add_language $ocr, "#{LANG_PATH}/lce-menu/"
LibOCR.ocr_add_language $ocr, "#{LANG_PATH}/lce-led/"

# Run tests on all provided locations
ARGV.each do |test_folder|
  if File.exist? "#{test_folder}/test.yaml"
    $pass, $fail = 0,0
    test = YAML.load File.open "#{test_folder}/test.yaml"
    LibOCR.ocr_remove_all_colors $ocr
    test['COLORS'].each { |r, g, b| LibOCR.ocr_add_color $ocr, r, g, b }
    process_folder test['IMAGES'], "#{test_folder}/img/"
    pass_rate = ($pass * 100.00) / ($pass + $fail)
    result = "#{test['NAME']}:".ljust(27) + " #{"%.2f" % pass_rate}% passed.".ljust(16)
    puts case pass_rate
      when 100     then result.green
      when 70..100 then result.yellow
      else              result.red
    end
  end
end
