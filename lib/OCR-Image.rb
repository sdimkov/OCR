require 'chunky_png'


module OCR
 
  TEXT_PIXELS = [ 4294902015, 4278059263 ] 
  WHITE_PIXEL = ChunkyPNG::Color::WHITE
  BLACK_PIXEL = ChunkyPNG::Color::BLACK
  
  class Image < ChunkyPNG::Image
    attr_accessor :text, :vert_offset
    
    def initialize *args
      super
      @text = ""
      @vert_offset = 0
    end
    
    def display
      unless @empty
        for y in 0..dimension.height-1 do
          for x in 0..dimension.width-1 do
            if get_pixel(x,y) == BLACK_PIXEL
              print "@"
            else
              print "."
            end
          end
          puts ""
        end
      end
    end
    
    def trim
      x = 0
      while x < dimension.width and empty_column? x
        x = x + 1
      end
      
      if x == dimension.width
        @empty = true
        return 0
      end

      y = 0
      while empty_row? y
        y = y + 1
      end
      
      width = dimension.width - 1
      while empty_column? width
        width = width - 1
      end
      
      height = dimension.height - 1
      while empty_row? height
        height = height - 1
      end
      
      crop! x, y, width-x+1, height-y+1
      
      return area
    end
    
    def filter_color
      for x in 0..dimension.width-1 do
        for y in 0..dimension.height-1 do
          if TEXT_PIXELS.include? self[x,y]
            self[x,y] = BLACK_PIXEL
          else
            self[x,y] = WHITE_PIXEL
          end
        end
      end
    end
    
    def split separator_width
      return [] if @empty
      
      found_word = false
      start = 0
      current = 0
      empty_space = 0
      words = []

      while current < dimension.width
        if not found_word
          if not empty_column? current
            start = current
            found_word = true
            empty_space = 0
          end
        elsif empty_column? current
          empty_space += 1
          if empty_space == separator_width
            new_word = crop(start, 0, 
              current - start - separator_width + 1, dimension.height)
            new_word.trim
            words << new_word
            found_word = false
          end
        else
          empty_space = 0
        end
        current += 1
      end
      
      if found_word
        new_word = crop(start, 0, current-start, dimension.height)
        new_word.trim
        words << new_word
      end
      
      return words
    end
    
    def empty_row?(id) not row(id).include? BLACK_PIXEL end
    def empty_column?(id) not column(id).include? BLACK_PIXEL end
    
    # TMP FUNC ! (MAYBE)
    def corner
      y = dimension.height-1
      while y >=0 and self[0,y] == WHITE_PIXEL
        y = y - 1
      end
      return (dimension.height - 1 - y)
    end
    
    def [](x,y)
      if x >= dimension.width or y >= dimension.height or x < 0 or y < 0
        return WHITE_PIXEL
      end
      super
    end
    
  end
end




