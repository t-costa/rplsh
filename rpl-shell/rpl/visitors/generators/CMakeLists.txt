# Create library
project (generators)

set (
  generator_files
  ffcode
  ffmapper
)

add_library(generators ${generator_files})
