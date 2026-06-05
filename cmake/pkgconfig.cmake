configure_file(
  pkgconfig/ns.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/ns.pc
  @ONLY
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ns.pc
  DESTINATION lib/pkgconfig
)
