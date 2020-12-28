#QUADMATH
FIND_PACKAGE(QUADMATH QUIET)
IF(QUADMATH_FOUND)
  LIST(APPEND ALL_LIBRARIES ${QUADMATH_LIBRARIES})
  MESSAGE(STATUS "Found QUADMATH @ ${QUADMATH_LIBRARIES}")
  IF(USE_QUADMATH)
    ADD_DEFINITIONS(-DQUADMATH_SUPPORT)
  ENDIF()
  ADD_DEFINITIONS(-DFOUND_QUADMATH)
ELSE(QUADMATH_FOUND)
  MESSAGE(SEND_ERROR "Cannot find Quadmath!")
ENDIF(QUADMATH_FOUND)

#EIGEN3
FIND_PACKAGE(Eigen3 QUIET)
IF(EIGEN3_FOUND)
  MESSAGE(STATUS "Found EIGEN3 @ ${EIGEN3_INCLUDE_DIR}")
  INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})
ELSE(EIGEN3_FOUND)
  IF(EXISTS ${PROJECT_BINARY_DIR}/eigen)
  ELSE()
    EXECUTE_PROCESS(COMMAND git clone https://gitlab.com/libeigen/eigen.git WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
  ENDIF()
  MESSAGE(STATUS "Using EIGEN3 @ ${PROJECT_BINARY_DIR}/eigen")
  SET(EIGEN3_INCLUDE_DIR ${PROJECT_BINARY_DIR}/eigen)
  FIND_PACKAGE(Eigen3 QUIET REQUIRED)
  INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})
ENDIF(EIGEN3_FOUND)

#TinyXML2
FIND_PACKAGE(TINYXML2 QUIET)
IF(TINYXML2_FOUND)
  INCLUDE_DIRECTORIES(${TINYXML2_INCLUDE_DIRS})
  MESSAGE(STATUS "Found TinyXML2 @ ${TINYXML2_INCLUDE_DIRS}")
  LIST(APPEND ALL_LIBRARIES ${TINYXML2_LIBRARIES})
ELSE(TINYXML2_FOUND)
  MESSAGE(SEND_ERROR "Cannot find TinyXML2!")
ENDIF(TINYXML2_FOUND)

#GMP
FIND_PACKAGE(GMP QUIET REQUIRED)
IF(GMP_FOUND)
  MESSAGE(STATUS "Found GMP @ ${GMP_INCLUDES}")
  LIST(APPEND ALL_LIBRARIES ${GMP_LIBRARIES})
ELSEIF(GMP_FOUND)
  MESSAGE(SEND_ERROR "Cannot find GMP!")
ENDIF(GMP_FOUND)

#FIND MPFR
FIND_PACKAGE(MPFR QUIET REQUIRED)
IF(MPFR_FOUND)
  INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/ThirdParty")
  INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/ThirdParty/mpfr-4.0.1/include")
  MESSAGE(STATUS "Found MPFR Libraries @ ${MPFR_LIBRARIES}")
  MESSAGE(STATUS "Found MPFR @ ${MPFR_INCLUDES}")
  LIST(APPEND ALL_LIBRARIES ${MPFR_LIBRARIES})
ELSEIF(MPFR_FOUND)
  MESSAGE(SEND_ERROR "Cannot find MPFR!")
ENDIF(MPFR_FOUND)

#Assimp
FIND_PACKAGE(assimp QUIET REQUIRED)
IF(assimp_FOUND)
  INCLUDE_DIRECTORIES(${assimp_INCLUDE_DIRS})
  MESSAGE(STATUS "Found assimp @ ${assimp_INCLUDE_DIRS}")
  LIST(APPEND ALL_LIBRARIES ${assimp_LIBRARIES})
ELSE(assimp_FOUND)
  MESSAGE(SEND_ERROR "Cannot find assimp!")
ENDIF(assimp_FOUND)

#CGAL
IF(WITH_CGAL)
  #CGAL depends on BOOST
  SET(Boost_USE_MULTITHREADED ON)
  FIND_PACKAGE(Boost QUIET REQUIRED filesystem system iostreams timer chrono random)
  IF(Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    MESSAGE(STATUS "Found BOOST @ ${Boost_INCLUDE_DIR}")
    LIST(APPEND ALL_LIBRARIES ${Boost_LIBRARIES})
  ELSE(Boost_FOUND)
    MESSAGE(SEND_ERROR "Cannot find BOOST")
  ENDIF(Boost_FOUND)

  FIND_PACKAGE(CGAL)
  FIND_PACKAGE(GMP)
  FIND_PACKAGE(MPFR)
  IF(CGAL_FOUND AND GMP_FOUND AND MPFR_FOUND)
    INCLUDE_DIRECTORIES(${CGAL_INCLUDE_DIR} ${GMP_INCLUDES} ${MPFR_INCLUDES})
    MESSAGE(STATUS "Found CGAL @ ${CGAL_INCLUDE_DIR} & ${GMP_INCLUDES} & ${MPFR_INCLUDES}")
    LIST(APPEND ALL_LIBRARIES ${CGAL_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES})
    ADD_DEFINITIONS(-DCGAL_SUPPORT)
  ELSE()
    MESSAGE(WARNING "Cannot find CGAL/GMP/MPFR, 3D meshing not supported!")
  ENDIF()
ENDIF()

IF(WITH_OPTIMIZER)
  FIND_PACKAGE(QPOASES QUIET REQUIRED)
  IF(QPOASES_FOUND)
    MESSAGE(STATUS "Found QPOASES @ ${QPOASES_INCLUDES}")
    LIST(APPEND ALL_LIBRARIES ${QPOASES_LIBRARIES})
  ELSEIF(QPOASES_FOUND)
    MESSAGE(SEND_ERROR "Cannot find QPOASES!")
  ENDIF(QPOASES_FOUND)
ENDIF(WITH_OPTIMIZER)

