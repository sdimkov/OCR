#!/usr/bin/env ruby
$OCR_DIR = '/home/sdimkov/Projects/OCR'
$LOAD_PATH << $OCR_DIR + '/lib'
require 'OCR'
require 'yaml'

$reader = OCR::Reader.new $OCR_DIR + '/lang/pos2'
$tests = YAML.load File.open 'test.yaml'


def run_test entry, path
  img = "#{path}#{entry[0]}.png"
  result = $reader.read img
  if result == entry[1] then puts "#{img}"
  else puts "#{img} FAILED! result=#{result}" end
end

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

process_folder $tests, "./img/"



