file(REMOVE_RECURSE
  "libbusiness.a"
  "libbusiness.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/business.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
