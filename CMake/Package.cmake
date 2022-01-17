set(_${PROJECT_NAME}_have_dependencies 0)

function(ToPackageName rst name version)
  set(tmp "${name}_${version}")
  string(REPLACE "." "_" tmp ${tmp})
  set(${rst} "${tmp}" PARENT_SCOPE)
endfunction()

function(PackageName rst)
  ToPackageName(tmp ${PROJECT_NAME} ${PROJECT_VERSION})
  set(${rst} ${tmp} PARENT_SCOPE)
endfunction()
