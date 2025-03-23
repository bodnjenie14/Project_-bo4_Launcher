#include "io.hpp"
#include "nt.hpp"
#include <fstream>

namespace utils::io
{
	bool remove_file(const std::string& file)
	{
		if (DeleteFileA(file.data()) != FALSE)
		{
			return true;
		}

		return GetLastError() == ERROR_FILE_NOT_FOUND;
	}

	bool move_file(const std::string& src, const std::string& target)
	{
		return MoveFileA(src.data(), target.data()) == TRUE;
	}

	bool file_exists(const std::string& file)
	{
		return std::ifstream(file).good();
	}

	bool write_file(const std::string& file, const std::string& data, const bool append)
	{
		const auto pos = file.find_last_of("/\\");
		if (pos != std::string::npos)
		{
			create_directory(file.substr(0, pos));
		}

		std::ofstream stream(
			file, std::ios::binary | std::ofstream::out | (append ? std::ofstream::app : 0));

		if (stream.is_open())
		{
			stream.write(data.data(), data.size());
			stream.close();
			return true;
		}

		return false;
	}

	std::string read_file(const std::string& file)
	{
		std::string data;
		read_file(file, &data);
		return data;
	}

	bool read_file(const std::string& file, std::string* data)
	{
		if (!data) return false;
		data->clear();

		if (file_exists(file))
		{
			std::ifstream stream(file, std::ios::binary);
			if (!stream.is_open()) return false;

			stream.seekg(0, std::ios::end);
			const std::streamsize size = stream.tellg();
			stream.seekg(0, std::ios::beg);

			if (size > -1)
			{
				data->resize(static_cast<uint32_t>(size));
				stream.read(const_cast<char*>(data->data()), size);
				stream.close();
				return true;
			}
		}

		return false;
	}

	std::string file_name(const std::string& path)
	{
		const auto pos = path.find_last_of('/');
		if (pos == std::string::npos) return path;

		return path.substr(pos + 1);
	}

	std::string file_extension(const std::string& path)
	{
		const auto pos = path.find_last_of('.');
		if (pos == std::string::npos) return "";

		return path.substr(pos + 1);
	}

	size_t file_size(const std::string& file)
	{
		if (file_exists(file))
		{
			std::ifstream stream(file, std::ios::binary);

			if (stream.good())
			{
				stream.seekg(0, std::ios::end);
				return static_cast<size_t>(stream.tellg());
			}
		}

		return 0;
	}

	time_t file_timestamp(const std::string& /*file*/) // ERROR ON VPS BUT WHY!?
	{
		//const auto time = std::chrono::clock_cast<std::chrono::system_clock>(std::filesystem::last_write_time(file));

		//return std::chrono::system_clock::to_time_t(time);

		return time(nullptr);
	}

	bool create_directory(const std::string& directory)
	{
		return std::filesystem::create_directories(directory);
	}

	bool directory_exists(const std::string& directory)
	{
		return std::filesystem::is_directory(directory);
	}

	bool directory_is_empty(const std::string& directory)
	{
		return std::filesystem::is_empty(directory);
	}

	std::vector<std::string> list_files(const std::string& directory)
	{
		std::vector<std::string> files;

		for (auto& file : std::filesystem::directory_iterator(directory))
		{
			if (std::filesystem::is_directory(file.path())) continue; //exclude directory from output

			files.push_back(file.path().generic_string());
		}

		return files;
	}

	void copy_folder(const std::filesystem::path& src, const std::filesystem::path& target)
	{
		std::filesystem::copy(src, target,
			std::filesystem::copy_options::overwrite_existing |
			std::filesystem::copy_options::recursive);
	}
}