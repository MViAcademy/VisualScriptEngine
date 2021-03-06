cmake_minimum_required (VERSION 3.6)

set (VSE_VERSION_1 0)
set (VSE_VERSION_2 3)
set (VSE_VERSION_3 5)
set (VSE_LIB_NAME VisualScriptEngine)

function (SetCompilerOptions module)
	target_compile_options (${module} PUBLIC "$<$<CONFIG:Debug>:-DDEBUG>")
	if (MSVC)
		target_compile_options (${module} PUBLIC /W4 /WX)
	else ()
		target_compile_options (${module} PUBLIC -std=c++11 -Wall -Wextra -Werror)
	endif ()
endfunction ()

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

set (CMAKE_SUPPRESS_REGENERATION 1)
set (CMAKE_CONFIGURATION_TYPES Debug;Release;RelWithDebInfo)
set (CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/DevKit CACHE PATH "Install prefix.")
set (CMAKE_DEBUG_POSTFIX "Debug")

add_definitions (-DUNICODE -D_UNICODE)

project (VisualScriptEngine)

enable_testing ()

# NodeEngine

set (NodeEngineSourcesFolder Sources/NodeEngine)
file (GLOB NodeEngineHeaderFiles ${NodeEngineSourcesFolder}/*.hpp)
file (GLOB NodeEngineSourceFiles ${NodeEngineSourcesFolder}/*.cpp)
set (
	NodeEngineFiles
	${NodeEngineHeaderFiles}
	${NodeEngineSourceFiles}
)
source_group ("Sources" FILES ${NodeEngineFiles})
add_library (NodeEngine STATIC ${NodeEngineFiles})
target_include_directories (NodeEngine PUBLIC ${NodeEngineSourcesFolder})
SetCompilerOptions (NodeEngine)
install (TARGETS NodeEngine DESTINATION lib)
install (FILES ${NodeEngineHeaderFiles} DESTINATION include)
install (FILES ${NodeEngineSourceFiles} DESTINATION source)

# NodeUIEngine

set (NodeUIEngineSourcesFolder Sources/NodeUIEngine)
configure_file (${NodeUIEngineSourcesFolder}/NUIE_VersionInfo.hpp.in ${PROJECT_BINARY_DIR}/NUIE_VersionInfo.hpp)
include_directories (${PROJECT_BINARY_DIR})
file (GLOB NodeUIEngineHeaderFiles ${NodeUIEngineSourcesFolder}/*.hpp ${PROJECT_BINARY_DIR}/NUIE_VersionInfo.hpp)
file (GLOB NodeUIEngineSourceFiles ${NodeUIEngineSourcesFolder}/*.cpp)
set (
	NodeUIEngineFiles
	${NodeUIEngineHeaderFiles}
	${NodeUIEngineSourceFiles}
)
source_group ("Sources" FILES ${NodeUIEngineFiles})
add_library (NodeUIEngine STATIC ${NodeUIEngineFiles})
target_include_directories (NodeUIEngine PUBLIC ${NodeEngineSourcesFolder} ${NodeUIEngineSourcesFolder})
target_link_libraries (NodeUIEngine NodeEngine)
SetCompilerOptions (NodeUIEngine)
install (TARGETS NodeUIEngine DESTINATION lib)
install (FILES ${NodeUIEngineHeaderFiles} DESTINATION include)
install (FILES ${NodeUIEngineSourceFiles} DESTINATION source)

# BuiltInNodes

set (BuiltInNodesSourcesFolder Sources/BuiltInNodes)
file (GLOB BuiltInNodesHeaderFiles ${BuiltInNodesSourcesFolder}/*.hpp)
file (GLOB BuiltInNodesSourceFiles ${BuiltInNodesSourcesFolder}/*.cpp)
set (
	BuiltInNodesFiles
	${BuiltInNodesHeaderFiles}
	${BuiltInNodesSourceFiles}
)
source_group ("Sources" FILES ${BuiltInNodesFiles})
add_library (BuiltInNodes STATIC ${BuiltInNodesFiles})
target_include_directories (BuiltInNodes PUBLIC ${NodeEngineSourcesFolder} ${NodeUIEngineSourcesFolder} ${BuiltInNodesSourcesFolder})
target_link_libraries (BuiltInNodes NodeEngine NodeUIEngine)
SetCompilerOptions (BuiltInNodes)
install (TARGETS BuiltInNodes DESTINATION lib)
install (FILES ${BuiltInNodesHeaderFiles} DESTINATION include)
install (FILES ${BuiltInNodesSourceFiles} DESTINATION source)

# NodeEngineTest

set (TestFrameworkSourcesFolder Sources/TestFramework)
set (NodeEngineTestSourcesFolder Sources/NodeEngineTest)
file (GLOB TestFrameworkFiles ${TestFrameworkSourcesFolder}/*.hpp ${TestFrameworkSourcesFolder}/*.cpp)
file (GLOB NodeEngineTestHeaderFiles ${NodeEngineTestSourcesFolder}/*.hpp)
file (GLOB NodeEngineTestSourceFiles ${NodeEngineTestSourcesFolder}/*.cpp)
set (
	NodeEngineTestTestFiles
	${NodeEngineTestHeaderFiles}
	${NodeEngineTestSourceFiles}
)
set (
	NodeEngineTestFiles
	${TestFrameworkFiles}
	${NodeEngineTestTestFiles}
)
source_group ("Framework" FILES ${TestFrameworkFiles})
source_group ("Sources" FILES ${NodeEngineTestTestFiles})
add_executable (NodeEngineTest ${NodeEngineTestFiles})
target_include_directories (NodeEngineTest PUBLIC ${NodeEngineSourcesFolder} ${NodeUIEngineSourcesFolder} ${BuiltInNodesSourcesFolder} ${TestFrameworkSourcesFolder})
target_link_libraries (NodeEngineTest NodeEngine NodeUIEngine BuiltInNodes)
get_filename_component (NodeEngineTestSourcesFolderAbsolute "${CMAKE_CURRENT_LIST_DIR}/${NodeEngineTestSourcesFolder}" ABSOLUTE)
add_custom_command (TARGET NodeEngineTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${NodeEngineTestSourcesFolderAbsolute}/VisualTestFiles $<TARGET_FILE_DIR:NodeEngineTest>/VisualTestFiles
)
SetCompilerOptions (NodeEngineTest)
add_test (NodeEngineTest NodeEngineTest)

# EmbeddingTutorial

set (EmbeddingTutorialSourcesFolder Sources/EmbeddingTutorial)
file (GLOB EmbeddingTutorialHeaderFiles ${EmbeddingTutorialSourcesFolder}/*.hpp)
file (GLOB EmbeddingTutorialSourceFiles ${EmbeddingTutorialSourcesFolder}/*.cpp)
set (
	EmbeddingTutorialFiles
	${EmbeddingTutorialHeaderFiles}
	${EmbeddingTutorialSourceFiles}
)
source_group ("Sources" FILES ${EmbeddingTutorialFiles})
add_executable (EmbeddingTutorial ${EmbeddingTutorialFiles})
target_include_directories (
	EmbeddingTutorial PUBLIC
	${NodeEngineSourcesFolder}
	${NodeUIEngineSourcesFolder}
	${BuiltInNodesSourcesFolder}
)
target_link_libraries (EmbeddingTutorial NodeEngine NodeUIEngine BuiltInNodes)
SetCompilerOptions (EmbeddingTutorial)
if (MSVC)
	target_compile_options (EmbeddingTutorial PUBLIC /wd4100)
else ()
	target_compile_options (EmbeddingTutorial PUBLIC -Wno-unused-parameter)
endif ()

if (MSVC)

	# WindowsAppSupport

	set (WindowsAppSupportSourcesFolder Sources/WindowsAppSupport)
	file (GLOB WindowsAppSupportHeaderFiles ${WindowsAppSupportSourcesFolder}/*.hpp)
	file (GLOB WindowsAppSupportSourceFiles ${WindowsAppSupportSourcesFolder}/*.cpp)
	set (
		WindowsAppSupportFiles
		${WindowsAppSupportHeaderFiles}
		${WindowsAppSupportSourceFiles}
	)
	source_group ("Sources" FILES ${WindowsAppSupportFiles})
	add_library (WindowsAppSupport STATIC ${WindowsAppSupportFiles})
	target_include_directories (WindowsAppSupport PUBLIC ${NodeEngineSourcesFolder} ${NodeUIEngineSourcesFolder} ${WindowsAppSupportSourcesFolder})
	target_link_libraries (WindowsAppSupport NodeEngine NodeUIEngine)
	SetCompilerOptions (WindowsAppSupport)
	install (TARGETS WindowsAppSupport DESTINATION lib)
	install (FILES ${WindowsAppSupportHeaderFiles} DESTINATION include)
	install (FILES ${WindowsAppSupportSourceFiles} DESTINATION source)

	# WindowsEmbeddingDemo

	set (WindowsEmbeddingDemoSourcesFolder Sources/WindowsEmbeddingDemo)
	file (GLOB WindowsEmbeddingDemoHeaderFiles ${WindowsEmbeddingDemoSourcesFolder}/*.hpp)
	file (GLOB WindowsEmbeddingDemoSourceFiles
		${WindowsEmbeddingDemoSourcesFolder}/*.cpp
		${WindowsEmbeddingDemoSourcesFolder}/*.rc
	)
	set (
		WindowsEmbeddingDemoFiles
		${WindowsEmbeddingDemoHeaderFiles}
		${WindowsEmbeddingDemoSourceFiles}
	)
	source_group ("Sources" FILES ${WindowsEmbeddingDemoFiles})
	add_executable (WindowsEmbeddingDemo WIN32 ${WindowsEmbeddingDemoFiles})
	target_include_directories (
		WindowsEmbeddingDemo PUBLIC
		${NodeEngineSourcesFolder}
		${NodeUIEngineSourcesFolder}
		${BuiltInNodesSourcesFolder}
		${TestFrameworkSourcesFolder}
		${WindowsAppSupportSourcesFolder}
	)
	target_link_libraries (WindowsEmbeddingDemo NodeEngine NodeUIEngine BuiltInNodes WindowsAppSupport)
	SetCompilerOptions (WindowsEmbeddingDemo)
	
	# WindowsEmbeddingDemo2

	set (WindowsEmbeddingDemo2SourcesFolder Sources/WindowsEmbeddingDemo2)
	file (GLOB WindowsEmbeddingDemo2HeaderFiles ${WindowsEmbeddingDemo2SourcesFolder}/*.hpp)
	file (GLOB WindowsEmbeddingDemo2SourceFiles ${WindowsEmbeddingDemo2SourcesFolder}/*.cpp)
	set (
		WindowsEmbeddingDemo2Files
		${WindowsEmbeddingDemo2HeaderFiles}
		${WindowsEmbeddingDemo2SourceFiles}
	)
	source_group ("Sources" FILES ${WindowsEmbeddingDemo2Files})
	add_executable (WindowsEmbeddingDemo2 WIN32 ${WindowsEmbeddingDemo2Files})
	target_include_directories (
		WindowsEmbeddingDemo2 PUBLIC
		${NodeEngineSourcesFolder}
		${NodeUIEngineSourcesFolder}
		${BuiltInNodesSourcesFolder}
		${TestFrameworkSourcesFolder}
		${WindowsAppSupportSourcesFolder}
	)
	target_link_libraries (WindowsEmbeddingDemo2 NodeEngine NodeUIEngine BuiltInNodes WindowsAppSupport)
	SetCompilerOptions (WindowsEmbeddingDemo2)

endif ()	
