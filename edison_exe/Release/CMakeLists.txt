FIND_PACKAGE(wxWidgets)

INCLUDE(${wxWidgets_USE_FILE})

INCLUDE_DIRECTORIES (
  ${CSC_EXE_SOURCE_DIR}/Include
  ${FOGOUTILS_LIB_INCLUDE}
  ${wxWidgets_INCLUDE_DIRS})

ADD_EXECUTABLE (csc_exe 
  ${CSC_EXE_SOURCE_DIR}/Source/App.cpp
  ${CSC_EXE_SOURCE_DIR}/Source/kipl-transform/color-transform.cpp
  ${CSC_EXE_SOURCE_DIR}/Source/kipl-base/image.cpp
  ${CSC_EXE_SOURCE_DIR}/Source/kipl-base/base.cpp
  ${CSC_EXE_SOURCE_DIR}/Source/kipl-base/loadsavemanager.cpp
  ${CSC_EXE_SOURCE_DIR}/Source/kipl-base/error.cpp
  ${CSC_EXE_SOURCE_DIR}/Source/kipl-csc/islhierarchy.cpp
  ${CSC_EXE_SOURCE_DIR}/Source/kipl-csc/trafficsigncolsim.cpp
  ${CSC_EXE_SOURCE_DIR}/Source/kipl-segmentation/segm-stat.cpp
)

LINK_DIRECTORIES (
  ${FOGOUTILS_LIB_DIR}
)

TARGET_LINK_LIBRARIES (csc_exe
  ${FOGOUTILS_LIB_DIR}/lib${FOGOUTILS_LIB}.a
  ${wxWidgets_LIBRARIES})