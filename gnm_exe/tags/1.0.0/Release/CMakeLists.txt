FIND_PACKAGE(wxWidgets)

INCLUDE(${wxWidgets_USE_FILE})

INCLUDE_DIRECTORIES (
  ${GNM_EXE_SOURCE_DIR}/Include
  ${GNM_LIB_INCLUDE}
  ${LABELING_LIB_INCLUDE}
  ${FOGOUTILS_LIB_INCLUDE}
  ${wxWidgets_INCLUDE_DIRS}
)

ADD_EXECUTABLE (gnm 
  ${GNM_EXE_SOURCE_DIR}/Source/app/ConsoleGNM.cpp
  ${GNM_EXE_SOURCE_DIR}/Source/argtable2/arg_date.c
  ${GNM_EXE_SOURCE_DIR}/Source/argtable2/arg_dbl.c
  ${GNM_EXE_SOURCE_DIR}/Source/argtable2/arg_end.c
  ${GNM_EXE_SOURCE_DIR}/Source/argtable2/arg_file.c
  ${GNM_EXE_SOURCE_DIR}/Source/argtable2/arg_int.c
  ${GNM_EXE_SOURCE_DIR}/Source/argtable2/arg_lit.c
  ${GNM_EXE_SOURCE_DIR}/Source/argtable2/arg_rem.c
  ${GNM_EXE_SOURCE_DIR}/Source/argtable2/arg_rex.c
  ${GNM_EXE_SOURCE_DIR}/Source/argtable2/arg_str.c
  ${GNM_EXE_SOURCE_DIR}/Source/argtable2/argtable2.c
  ${GNM_EXE_SOURCE_DIR}/Source/argtable2/getopt1.c
  ${GNM_EXE_SOURCE_DIR}/Source/argtable2/getopt.c
)

LINK_DIRECTORIES (
  ${GNM_LIB_DIR}
  ${LABELING_LIB_DIR}
  ${FOGOUTILS_LIB_DIR}
)

TARGET_LINK_LIBRARIES (gnm
  ${GNM_LIB_DIR}/lib${GNM_LIB}.a
  ${LABELING_LIB_DIR}/lib${LABELING_LIB}.a
  ${FOGOUTILS_LIB_DIR}/lib${FOGOUTILS_LIB}.a
  ${wxWidgets_LIBRARIES}
)