INCLUDE_DIRECTORIES (
  ${FOGOUTILS_LIB_SOURCE_DIR}/Include)

ADD_LIBRARY (fogoutils 
  ${FOGOUTILS_LIB_SOURCE_DIR}/Source/Color/ColorspaceUtil.cpp
  ${FOGOUTILS_LIB_SOURCE_DIR}/Source/Verbose/VerboseUtil.cpp)