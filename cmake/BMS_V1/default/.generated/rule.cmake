# The following functions contains all the flags passed to the different build stages.

set(PACK_REPO_PATH "C:/Users/dku/.mchp_packs" CACHE PATH "Path to the root of a pack repository.")

function(BMS_V1_default_default_XC8_assemble_rule target)
    set(options
        "-c"
        "${MP_EXTRA_AS_PRE}"
        "-mcpu=AVR64DD14"
        "-g"
        "-x"
        "assembler-with-cpp"
        "-mdfp=${PACK_REPO_PATH}/Microchip/AVR-Dx_DFP/2.8.343/xc8"
        "-Wl,--gc-sections"
        "-O1"
        "-ffunction-sections"
        "-fdata-sections"
        "-fshort-enums"
        "-fno-common"
        "-funsigned-char"
        "-funsigned-bitfields"
        "-Wall"
        "-L${CMAKE_CURRENT_SOURCE_DIR}/../../.."
        "-gdwarf-3"
        "-mconst-data-in-progmem"
        "-mno-const-data-in-config-mapped-progmem"
        "-Wa,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__AVR64DD14__"
        PRIVATE "__DEBUG=1"
        PRIVATE "DEBUG"
        PRIVATE "XPRJ_default=default")
    target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
endfunction()
function(BMS_V1_default_default_XC8_assemblePreprocess_rule target)
    set(options
        "-c"
        "${MP_EXTRA_AS_PRE}"
        "-mcpu=AVR64DD14"
        "-g"
        "-x"
        "assembler-with-cpp"
        "-mdfp=${PACK_REPO_PATH}/Microchip/AVR-Dx_DFP/2.8.343/xc8"
        "-Wl,--gc-sections"
        "-O1"
        "-ffunction-sections"
        "-fdata-sections"
        "-fshort-enums"
        "-fno-common"
        "-funsigned-char"
        "-funsigned-bitfields"
        "-Wall"
        "-L${CMAKE_CURRENT_SOURCE_DIR}/../../.."
        "-gdwarf-3"
        "-mconst-data-in-progmem"
        "-mno-const-data-in-config-mapped-progmem"
        "-Wa,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__AVR64DD14__"
        PRIVATE "__DEBUG=1"
        PRIVATE "DEBUG"
        PRIVATE "XPRJ_default=default")
    target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
endfunction()
function(BMS_V1_default_default_XC8_compile_rule target)
    set(options
        "-c"
        "${MP_EXTRA_CC_PRE}"
        "-mcpu=AVR64DD14"
        "-x"
        "c"
        "-mdfp=${PACK_REPO_PATH}/Microchip/AVR-Dx_DFP/2.8.343/xc8"
        "-Wl,--gc-sections"
        "-O1"
        "-ffunction-sections"
        "-fdata-sections"
        "-fshort-enums"
        "-fno-common"
        "-funsigned-char"
        "-funsigned-bitfields"
        "-Wall"
        "-L${CMAKE_CURRENT_SOURCE_DIR}/../../.."
        "-gdwarf-3"
        "-mconst-data-in-progmem"
        "-mno-const-data-in-config-mapped-progmem")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__AVR64DD14__"
        PRIVATE "__DEBUG=1"
        PRIVATE "DEBUG"
        PRIVATE "XPRJ_default=default")
    target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
endfunction()
function(BMS_V1_default_link_rule target)
    set(options
        "-Wl,-Map=mem.map"
        "${MP_EXTRA_LD_PRE}"
        "-mcpu=AVR64DD14"
        "-Wl,--defsym=__MPLAB_BUILD=1"
        "-mdfp=${PACK_REPO_PATH}/Microchip/AVR-Dx_DFP/2.8.343/xc8"
        "-Wl,--gc-sections"
        "-O1"
        "-ffunction-sections"
        "-fdata-sections"
        "-fshort-enums"
        "-fno-common"
        "-funsigned-char"
        "-funsigned-bitfields"
        "-Wall"
        "-L${CMAKE_CURRENT_SOURCE_DIR}/../../.."
        "-gdwarf-3"
        "-mconst-data-in-progmem"
        "-mno-const-data-in-config-mapped-progmem"
        "-Wl,--memorysummary,memoryfile.xml"
        "-Wl,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1")
    list(REMOVE_ITEM options "")
    target_link_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__DEBUG=1"
        PRIVATE "XPRJ_default=default")
    target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
endfunction()
function(BMS_V1_default_objcopy_avr_rule target)
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND ${OBJCOPY}
        ARGS -O ihex ${BMS_V1_default_image_name} ${BMS_V1_default_image_base_name}.hex
        WORKING_DIRECTORY ${BMS_V1_default_output_dir})
endfunction()
