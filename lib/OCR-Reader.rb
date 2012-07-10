require 'yaml'

module OCR

  class Reader
    def initialize lang_path
      lang = YAML.load File.open(lang_path + '/lang.yaml')
      @lang_name = lang['name']
      @min_space = lang['min_space']
      @min_accuracy = lang['min_accuracy']
      @chars = []
      for group in lang['chars'].keys
        for char in lang['chars'][group]
          new_char = Image.from_file(
            "#{lang_path}/chars/#{group}/#{(char['file'] || "#{char['name']}.png")}")
          new_char.text = char['name'].to_s
          new_char.vert_offset = (char['vert_offset'] || 0)
          @chars << new_char
        end
      end
    end
    
    def read arg
      if arg.is_a? String
        read_text(Image.from_file arg)
      elsif arg.is_a? OCR::Image
        read_text(arg)
      else
        ## HOW DO WE HANDLE ERRORS?
      end
    end
    
    private
    def read_text image
      image.text = ""
      image.filter_color
      image.trim
      words = image.split @min_space

      for word in words do
        text = read_word(word)
        return nil unless text
        image.text += text + " " 
      end
      
      return image.text.chop!
    end
  
    def merge char, word, left_offset = 0
      wrong_pixels = 0
      vert_offset = char.vert_offset - word.vert_offset
      unless (word.dimension.width - left_offset < char.dimension.width or
        word.dimension.height + word.vert_offset < char.dimension.height + char.vert_offset or
        word.vert_offset > char.vert_offset)
      then
        for x in 0..char.dimension.width-1
          wd_x = x + left_offset
          ch_y = char.dimension.height + vert_offset - word.dimension.height
          for y in 0..word.dimension.height
            wrong_pixels += 1 if char[x,ch_y] != word[wd_x,y]
            ch_y += 1
          end
        end
      else
        wrong_pixels = char.area
      end
      rez = ( (1 - ( wrong_pixels.to_f / char.area ) ) * 100)
	  #if rez >= MIN_ACCURACY
	  #  print char.text
	  #end
      return ( (1 - ( wrong_pixels.to_f / char.area ) ) * 100)
    end
  
    def read_word word
      wd_corner = word.corner
      results = []
      for char in @chars
        word.vert_offset = char.vert_offset + char.corner - wd_corner
        accuracy = merge char, word
        if accuracy >= @min_accuracy
          results << { :char => char, :accuracy => accuracy }
        end
      end
      return nil if results.empty?
      results.sort! {|a,b| b[:char].dimension.width <=> a[:char].dimension.width}
      result = results.first
      word.vert_offset = result[:char].vert_offset + result[:char].corner - wd_corner
      word.text = result[:char].text
      left_offset = result[:char].dimension.width
      
      while left_offset < word.dimension.width
        while word.empty_column? left_offset
          left_offset += 1
          return if left_offset == word.dimension.width
        end
        results = []
        for char in @chars
          accuracy = merge char, word, left_offset
          if accuracy >= @min_accuracy
            results << { :char => char, :accuracy => accuracy }
          end
        end
        if results.empty?
          left_offset -= 1
          for char in @chars
            accuracy = merge char, word, left_offset
            if accuracy >= @min_accuracy
              results << { :char => char, :accuracy => accuracy }
            end
          end
        end
        return nil if results.empty?
        results.sort! {|a,b| b[:char].dimension.width <=> a[:char].dimension.width}
        result = results.first
        word.text += result[:char].text
        left_offset += result[:char].dimension.width
      end
      
      return word.text
    end
    
  end
end
