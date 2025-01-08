target_sources(SRB2SDL2 PRIVATE
	handle.hpp
	rhi.cpp
	rhi.hpp
	shader_load_context.cpp
	shader_load_context.hpp
)

if("${SRB2_CONFIG_GLES2}")
	option(SRB2_CONFIG_GLES3 "Enable GLES3 extentions in the GLES backend" OFF)
	if("${SRB2_CONFIG_GLES3}")
		target_compile_definitions(SRB2SDL2 PRIVATE -DHAVE_GLES3)
	endif()
	add_subdirectory(gles2)
else()
	add_subdirectory(gl2)
endif()
