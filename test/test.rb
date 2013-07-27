#!/usr/bin/env ruby
require 'rubygems'
require 'ffi'
require 'yaml'


# Load shared OCR library
module LibOCR
  extend FFI::Library
  ffi_lib "/home/sdimkov/Projects/RubyLCE/OCR/lib.so"
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
    puts "#{img} FAILED!!! result=#{result}"
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
LibOCR.ocr_add_language $ocr, '/home/sdimkov/Projects/RubyLCE/OCR/lang/lce-receipt/'
LibOCR.ocr_add_language $ocr, '/home/sdimkov/Projects/RubyLCE/OCR/lang/lce-prompt/'
LibOCR.ocr_add_language $ocr, '/home/sdimkov/Projects/RubyLCE/OCR/lang/lce-msg/'
LibOCR.ocr_add_language $ocr, '/home/sdimkov/Projects/RubyLCE/OCR/lang/lce-menu/'
LibOCR.ocr_add_language $ocr, '/home/sdimkov/Projects/RubyLCE/OCR/lang/lce-led/'

# Run tests on all provided locations
ARGV.each do |test_folder|
  if File.exist? "#{test_folder}/test.yaml"
    $pass, $fail = 0,0
    test = YAML.load File.open "#{test_folder}/test.yaml"
    LibOCR.ocr_remove_all_colors $ocr
    test['COLORS'].each { |r, g, b| LibOCR.ocr_add_color $ocr, r, g, b }
    process_folder test['IMAGES'], "#{test_folder}/img/"
    puts "#{test['NAME']}: #{"%.2f" % (($pass*100.00)/($pass+$fail))}% passed."
  end
end