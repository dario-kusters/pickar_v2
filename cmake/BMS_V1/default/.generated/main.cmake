include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(BMS_V1_default_library_list )

# Handle files with suffix (s|as|asm|AS|ASM|As|aS|Asm), for group default-XC8
if(BMS_V1_default_default_XC8_FILE_TYPE_assemble)
add_library(BMS_V1_default_default_XC8_assemble OBJECT ${BMS_V1_default_default_XC8_FILE_TYPE_assemble})
    BMS_V1_default_default_XC8_assemble_rule(BMS_V1_default_default_XC8_assemble)
    list(APPEND BMS_V1_default_library_list "$<TARGET_OBJECTS:BMS_V1_default_default_XC8_assemble>")

endif()

# Handle files with suffix S, for group default-XC8
if(BMS_V1_default_default_XC8_FILE_TYPE_assemblePreprocess)
add_library(BMS_V1_default_default_XC8_assemblePreprocess OBJECT ${BMS_V1_default_default_XC8_FILE_TYPE_assemblePreprocess})
    BMS_V1_default_default_XC8_assemblePreprocess_rule(BMS_V1_default_default_XC8_assemblePreprocess)
    list(APPEND BMS_V1_default_library_list "$<TARGET_OBJECTS:BMS_V1_default_default_XC8_assemblePreprocess>")

endif()

# Handle files with suffix [cC], for group default-XC8
if(BMS_V1_default_default_XC8_FILE_TYPE_compile)
add_library(BMS_V1_default_default_XC8_compile OBJECT ${BMS_V1_default_default_XC8_FILE_TYPE_compile})
    BMS_V1_default_default_XC8_compile_rule(BMS_V1_default_default_XC8_compile)
    list(APPEND BMS_V1_default_library_list "$<TARGET_OBJECTS:BMS_V1_default_default_XC8_compile>")

endif()

# Handle files with suffix elf, for group default-XC8
if(BMS_V1_default_default_XC8_FILE_TYPE_objcopy_avr)
add_library(BMS_V1_default_default_XC8_objcopy_avr OBJECT ${BMS_V1_default_default_XC8_FILE_TYPE_objcopy_avr})
    BMS_V1_default_default_XC8_objcopy_avr_rule(BMS_V1_default_default_XC8_objcopy_avr)
    list(APPEND BMS_V1_default_library_list "$<TARGET_OBJECTS:BMS_V1_default_default_XC8_objcopy_avr>")

endif()


# Main target for this project
add_executable(BMS_V1_default_image_NDyzfZZ4 ${BMS_V1_default_library_list})

set_target_properties(BMS_V1_default_image_NDyzfZZ4 PROPERTIES
    OUTPUT_NAME "default"
    SUFFIX ".elf"
    ADDITIONAL_CLEAN_FILES "${output_extensions}"
    RUNTIME_OUTPUT_DIRECTORY "${BMS_V1_default_output_dir}")
target_link_libraries(BMS_V1_default_image_NDyzfZZ4 PRIVATE ${BMS_V1_default_default_XC8_FILE_TYPE_link})

#Add objcopy steps
BMS_V1_default_objcopy_avr_rule(BMS_V1_default_image_NDyzfZZ4)
# Add the link options from the rule file.
BMS_V1_default_link_rule( BMS_V1_default_image_NDyzfZZ4)


