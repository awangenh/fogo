FIND_PACKAGE(wxWidgets)

INCLUDE(${wxWidgets_USE_FILE})

INCLUDE_DIRECTORIES (
  ${IMGTOSEG_EXE_SOURCE_DIR}/Source
  ${LABELING_LIB_INCLUDE}
  ${wxWidgets_INCLUDE_DIRS}
)

ADD_EXECUTABLE (imgtoseg_exe 
  ${IMGTOSEG_EXE_SOURCE_DIR}/Source/App.cpp
  ${IMGTOSEG_EXE_SOURCE_DIR}/Source/argtable2/arg_date.c
  ${IMGTOSEG_EXE_SOURCE_DIR}/Source/argtable2/arg_dbl.c
  ${IMGTOSEG_EXE_SOURCE_DIR}/Source/argtable2/arg_end.c
  ${IMGTOSEG_EXE_SOURCE_DIR}/Source/argtable2/arg_file.c
  ${IMGTOSEG_EXE_SOURCE_DIR}/Source/argtable2/arg_int.c
  ${IMGTOSEG_EXE_SOURCE_DIR}/Source/argtable2/arg_lit.c
  ${IMGTOSEG_EXE_SOURCE_DIR}/Source/argtable2/arg_rem.c
  ${IMGTOSEG_EXE_SOURCE_DIR}/Source/argtable2/arg_rex.c
  ${IMGTOSEG_EXE_SOURCE_DIR}/Source/argtable2/arg_str.c
  ${IMGTOSEG_EXE_SOURCE_DIR}/Source/argtable2/argtable2.c
  ${IMGTOSEG_EXE_SOURCE_DIR}/Source/argtable2/getopt1.c
  ${IMGTOSEG_EXE_SOURCE_DIR}/Source/argtable2/getopt.c
)

LINK_DIRECTORIES (
  ${LABELING_LIB_DIR}
)

TARGET_LINK_LIBRARIES (imgtoseg_exe
  ${LABELING_LIB_DIR}/lib${LABELING_LIB}.a
  ${wxWidgets_LIBRARIES}
)