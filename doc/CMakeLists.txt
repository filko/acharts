add_custom_command(
  OUTPUT   ${CMAKE_CURRENT_BINARY_DIR}/acharts.1.xml
  DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/acharts.1.txt
  COMMAND  ${ASCIIDOC_EXECUTABLE} ARGS --doctype=manpage --backend=docbook --out-file=${CMAKE_CURRENT_BINARY_DIR}/acharts.1.xml ${CMAKE_CURRENT_SOURCE_DIR}/acharts.1.txt
  COMMENT  "Preparing xml for manpage"
)
add_custom_command(
  OUTPUT   ${CMAKE_CURRENT_BINARY_DIR}/acharts.1
  DEPENDS  ${CMAKE_CURRENT_BINARY_DIR}/acharts.1.xml
  COMMAND  xmlto ARGS man ${CMAKE_CURRENT_BINARY_DIR}/acharts.1.xml
  COMMENT  "Building manpage"
)
add_custom_target(doc-manpage ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/acharts.1)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/acharts.1 DESTINATION ${SHAREDIR}/man/man1)
