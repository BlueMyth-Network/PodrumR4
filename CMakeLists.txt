cmake_minimum_required(VERSION 3.0)

project(PodrumR4)
find_package(OpenSSL REQUIRED)

set(CMAKE_C_STANDARD 11)
set(USE_LEAK_DETECTOR 0)

file(GLOB CBINARYSTREAM cbinarystream/*.c)
file(GLOB CNBT cnbt/*.c)
file(GLOB CZLIBHELPER czlibhelper/*.c)
file(GLOB MINIZ miniz/*.c)
file(GLOB PODRUM podrum/*.c podrum/misc/*.c podrum/world/*.c podrum/world/format/anvil/*.c podrum/world/chunk/*.c podrum/command/*.c podrum/network/minecraft/*.c podrum/network/raknet/*.c)
file(GLOB TINYAES tinyaes/*.c)
file(GLOB ECC ecc/*.c)
file(GLOB SHA sha256/*.c)

include_directories(.)

IF("${CMAKE_SYSTEM}" MATCHES "Windows")
		if("${USE_LEAK_DETECTOR}" MATCHES "1")
				include_directories("c:\\Program Files (x86)\\Visual Leak Detector\\include")
				link_directories("c:\\Program Files (x86)\\Visual Leak Detector\\lib\\Win64")
		endif()
endif()

add_executable(PodrumR4
        ${CBINARYSTREAM}
        ${CNBT}
        ${CZLIBHELPER}
        ${MINIZ}
        ${PODRUM}
		${TINYAES}
		${SHA}
		${ECC}
		)
target_link_libraries(PodrumR4 OpenSSL::SSL)
target_link_libraries(PodrumR4 OpenSSL::Crypto)

IF(NOT ("${CMAKE_SYSTEM}" MATCHES "Windows"))
	TARGET_LINK_LIBRARIES(PodrumR4 pthread)
	TARGET_LINK_LIBRARIES(PodrumR4 m)
endif()
