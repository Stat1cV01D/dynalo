#pragma once

#include <string>
#include <stdexcept>
#include <filesystem>

#include <dlfcn.h>

namespace dynalo { namespace detail
{

inline
std::string last_error()
{
	return std::string(::dlerror());
}

namespace native
{

using handle = void*;

using string_type = std::filesystem::path::string_type;

inline handle invalid_handle() { return nullptr; }

namespace name
{

inline string_type prefix()    { return {"lib"}; }
inline string_type suffix()    { return {}; }
inline string_type extension() { return {".dylib"}; }

}

}

inline native::handle open(const std::filesystem::path& dyn_lib_path)
{
	native::handle lib_handle = ::dlopen(dyn_lib_path.c_str(), RTLD_LAZY);
	if (lib_handle == nullptr)
	{
        throw std::runtime_error(std::string("Failed to open [dyn_lib_path:") + dyn_lib_path.string() + "]: " + last_error());
	}

	return lib_handle;
}

inline
void close(native::handle lib_handle)
{
	const int rc = ::dlclose(lib_handle);
	if (rc != 0)
	{
		throw std::runtime_error(std::string("Failed to close the dynamic library: ") + last_error());
	}
}

template <typename FunctionSignature>
inline
FunctionSignature* get_function(native::handle lib_handle, const std::string& func_name)
{
	void* func_ptr = ::dlsym(lib_handle, func_name.c_str());
	if (func_ptr == nullptr)
	{
		throw std::runtime_error(std::string("Failed to get [func_name:") + func_name + "]: " + last_error());
	}

	return reinterpret_cast<FunctionSignature*>(func_ptr);
}

}}
