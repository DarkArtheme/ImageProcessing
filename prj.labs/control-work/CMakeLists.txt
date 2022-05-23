project(control-work)
add_executable(control-work control-work.cpp control-work.report.md.in.txt)
target_link_libraries(control-work ${OpenCV_LIBS})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/output DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(READ control-work.cpp cpp_source)
# prepare report md file
configure_file(control-work.report.md.in.txt ${CMAKE_CURRENT_BINARY_DIR}/control-work.report.md @ONLY)
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/control-work.report.md" "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
